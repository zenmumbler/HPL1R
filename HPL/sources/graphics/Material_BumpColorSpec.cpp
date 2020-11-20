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
#include "graphics/Material_BumpColorSpec.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_BumpColorSpec::cMaterial_BumpColorSpec(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
		cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
		cRenderer3D *apRenderer3D)
		: iMaterial_BaseLight(	"DiffuseSpec_Light_vp.cg",
								"BumpColorSpec_Light_fp.cg",
			asName,apLowLevelGraphics,apTextureManager,apProgramManager,apRenderer3D)
	{
		mbUseSpecular = true;
		mbUseNormalMap = true;
		mbUseColorSpecular = true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iMaterial* cMaterialType_BumpColorSpec::Create(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
		cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
		cRenderer3D *apRenderer3D)
	{
		return hplNew( cMaterial_BumpColorSpec, (asName,apLowLevelGraphics,apTextureManager,apProgramManager,apRenderer3D) );
	}

	//-----------------------------------------------------------------------
}
