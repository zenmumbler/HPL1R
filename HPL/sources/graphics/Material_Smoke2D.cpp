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

#include "graphics/Material_Smoke2D.h"
#include "graphics/LowLevelGraphics.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_Smoke2D::cMaterial_Smoke2D(cImageManager* apImageManager)
		: iOldMaterial(eOldMaterialType_Smoke, apImageManager)
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMaterial_Smoke2D::StartRendering(iLowLevelGraphics* apLowLevelGraphics) const
	{
		apLowLevelGraphics->SetBlendActive(true);
		apLowLevelGraphics->SetBlendFunc(eBlendFunc_Zero,eBlendFunc_OneMinusSrcColor);

		apLowLevelGraphics->SetTexture(0, GetTexture());
		apLowLevelGraphics->SetActiveTextureUnit(0);
		//apLowLevelGraphics->SetTextureEnv(eTextureParam_ColorOp1,eTextureOp_Alpha);
		//apLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Modulate);
		apLowLevelGraphics->SetTextureEnv(eTextureParam_ColorOp1,eTextureOp_Color);
		apLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Modulate);
	}

	//-----------------------------------------------------------------------

	void cMaterial_Smoke2D::EndRendering(iLowLevelGraphics* apLowLevelGraphics) const
	{
		apLowLevelGraphics->SetActiveTextureUnit(0);
		apLowLevelGraphics->SetTextureEnv(eTextureParam_ColorOp1,eTextureOp_Color);
		apLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Modulate);
	}

	//-----------------------------------------------------------------------
}
