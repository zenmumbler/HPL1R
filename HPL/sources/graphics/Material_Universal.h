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
						   cGpuProgramManager* apProgramManager);

		virtual ~Material_Universal();

		tTextureTypeList GetTextureTypes() override;

		bool UsesType(eMaterialRenderType renType) override;

		iGpuProgram* GetProgramEx(eMaterialRenderType aType) override;

		iMaterialProgramSetup * GetProgramSetup(eMaterialRenderType aType) override;

		eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType) override;
		eMaterialBlendMode GetBlendMode(eMaterialRenderType aType) override;
		eMaterialChannelMode GetChannelMode(eMaterialRenderType aType) override;

		iTexture* GetTexture(int alUnit,eMaterialRenderType aType) override;

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
			cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager);

	private:
	};


};

#endif // HPL_MATERIAL_UNIVERSAL_H
