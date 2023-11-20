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

#include "graphics/MaterialHandler.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/TextureManager.h"
#include "resources/GpuProgramManager.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterialHandler::cMaterialHandler(iLowLevelGraphics *llGfx, cTextureManager* textureManager, cGpuProgramManager* programManager)
	: _llGfx{llGfx}, _textureManager{textureManager}, _programManager{programManager}
	{
	}

	cMaterialHandler::~cMaterialHandler() {
		for (auto matType : _materialTypes) {
			delete matType;
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMaterialHandler::AddType(iMaterialType* matType)
	{
		_materialTypes.push_back(matType);
	}

	//-----------------------------------------------------------------------

	iMaterial* cMaterialHandler::Create(const tString& asMatName)
	{
		return Create("", asMatName);
	}

	iMaterial* cMaterialHandler::Create(const tString& asName,tString asMatName)
	{
		iMaterial* pMat=NULL;
		unsigned int lIdCount = 1;

		for(auto &matType : _materialTypes)
		{
			if(matType->IsCorrect(asMatName))
			{
				pMat = matType->Create(asName, _llGfx, _textureManager, _programManager);

				//Set an id to the material for easier rendering later on.
				pMat->SetId(lIdCount);

				break;
			}

			lIdCount++;
		}

		return pMat;
	}

	//-----------------------------------------------------------------------
}
