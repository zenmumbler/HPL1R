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
#include "graphics/Graphics.h"
#include "resources/Resources.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterialHandler::cMaterialHandler(cGraphics* apGraphics, cResources* apResources)
	{
		mpGraphics = apGraphics;
		mpResources = apResources;
	}

	//-----------------------------------------------------------------------

	cMaterialHandler::~cMaterialHandler()
	{
		tMaterialTypeListIt it = mlstMatTypes.begin();
		for(;it!=mlstMatTypes.end();it++)
		{
			hplDelete(*it);
		}

		mlstMatTypes.clear();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMaterialHandler::Add(iMaterialType* apTypedata)
	{
		mlstMatTypes.push_back(apTypedata);
	}

	//-----------------------------------------------------------------------
	iMaterial* cMaterialHandler::Create(tString asMatName)
	{
		return Create("", asMatName);
	}

	iMaterial* cMaterialHandler::Create(const tString& asName,tString asMatName)
	{
		iMaterial* pMat=NULL;
		unsigned int lIdCount = 1;

		for(tMaterialTypeListIt it = mlstMatTypes.begin(); it!=mlstMatTypes.end();it++)
		{
			if((*it)->IsCorrect(asMatName))
			{
				pMat = (*it)->Create(asName,mpGraphics->GetLowLevel(),
									mpResources->GetTextureManager(),
									mpResources->GetGpuProgramManager(),
									mpGraphics->GetRenderer3D());

				//Set an id to the material for easier rendering later on.
				pMat->SetId(lIdCount);

				break;
			}

			lIdCount++;
		}

		return pMat;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------



	//-----------------------------------------------------------------------

}
