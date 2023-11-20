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
#include "graphics/Material.h"
#include "graphics/Renderer3D.h"
#include "resources/TextureManager.h"
#include "resources/GpuProgramManager.h"
#include "graphics/GPUProgram.h"
#include "scene/Camera.h"
#include "system/String.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iMaterial::iMaterial(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
		cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager)
		: iResourceBase(asName, 0)
	{
		mvTexture.resize(eMaterialTexture_LastEnum, NULL);

		mpLowLevelGraphics = apLowLevelGraphics;
		mpTextureManager = apTextureManager;
		mpProgramManager = apProgramManager;

		mbIsTransperant = false;
		mbHasAlpha = false;
		mbDepthTest = true;

		mlId = -1;
	}

	iMaterial::~iMaterial()
	{
		int i;

		for(i=0;i<(int)mvTexture.size();i++){
			if(mvTexture[i])
				mpTextureManager->Destroy(mvTexture[i]);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iTexture* iMaterial::GetTexture(eMaterialTexture aType)
	{
		return mvTexture[aType];
	}

	//-----------------------------------------------------------------------

}
