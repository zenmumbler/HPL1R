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
#include "resources/impl/MeshLoaderMSH.h"
#include "resources/impl/MeshLoaderGLTF2.h"
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

	Maybe<Bitmap> cLowLevelResourcesSDL::LoadBitmap2D(tString asFilePath, tString asType)
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
			return std::nullopt;
		}

		Bitmap bmp;
		if (! bmp.CreateFromPixels(pixels, width, height)) {
			Error("Failed to create bitmap for image: %s!\n", asFilePath.c_str());
			stbi_image_free(pixels);
			return std::nullopt;
		}
		bmp.TogglePixelFormat();
		bmp.SetPath(asFilePath);
		stbi_image_free(pixels);
		
//		Log("Image: %s - %d x %d - Chans: %d\n", asFilePath.c_str(), width, height, chans);

		return bmp;
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
		apHandler->AddLoader(hplNew( cMeshLoaderGLTF2,(mpLowLevelGraphics)));
		apHandler->AddLoader(hplNew( cMeshLoaderCollada,(mpLowLevelGraphics)));
	}

	//-----------------------------------------------------------------------

	void cLowLevelResourcesSDL::AddVideoLoaders(cVideoManager* apManager)
	{
		#ifdef INCLUDE_THEORA
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
