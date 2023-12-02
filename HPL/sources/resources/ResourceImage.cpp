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
#include "resources/FrameBitmap.h"
#include "graphics/Texture.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

#define kContractSize (0.001f)

	cResourceImage::cResourceImage(tString asName, std::shared_ptr<cFrameBitmap> frameBmp, cRect2l aRect, cVector2l avSrcSize)
	: iResourceBase(asName)
	, _frameBitmap{frameBmp}
	{
		mRect = aRect;

		auto sourceSize = cVector2f{(float)avSrcSize.x, (float)avSrcSize.y};
		auto vTexSize   = cVector2f{(float)mRect.w, (float)mRect.h} / sourceSize;
		auto vTexPos    = cVector2f{(float)mRect.x, (float)mRect.y} / sourceSize;

		_uvs = {
			.uv0 = { vTexPos.x + kContractSize, vTexPos.y + kContractSize },
			.uv1 = { vTexPos.x + vTexSize.x - kContractSize, vTexPos.y + kContractSize },
			.uv2 = { vTexPos.x + vTexSize.x - kContractSize, vTexPos.y + vTexSize.y - kContractSize },
			.uv3 = { vTexPos.x + kContractSize, vTexPos.y + vTexSize.y - kContractSize }
		};
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iTexture *cResourceImage::GetTexture() const
	{
		return GetFrameBitmap()->GetTexture();
	}

	//-----------------------------------------------------------------------

}
