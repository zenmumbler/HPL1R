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
#ifndef HPL_MATERIAL_FALLBACK02_BASE_LIGHT_H
#define HPL_MATERIAL_FALLBACK02_BASE_LIGHT_H

#include <vector>
#include "graphics/Material.h"
#include "scene/Light3D.h"

#include "graphics/Material_BaseLight.h"

namespace hpl {

	//---------------------------------------------------------------

	class iMaterial_Fallback02_BaseLight : public iMaterial
	{
	public:
		iMaterial_Fallback02_BaseLight(
			const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
			cImageManager* apImageManager, cTextureManager *apTextureManager,
			cGpuProgramManager* apProgramManager,
			eMaterialPicture aPicture, cRenderer3D *apRenderer3D);

		virtual ~iMaterial_Fallback02_BaseLight();

		tTextureTypeList GetTextureTypes();

		bool UsesType(eMaterialRenderType aType);

		iGpuProgram* GetVertexProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		bool VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		bool VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		iGpuProgram* GetFragmentProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialBlendMode GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialChannelMode GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		iTexture* GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialBlendMode GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight);

		int GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight);

	protected:
		iTexture *mpNormalizationMap;
		iTexture *mpSpotNegativeRejectMap;

		iTexture *mpAttenuationMap;

		iGpuProgram* mvVtxPrograms[eBaseLightProgram_LastEnum];
		iGpuProgram* mvFragPrograms[eBaseLightProgram_LastEnum];
	};

	//---------------------------------------------------------------

	class cGLStateTwoUnits_Diffuse : public iGLStateProgram
	{
	public:
		cGLStateTwoUnits_Diffuse();

		void Bind();
		void UnBind();
	private:
		void InitData(){}
	};

	//---------------------------------------------------------------

	class cGLStateTwoUnits_ATIDiffuse : public iGLStateProgram
	{
	public:
		cGLStateTwoUnits_ATIDiffuse();
		~cGLStateTwoUnits_ATIDiffuse();

		void Bind();
		void UnBind();
	private:
		void InitData();

		int mlBind;
	};

	//---------------------------------------------------------------

	class cGLStateTwoUnits_Spot : public iGLStateProgram
	{
	public:
		cGLStateTwoUnits_Spot();

		void Bind();
		void UnBind();
	private:
		void InitData(){}
	};

	//---------------------------------------------------------------

	///////////////////////////////////////////
	// Diffuse
	///////////////////////////////////////////

	class cMaterial_Fallback02_Diffuse : public iMaterial_Fallback02_BaseLight
	{
	public:
		cMaterial_Fallback02_Diffuse(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
			cImageManager* apImageManager, cTextureManager *apTextureManager,
			cGpuProgramManager* apProgramManager,
			eMaterialPicture aPicture, cRenderer3D *apRenderer3D)
		: iMaterial_Fallback02_BaseLight(
				asName,apLowLevelGraphics,apImageManager,apTextureManager,apProgramManager,
				aPicture,apRenderer3D)
		{
		}
	};

	//---------------------------------------------------------------

};
#endif // HPL_MATERIAL_FALLBACK02_BASE_LIGHT_H
