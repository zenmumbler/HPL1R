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
#ifndef HPL_MATERIAL_WATER_H
#define HPL_MATERIAL_WATER_H

#include "graphics/Material.h"

namespace hpl {

	class cMaterial_Water : public iMaterial
	{
	public:
		cMaterial_Water(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
			cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
			cRenderer3D *apRenderer3D);

		virtual ~cMaterial_Water();

		tTextureTypeList GetTextureTypes();

		bool UsesType(eMaterialRenderType aType);

		void Update(float afTimeStep);

		iGpuProgram* GetVertexProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		iMaterialProgramSetup* GetVertexProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		bool VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		bool VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		iGpuProgram* GetFragmentProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialBlendMode GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialChannelMode GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		iTexture* GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialBlendMode GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight);

		int GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight){ return 1;}

		iGpuProgram* GetRefractionVertexProgam(){ return mpRefractVtxProg;}
		iGpuProgram* GetRefractionFragmentProgam(){ return mpRefractFragProg;}
		bool GetRefractionUsesDiffuse(){ return true;}
		eMaterialTexture GetRefractionDiffuseTexture(){ return eMaterialTexture_Specular;}
		bool GetRefractionUsesEye(){ return true;}
		bool GetRefractionSkipsStandardTrans(){ return true;}
		bool GetRefractionUsesTime(){ return true;}

	private:
		iGpuProgram *mpFogVtxProg;

		iGpuProgram *mpRefractVtxProg;
		iGpuProgram *mpRefractFragProg;

		float mfTime;
	};

	class cMaterialType_Water : public iMaterialType
	{
	public:
		bool IsCorrect(tString asName){
			return cString::ToLowerCase(asName)=="water";
		}

		iMaterial* Create(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
			cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
			cRenderer3D *apRenderer3D)
		{
			return new cMaterial_Water(asName,apLowLevelGraphics, apTextureManager,apProgramManager,apRenderer3D);
		}
	};

};

#endif // HPL_MATERIAL_WATER_H
