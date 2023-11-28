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
#include "resources/ResourceImage.h"
#include "resources/FrameTexture.h"
#include "graphics/Texture.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

#define kContractSize (0.001f)

	cResourceImage::cResourceImage(tString asName,cFrameTexture *apFrameTex,
		cFrameBitmap *apFrameBmp, cRect2l aRect,
		cVector2l avSrcSize, int alHandle) : iResourceBase(asName)
	{
		mpFrameTexture = apFrameTex;
		mpFrameBitmap = apFrameBmp;
		mRect = aRect;
		mvSourceSize = avSrcSize;
		mlHandle = alHandle;

		cVector2f vTexSize = cVector2f((float)mRect.w,(float)mRect.h ) /
								cVector2f((float)mvSourceSize.x,(float)mvSourceSize.y);
		cVector2f vTexPos = cVector2f((float)mRect.x,(float)mRect.y ) /
								cVector2f((float)mvSourceSize.x,(float)mvSourceSize.y);

		_uvs.reserve(4);
		_uvs.emplace_back(vTexPos.x + kContractSize, vTexPos.y + kContractSize);
		_uvs.emplace_back(vTexPos.x + vTexSize.x - kContractSize, vTexPos.y + kContractSize );
		_uvs.emplace_back(vTexPos.x + vTexSize.x - kContractSize, vTexPos.y + vTexSize.y - kContractSize);
		_uvs.emplace_back(vTexPos.x + kContractSize, vTexPos.y + vTexSize.y - kContractSize);
	}

	//-----------------------------------------------------------------------

	cResourceImage::~cResourceImage()
	{
		//mpFrameTexture->DecPicCount();
		mpFrameTexture = NULL;
		mlHandle = -1;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iTexture *cResourceImage::GetTexture() const
	{
		return mpFrameTexture->GetTexture();
	}

	//-----------------------------------------------------------------------

}
