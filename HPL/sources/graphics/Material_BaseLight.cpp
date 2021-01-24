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
#include "graphics/Material_BaseLight.h"
#include "scene/Light.h"
#include "scene/Camera.h"
#include "resources/GpuProgramManager.h"
#include "resources/TextureManager.h"
#include "graphics/GPUProgram.h"
#include "math/Math.h"
#include "system/String.h"
#include "scene/Light3DSpot.h"
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
		void Setup(iGpuProgram *apProgram,cRenderSettings* apRenderSettings)
		{
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

	iMaterial_BaseLight::iMaterial_BaseLight(const tString& asLightVertexProgram,
										const tString& asLightFragmentProgram,
		const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
		cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
		cRenderer3D *apRenderer3D)
		: iMaterial(asName,apLowLevelGraphics,apTextureManager,apProgramManager,apRenderer3D)
	{
		mbIsTransperant = false;
		mbUsesLights = true;
		mbUseColorSpecular = false;

		for(int i=0; i<eBaseLightProgram_LastEnum; i++) {
			mvVtxPrograms[i] =NULL;
			mvFragPrograms[i] =NULL;
		}

		////////////////////////////////////////
		//Get names for other light programs
		tString sSpotVtxProgram = cString::Sub(asLightVertexProgram,0, (int)asLightVertexProgram.size() - 5) +
									"Spot_vp.cg";
		tString sSpotFragProgram = cString::Sub(asLightFragmentProgram,0, (int)asLightFragmentProgram.size() - 5) +
									"Spot_fp.cg";

		///////////////////////////////////////////
		//Load the light pass vertex program
		//Point
		mvVtxPrograms[eBaseLightProgram_Point] = mpProgramManager->CreateProgram(asLightVertexProgram,"main",
			eGpuProgramType_Vertex);
		//Spot
		mvVtxPrograms[eBaseLightProgram_Spot] = mpProgramManager->CreateProgram(sSpotVtxProgram,"main",
			eGpuProgramType_Vertex);

		///////////////////////////////////////////
		//Load the light pass fragment program
		//Point
		mvFragPrograms[eBaseLightProgram_Point] = mpProgramManager->CreateProgram(asLightFragmentProgram,"main",
			eGpuProgramType_Fragment);
		//Spot
		mvFragPrograms[eBaseLightProgram_Spot] = mpProgramManager->CreateProgram(sSpotFragProgram,"main",
															eGpuProgramType_Fragment);


		///////////////////////////////////////////
		//Load the Z pass vertex program
		iGpuProgram *pVtxProg = mpProgramManager->CreateProgram("Diffuse_Color_vp.cg","main",eGpuProgramType_Vertex);
		SetProgram(pVtxProg,eGpuProgramType_Vertex,1);


		///////////////////////////////////////////
		//More fragment programs
		mpSimpleFP = mpProgramManager->CreateProgram("Diffuse_Color_fp.cg","main",eGpuProgramType_Fragment);
		mpAmbientFP = mpProgramManager->CreateProgram("Ambient_Color_fp.cg","main",eGpuProgramType_Fragment);

		///////////////////////////////////////////
		//Normalization map
		mpNormalizationMap = mpTextureManager->CreateCubeMap("Normalization", false);
		mpNormalizationMap->SetWrapS(eTextureWrap_ClampToEdge);
		mpNormalizationMap->SetWrapT(eTextureWrap_ClampToEdge);

		///////////////////////////////////////////
		//Negative rejection
		mpSpotNegativeRejectMap = mpTextureManager->Create1D("core_spot_negative_reject",false);
		if(mpSpotNegativeRejectMap)
		{
			mpSpotNegativeRejectMap->SetWrapS(eTextureWrap_ClampToEdge);
			mpSpotNegativeRejectMap->SetWrapT(eTextureWrap_ClampToEdge);
		}


		mbUseSpecular = false;
		mbUseNormalMap = false;
	}

	//-----------------------------------------------------------------------

	iMaterial_BaseLight::~iMaterial_BaseLight()
	{
		if(mpNormalizationMap) mpTextureManager->Destroy(mpNormalizationMap);
		if(mpSpotNegativeRejectMap) mpTextureManager->Destroy(mpSpotNegativeRejectMap);

		for(int i=0; i<eBaseLightProgram_LastEnum; i++)
		{
			if(mvVtxPrograms[i])	mpProgramManager->Destroy(mvVtxPrograms[i]);
			if(mvFragPrograms[i])	mpProgramManager->Destroy(mvFragPrograms[i]);
		}

		if(mpSimpleFP) mpProgramManager->Destroy(mpSimpleFP);
		if(mpAmbientFP) mpProgramManager->Destroy(mpAmbientFP);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iGpuProgram* iMaterial_BaseLight::GetVertexProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(apLight && aType == eMaterialRenderType_Light)
		{
			eBaseLightProgram program;

			if(apLight->GetLightType()==eLight3DType_Point)
				program = eBaseLightProgram_Point;
			else
				program = eBaseLightProgram_Spot;

			return mvVtxPrograms[program];
		}

		if(aType == eMaterialRenderType_Z) return mpProgram[eGpuProgramType_Vertex][1];
		if(aType == eMaterialRenderType_Diffuse) return mpProgram[eGpuProgramType_Vertex][1];

		return NULL;
	}

	//------------------------------------------------------------------------------------

	bool iMaterial_BaseLight::VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Light) return true;
		return false;
	}

	//------------------------------------------------------------------------------------

	bool iMaterial_BaseLight::VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Light && mbUseSpecular) return true;
		return false;
	}

	//------------------------------------------------------------------------------------

	iGpuProgram* iMaterial_BaseLight::GetFragmentProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Light)
		{
			eBaseLightProgram program;

			if(apLight->GetLightType()==eLight3DType_Point)
				program = eBaseLightProgram_Point;
			else
				program = eBaseLightProgram_Spot;

			return mvFragPrograms[program];
		}
		else if(aType == eMaterialRenderType_Diffuse)
		{
			return mpSimpleFP;
		}
		else if(aType == eMaterialRenderType_Z)
		{
			return mpAmbientFP;
		}
		return NULL;
	}


	iMaterialProgramSetup * iMaterial_BaseLight::GetFragmentProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Z)
		{
			return &gAmbProgramSetup;
		}

		return NULL;
	}

	//------------------------------------------------------------------------------------

	eMaterialAlphaMode iMaterial_BaseLight::GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Z && mbHasAlpha) return eMaterialAlphaMode_Trans;

		return eMaterialAlphaMode_Solid;
	}

	//------------------------------------------------------------------------------------

	eMaterialBlendMode iMaterial_BaseLight::GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Z) return eMaterialBlendMode_Replace;

		return eMaterialBlendMode_Add;
	}

	//------------------------------------------------------------------------------------

	eMaterialChannelMode iMaterial_BaseLight::GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Z)
		{
			//return eMaterialChannelMode_Z;
			return eMaterialChannelMode_RGBA;
		}

		return eMaterialChannelMode_RGBA;
	}

	//-----------------------------------------------------------------------

	iTexture* iMaterial_BaseLight::GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Z)
		{
			//if(alUnit==0 && mbHasAlpha) return mvTexture[eMaterialTexture_Diffuse];
			if(alUnit==0) return mvTexture[eMaterialTexture_Diffuse];
			return NULL;
		}
		else if(aType == eMaterialRenderType_Diffuse)
		{
			if(alUnit==0) return mvTexture[eMaterialTexture_Illumination];
			return NULL;
		}
		else if(aType == eMaterialRenderType_Light)
		{
			switch(alUnit)
			{
			//Diffuse texture
			case 0: return mvTexture[eMaterialTexture_Diffuse];

			//Normalmap
			case 1: if(mbUseNormalMap) return mvTexture[eMaterialTexture_NMap];break;

			//Normalization map
			case 2: return mpNormalizationMap; break;

			//Falloff map
			case 3: return apLight->GetFalloffMap();

			//Spotlight texture
			case 4: if(apLight->GetLightType() == eLight3DType_Spot)
					{
						cLight3DSpot *pSpotLight = static_cast<cLight3DSpot*>(apLight);
						return pSpotLight->GetTexture();
					};
				break;
			//Negative rejection
			case 5: if(apLight->GetLightType() == eLight3DType_Spot)
					{
						return mpSpotNegativeRejectMap;
					}
					break;
			//Color specular
			case 6:	if(mbUseColorSpecular)
					{
						return mvTexture[eMaterialTexture_Specular];
					}
					break;
			}

		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	eMaterialBlendMode iMaterial_BaseLight::GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialBlendMode_None;
	}

	//-----------------------------------------------------------------------

	int iMaterial_BaseLight::GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight)
	{
		return 1;
	}

	//-----------------------------------------------------------------------

	bool iMaterial_BaseLight::UsesType(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Diffuse)
		{
			if(mvTexture[eMaterialTexture_Illumination])
				return true;
			else
				return false;
		}
		return true;
	}

	//-----------------------------------------------------------------------

	tTextureTypeList iMaterial_BaseLight::GetTextureTypes()
	{
		tTextureTypeList vTypes;
		vTypes.push_back(cTextureType("",eMaterialTexture_Diffuse));
		if(mbUseNormalMap)
			vTypes.push_back(cTextureType("_bump",eMaterialTexture_NMap));

		if(mbUseColorSpecular)
			vTypes.push_back(cTextureType("_spec",eMaterialTexture_Specular));

		vTypes.push_back(cTextureType("_illum",eMaterialTexture_Illumination));

		return vTypes;
	}

	//-----------------------------------------------------------------------
}
