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
#include "graphics/Material_DiffuseAdditive2D.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_DiffuseAdditive2D::cMaterial_DiffuseAdditive2D(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
		cImageManager* apImageManager, cTextureManager *apTextureManager,
		cGpuProgramManager* apProgramManager,
		eMaterialPicture aPicture, cRenderer3D *apRenderer3D)
	: iOldMaterial(asName,eOldMaterialType_DiffuseAdditive,
				   apLowLevelGraphics,apImageManager,apTextureManager,apProgramManager,
				   aPicture,apRenderer3D)
	{
		mbIsTransperant = true;
		mbIsGlowing= true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMaterial_DiffuseAdditive2D::StartRendering()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_One);

		mpLowLevelGraphics->SetActiveTextureUnit(0);
		//mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorOp1,eTextureOp_Alpha);
		mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Modulate);

		mpLowLevelGraphics->SetTexture(0,GetTexture(eMaterialTexture_Diffuse));
	}

	//-----------------------------------------------------------------------

	void cMaterial_DiffuseAdditive2D::EndRendering()
	{
		mpLowLevelGraphics->SetTexture(0,NULL);
		mpLowLevelGraphics->SetActiveTextureUnit(0);
		mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorOp1,eTextureOp_Color);
		mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Modulate);
	}

	//-----------------------------------------------------------------------
}
