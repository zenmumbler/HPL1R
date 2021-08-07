/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "resources/impl/LowLevelResourcesSDL.h"
#include "graphics/impl/SDLBitmap2D.h"
#include "resources/impl/MeshLoaderMSH.h"
#include "resources/impl/MeshLoaderCollada.h"
#ifdef INCLUDE_THEORA
#include "graphics/impl/VideoStreamTheora.h"
#endif
#include "impl/Platform.h"
#include "system/String.h"

#include <SDL2/SDL.h>
#include "resources/MeshLoaderHandler.h"
#include "resources/VideoManager.h"
#include "system/LowLevelSystem.h"

#include "stb/stb_image.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelResourcesSDL::cLowLevelResourcesSDL(cLowLevelGraphicsSDL *apLowLevelGraphics)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
	}

	//-----------------------------------------------------------------------

	cLowLevelResourcesSDL::~cLowLevelResourcesSDL()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iBitmap2D* cLowLevelResourcesSDL::LoadBitmap2D(tString asFilePath, tString asType)
	{
		tString tType;
		if(asType != "") {
			asFilePath = cString::SetFileExt(asFilePath,asType);
		}
		tType = cString::GetFileExt(asFilePath);
		
		int width, height, chans;
		const auto pixels = stbi_load(asFilePath.c_str(), &width, &height, &chans, 4);
				
		if (pixels == nullptr) {
			Error("Failed to load image: %s!\n", asFilePath.c_str());
			return nullptr;
		}

		SDL_Surface* pSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
//		memcpy(pSurface->pixels, pixels, width * height * 4);
		
		// [ZM] apply the same hack fix for now until I factor out the SDL_surface nonsense
		uint32_t* pDst = static_cast<uint32_t*>(pSurface->pixels);
		uint32_t* pSrc = reinterpret_cast<uint32_t*>(pixels);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				auto pixIn = *pSrc;
				
				// flip R and G RGBA -> BGRA
				auto pixOut = (pixIn & 0xff00ff00) | ((pixIn & 0xff) << 16) | ((pixIn & 0xff0000) >> 16);
				*pDst = pixOut;

				pSrc += 1;
				pDst += 1;
			}
		}
		
		stbi_image_free(pixels);
		
//		Log("Image: %s - %d x %d - Chans: %d - Format = %s\n", asFilePath.c_str(), width, height, chans, SDL_GetPixelFormatName(pSurface->format->format));

		iBitmap2D* pBmp = mpLowLevelGraphics->CreateBitmap2DFromSurface(pSurface, cString::GetFileExt(asFilePath));
		pBmp->SetPath(asFilePath);

		return pBmp;
	}

	//-----------------------------------------------------------------------

	void cLowLevelResourcesSDL::GetSupportedImageFormats(tStringList &alstFormats)
	{
		alstFormats.push_back("BMP");
		alstFormats.push_back("JPG");
		alstFormats.push_back("JPEG");
		alstFormats.push_back("PNG");
		alstFormats.push_back("TGA");
	}
	//-----------------------------------------------------------------------

	void cLowLevelResourcesSDL::AddMeshLoaders(cMeshLoaderHandler* apHandler)
	{
		//apHandler->AddLoader(hplNew( cMeshLoaderFBX,(mpLowLevelGraphics)));
		apHandler->AddLoader(hplNew( cMeshLoaderMSH,(mpLowLevelGraphics)));
		apHandler->AddLoader(hplNew( cMeshLoaderCollada,(mpLowLevelGraphics)));
	}

	//-----------------------------------------------------------------------

	void cLowLevelResourcesSDL::AddVideoLoaders(cVideoManager* apManager)
	{
		#ifdef INCLUDE_THORA
		apManager->AddVideoLoader(hplNew( cVideoStreamTheora_Loader,()));
		#endif
	}

	//-----------------------------------------------------------------------

	//This is a windows implementation of this...I think.
	void cLowLevelResourcesSDL::FindFilesInDir(tWStringList &alstStrings,tWString asDir, tWString asMask)
	{
		Platform::FindFileInDir(alstStrings, asDir,asMask);
	}

	//-----------------------------------------------------------------------

}
