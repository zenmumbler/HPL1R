/*
 * 2023 by zenmumbler
 * This file is part of Rehatched
 */

#include "graphics/Material_Universal.h"
#include "scene/Light.h"
#include "scene/Camera.h"
#include "resources/GpuProgramManager.h"
#include "resources/TextureManager.h"
#include "graphics/GPUProgram.h"
#include "math/Math.h"
#include "system/String.h"
#include "graphics/Renderer3D.h"
#include "scene/PortalContainer.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// FRAGMENT PRORGAM SETUP
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	class cAmbProgramSetup : public iMaterialProgramSetup
	{
	public:
		void Setup(iGpuProgram *apProgram, cRenderSettings* apRenderSettings) {
			if(apRenderSettings->mpSector)
				apProgram->SetColor3f("ambientColor",apRenderSettings->mAmbientColor * apRenderSettings->mpSector->GetAmbientColor());
			else
				apProgram->SetColor3f("ambientColor",apRenderSettings->mAmbientColor);
		}
	};

	static cAmbProgramSetup gAmbProgramSetup;


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	Material_Universal::Material_Universal(const tString& asName, iLowLevelGraphics* apLowLevelGraphics,
		cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
		cRenderer3D *apRenderer3D)
		: iMaterial(asName,apLowLevelGraphics,apTextureManager,apProgramManager,apRenderer3D)
	{
		mbIsTransperant = false;
		mbUsesLights = false;

		///////////////////////////////////////////
		//Load the light pass program
		_program = mpProgramManager->CreateProgram("Universal.vert", "Universal.frag");
	}

	//-----------------------------------------------------------------------

	Material_Universal::~Material_Universal()
	{
		if (_program)
			mpProgramManager->Destroy(_program);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iGpuProgram* Material_Universal::GetProgramEx(eMaterialRenderType aType, int alPass, iLight3D *apLight) {
		if (aType == eMaterialRenderType_Z || aType == eMaterialRenderType_Diffuse) {
			return _program;
		}
		return NULL;
	}

	bool Material_Universal::VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return false;
	}

	//------------------------------------------------------------------------------------

	bool Material_Universal::VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return false;
	}

	//------------------------------------------------------------------------------------

	iMaterialProgramSetup * Material_Universal::GetFragmentProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if (aType == eMaterialRenderType_Z) {
			return &gAmbProgramSetup;
		}

		return NULL;
	}

	//------------------------------------------------------------------------------------

	eMaterialAlphaMode Material_Universal::GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialAlphaMode_Solid;
	}

	//------------------------------------------------------------------------------------

	eMaterialBlendMode Material_Universal::GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialBlendMode_Replace;
	}

	//------------------------------------------------------------------------------------

	eMaterialChannelMode Material_Universal::GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialChannelMode_RGBA;
	}

	//-----------------------------------------------------------------------

	iTexture* Material_Universal::GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if (alUnit == 0)
			return mvTexture[eMaterialTexture_Diffuse];
		return NULL;
	}

	//-----------------------------------------------------------------------

	eMaterialBlendMode Material_Universal::GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialBlendMode_None;
	}

	//-----------------------------------------------------------------------

	int Material_Universal::GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight)
	{
		return 1;
	}

	//-----------------------------------------------------------------------

	bool Material_Universal::UsesType(eMaterialRenderType renType)
	{
//		
//		if (renType != eMaterialRenderType_Light)
//		{
//			return false;
//		}
		return true;
	}

	//-----------------------------------------------------------------------

	tTextureTypeList Material_Universal::GetTextureTypes()
	{
		return {
			cTextureType("", eMaterialTexture_Diffuse)
		};
	}

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	iMaterial* MaterialType_Universal::Create(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
										cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
										cRenderer3D *apRenderer3D)
	{
		return new Material_Universal(asName, apLowLevelGraphics, apTextureManager, apProgramManager, apRenderer3D);
	}

	//-----------------------------------------------------------------------

}
