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
#include "graphics/Material_Diffuse.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_Diffuse::cMaterial_Diffuse(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
		cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
		cRenderer3D *apRenderer3D)
		: iMaterial_BaseLight(	"Diffuse_Light_vp.cg",
								"Diffuse_Light_fp.cg",
			asName,apLowLevelGraphics,apTextureManager,apProgramManager,apRenderer3D)
	{
		mbUseSpecular = false;
		mbUseNormalMap = false;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	iMaterial* cMaterialType_Diffuse::Create(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
										cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
										cRenderer3D *apRenderer3D)
	{
		return new cMaterial_Diffuse(asName,apLowLevelGraphics,apTextureManager,apProgramManager,apRenderer3D);
	}

	//-----------------------------------------------------------------------
}
