/*
 * 2023 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_MATERIAL_UNIVERSAL_H
#define HPL_MATERIAL_UNIVERSAL_H

#include "graphics/Material.h"
#include "scene/Light3D.h"

namespace hpl {

	class Material_Universal : public iMaterial
	{
	public:
		Material_Universal(const tString& asName,
						   iLowLevelGraphics* apLowLevelGraphics,
						   cTextureManager *apTextureManager,
						   cGpuProgramManager* apProgramManager,
						   cRenderer3D *apRenderer3D);

		virtual ~Material_Universal();

		tTextureTypeList GetTextureTypes() override;

		bool UsesType(eMaterialRenderType renType) override;

		iGpuProgram* GetProgramEx(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		bool VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		bool VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		iMaterialProgramSetup * GetFragmentProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		eMaterialBlendMode GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		eMaterialChannelMode GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		iTexture* GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		eMaterialBlendMode GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		int GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight) override;

	protected:
		iGpuProgram* _program;
	};

	class MaterialType_Universal : public iMaterialType
	{
	public:
		bool IsCorrect(tString asName){
			return true;
		}

		iMaterial* Create(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
			cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
			cRenderer3D *apRenderer3D);

	private:
	};


};

#endif // HPL_MATERIAL_UNIVERSAL_H
