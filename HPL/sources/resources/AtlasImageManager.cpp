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
#include "resources/AtlasImageManager.h"
#include "resources/ResourceImage.h"
#include "resources/FrameBitmap.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/LoadImage.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cAtlasImageManager::cAtlasImageManager(iLowLevelGraphics *llGfx)
	: iResourceManager{"atlas_image"}
	{
	   _llGfx = llGfx;
	   GetSupportedImageFormats(_fileFormats);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cAtlasImageManager::LoadAsset(const tString &name, const tString &fullPath) {
		auto bitmap = LoadBitmapFile(fullPath);
		if (! bitmap) {
			Error("AtlasImageManager Couldn't load bitmap '%s'\n", fullPath.c_str());
			return nullptr;
		}

		auto image = AddToFrame(name, *bitmap);
		if (image == nullptr) {
			Error("AtlasImageManager couldn't frame image '%s'\n", name.c_str());
		}
		return image;
	}

	std::span<const tString> cAtlasImageManager::SupportedExtensions() const {
		return { _fileFormats };
	}

	//-----------------------------------------------------------------------

	cResourceImage* cAtlasImageManager::CreateImage(const tString& name)
	{
		return static_cast<cResourceImage*>(GetOrLoadResource(name));
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	constexpr int FRAME_DIM = 512;
	static int s_FrameIndex = 0;

	cResourceImage *cAtlasImageManager::AddToFrame(const tString &name, const Bitmap &aBmp)
	{
		cRect2l destRect;

		// check if any non-full frames can fit the bitmap
		auto frameIter = std::find_if(_currentFrames.begin(), _currentFrames.end(), [&](std::shared_ptr<cFrameBitmap>& frame) {
			destRect = frame->AddBitmap(aBmp);
			return destRect.w > 0;
		});

		if (frameIter == _currentFrames.end()) {
			// No applicable frame found. Append new frame to end, update iterator
			Log("RESIMG: No frame with space found, creating new #%d\n", s_FrameIndex);
			iTexture *pTex = _llGfx->CreateTexture(tString{"FRAME_"} + std::to_string(s_FrameIndex), eTextureTarget_2D);
			auto newFrame = std::make_shared<cFrameBitmap>(FRAME_DIM, FRAME_DIM, pTex, s_FrameIndex++);
			frameIter = _currentFrames.insert(_currentFrames.end(), newFrame);

			// try inserting the bitmap into the new frame
			destRect = newFrame->AddBitmap(aBmp);
		}

		if (destRect.w == 0) {
			Error("AtlasImageManager: unable to insert bitmap into any frame!\n");
			return nullptr;
		}

		// Log("RESIMG: Added bitmap of size [%d, %d] to frame %d\n", aBmp.GetWidth(), aBmp.GetHeight(), (*frameIter)->GetIndex());
		auto image = new cResourceImage(name, *frameIter, destRect, cVector2l{FRAME_DIM, FRAME_DIM});

		// did this new image fill up the frame we found, then remove from active frames
		if ((*frameIter)->IsFull()) {
			// Log("RESIMG: Frame %d was full, no longer tracking\n", (*frameIter)->GetIndex());
			_currentFrames.erase(frameIter);
		}

		return image;
	}

	//-----------------------------------------------------------------------
}

