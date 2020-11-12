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

#include "graphics/LowLevelGraphics.h"
#include "resources/ResourceImage.h"
#include "resources/ImageManager.h"
#include "graphics/GfxObject.h"
#include "graphics/Material.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// iOldMaterial
	//////////////////////////////////////////////////////////////////////////

	iOldMaterial::iOldMaterial(eOldMaterialType aiType, cImageManager* apImageManager)
		: mType(aiType), mpImageManager(apImageManager)
	{
	}

	iOldMaterial::~iOldMaterial() {
		if (mpImage) {
			mpImageManager->Destroy(mpImage);
		}
	}

	iTexture* iOldMaterial::GetTexture() const {
		if (mpImage == NULL) {
			return NULL;
		}
		return mpImage->GetTexture();
	}


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	typedef std::vector<cGfxObject> tGfxObjectVec;
	typedef tGfxObjectVec::iterator tGfxObjectVecIt;

	//-----------------------------------------------------------------------

	cGfxObject::cGfxObject(iOldMaterial* apMat, const tString& asFile)
		: mpMat(apMat)
		, msSourceFile(asFile)
	{
		mvVtx = apMat->GetImage()->GetVertexVecCopy(0,-1);
	}

	//-----------------------------------------------------------------------

	cGfxObject::~cGfxObject()
	{
		hplDelete(mpMat);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

}
