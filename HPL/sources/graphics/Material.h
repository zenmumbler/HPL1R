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
#ifndef HPL_MATERIAL_H
#define HPL_MATERIAL_H

#include <vector>
#include "graphics/Texture.h"
#include "system/SystemTypes.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/GPUProgram.h"

class TiXmlElement;

namespace hpl {


#define kMaxProgramNum (5)

	enum eMaterialQuality
	{
		eMaterialQuality_Classic=3,
		eMaterialQuality_Modern=4,
		eMaterialQuality_LastEnum=5
	};

	enum eMaterialTexture
	{
		eMaterialTexture_Diffuse,
		eMaterialTexture_NMap,
		eMaterialTexture_Specular,
		eMaterialTexture_Alpha,
		eMaterialTexture_Illumination,
		eMaterialTexture_CubeMap,
		eMaterialTexture_Refraction,
		eMaterialTexture_LastEnum
	};

	enum eMaterialRenderType
	{
		eMaterialRenderType_Z,
		eMaterialRenderType_Light,
		eMaterialRenderType_Diffuse,
		eMaterialRenderType_LastEnum
	};

	enum eMaterialBlendMode
	{
		eMaterialBlendMode_None,
		eMaterialBlendMode_Add,
		eMaterialBlendMode_Mul,
		eMaterialBlendMode_MulX2,
		eMaterialBlendMode_Replace,
		eMaterialBlendMode_Alpha,
		eMaterialBlendMode_DestAlphaAdd,
		eMaterialBlendMode_LastEnum
	};

	enum eMaterialAlphaMode
	{
		eMaterialAlphaMode_Solid,
		eMaterialAlphaMode_Trans,
		eMaterialAlphaMode_LastEnum,
	};

	//! Determines what color channels are going to be affected
	enum eMaterialChannelMode
	{
		eMaterialChannelMode_RGBA,
		eMaterialChannelMode_RGB,
		eMaterialChannelMode_A,
		eMaterialChannelMode_Z,
		eMaterialChannelMode_LastEnum,
	};

	//---------------------------------------------------

	class cRenderer3D;
	class cRenderSettings;
	class cTextureManager;
	class cGpuProgramManager;
	class iLight;
	class iCamera;
	class iLight3D;

	//---------------------------------------------------

	class iGLStateProgram : public iGpuProgram
	{
	public:
		iGLStateProgram(tString asName)
			: iGpuProgram(asName,eGpuProgramType_Fragment)
		{
			mbSetUpDone = false;
		}
		virtual ~iGLStateProgram(){}

		void SetUp(iLowLevelGraphics *apLowLevelGraphics)
		{
			if(mbSetUpDone==false)
			{
				mpLowGfx = apLowLevelGraphics;
				mbSetUpDone = true;
				InitData();
			}
		}

		virtual void Bind()=0;
		virtual void UnBind()=0;

		bool CreateFromFile(const tString& asFile, const tString& asEntry){return false;}
		bool SetFloat(const tString& asName, float afX){return false;}

		bool SetVec2f(const tString& asName, float afX,float afY){return false;}
		bool SetVec3f(const tString& asName, float afX,float afY,float afZ){return false;}
		bool SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW){return false;}
		bool SetMatrixf(const tString& asName, const cMatrixf& mMtx){return false;}
		bool SetMatrixIdentityf(const tString& asName, eGpuProgramMatrix mType){return false;}
		bool SetTexture(const tString& asName,iTexture* apTexture, bool abAutoDisable=true){return false;}
		bool SetTextureToUnit(int alUnit, iTexture* apTexture){return false;}

		eGpuProgramType GetType() { return mProgramType;}

		void Destroy(){}
	protected:

		iLowLevelGraphics *mpLowGfx;

		bool mbSetUpDone;

		virtual void InitData()=0;
	};

	//---------------------------------------------------------------


	class iMaterialProgramSetup
	{
	public:
		virtual ~iMaterialProgramSetup() = default;
		virtual void Setup(iGpuProgram *apProgram,cRenderSettings* apRenderSettings)=0;
		virtual void SetupMatrix(cMatrixf *apModelMatrix, cRenderSettings* apRenderSettings){}
	};

	//---------------------------------------------------

	class cTextureType
	{
	public:
		cTextureType(tString asSuffix, eMaterialTexture aType): msSuffix(asSuffix), mType(aType){}

		tString msSuffix;
		eMaterialTexture mType;
	};

	typedef std::list<cTextureType> tTextureTypeList;
	typedef tTextureTypeList::iterator tTextureTypeListIt;

	//---------------------------------------------------

	class iMaterial : public iResourceBase
	{
	public:
		iMaterial(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
				cTextureManager *apTextureManager,
				cGpuProgramManager* apProgramManager, cRenderer3D *apRenderer3D);
		virtual ~iMaterial();

		//resources stuff.
		void Destroy(){}

		virtual void Update(float afTimeStep){}

		//The new render system stuff
		virtual iGpuProgram* GetVertexProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight){return NULL;}
		virtual bool VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight){return false;}
		virtual bool VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight){return false;}
		virtual iMaterialProgramSetup * GetVertexProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight){return NULL;}

		virtual iGpuProgram* GetFragmentProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight){return NULL;}
		virtual iMaterialProgramSetup * GetFragmentProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight){return NULL;}

		virtual eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight){return eMaterialAlphaMode_Solid;}
		virtual eMaterialBlendMode GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight){return eMaterialBlendMode_Replace;}
		virtual eMaterialChannelMode GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight){return eMaterialChannelMode_RGBA;}

		virtual iTexture* GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight){ return NULL;}
		virtual eMaterialBlendMode GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight){ return eMaterialBlendMode_None;}

		virtual int GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight){ return 0;}

		virtual bool UsesType(eMaterialRenderType aType){return false;}

		bool HasAlpha(){ return mbHasAlpha;}
		void SetHasAlpha(bool abX){ mbHasAlpha= abX; }

		bool GetDepthTest(){ return mbDepthTest;}
		void SetDepthTest(bool abX){ mbDepthTest = abX;}

		float GetValue() { return mfValue;}
		void SetValue(float afX) { mfValue = afX;}

		virtual bool LoadData(TiXmlElement* apRootElem){ return true; }

		iTexture* GetTexture(eMaterialTexture aType);
		cRect2f GetTextureOffset(eMaterialTexture aType);

		void SetTexture(iTexture* apTex,eMaterialTexture aType){ mvTexture[aType] = apTex; }

		void SetProgram(iGpuProgram* apProgram, eGpuProgramType aType, unsigned int alNum){
						mpProgram[aType][alNum] = apProgram;}
		iGpuProgram* GetProgram(eGpuProgramType aType,unsigned int alNum){
						return mpProgram[aType][alNum];}

		/**
		 * return true if the material is transparent
		 * \return
		 */
		virtual bool IsTransperant() {return mbIsTransperant;}

		/**
		 * return true if the material has a light pass
		 * \return
		 */
		virtual bool UsesLights(){ return mbUsesLights; }

		virtual tTextureTypeList GetTextureTypes(){
			tTextureTypeList vTypes;
			vTypes.push_back(cTextureType("",eMaterialTexture_Diffuse));
			return vTypes;
		}

		/**
		 * The type of the material
		 * \param alId
		 */
		void SetId(int alId){ mlId = alId;}
		int GetId(){ return mlId;}

		virtual iGpuProgram* GetRefractionVertexProgam(){ return NULL;}
		virtual iGpuProgram* GetRefractionFragmentProgam(){ return NULL;}

		virtual bool GetRefractionUsesDiffuse(){ return false;}
		virtual eMaterialTexture GetRefractionDiffuseTexture(){ return eMaterialTexture_Diffuse;}

		virtual bool GetRefractionUsesEye(){ return false;}
		virtual bool GetRefractionUsesTime(){ return false;}
		virtual bool GetRefractionSkipsStandardTrans(){ return false;}

		const tString& GetPhysicsMaterial(){ return msPhysicsMaterial;}
		void SetPhysicsMaterial(const tString& asName){ msPhysicsMaterial = asName;}

		static void SetQuality(eMaterialQuality aQuality){ mQuality = aQuality;}
		static eMaterialQuality GetQuality(){ return mQuality;}

	protected:
		iLowLevelGraphics* mpLowLevelGraphics;
		cTextureManager* mpTextureManager;
		cRenderer3D* mpRenderer3D;
		cRenderSettings *mpRenderSettings;
		cGpuProgramManager* mpProgramManager;

		static eMaterialQuality	mQuality;

		bool mbIsTransperant;
		bool mbUsesLights;
		bool mbHasAlpha;
		bool mbDepthTest;
		float mfValue;

		int mlId;

		tString msPhysicsMaterial;

		tTextureVec mvTexture;

		iGpuProgram *mpProgram[2][kMaxProgramNum];

		int mlPassCount;

		//void Destroy();
	};

	typedef std::vector<iMaterial*> tMaterialVec;
	typedef tMaterialVec::iterator tMaterialVecIt;

	class iMaterialType
	{
	public:
		virtual ~iMaterialType() {}
		virtual bool IsCorrect(tString asName)=0;
		virtual iMaterial* Create(const tString& asName, iLowLevelGraphics* apLowLevelGraphics,
			cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
			cRenderer3D *apRenderer3D)=0;
	};

	typedef std::list<iMaterialType*> tMaterialTypeList;
	typedef tMaterialTypeList::iterator tMaterialTypeListIt;

};
#endif // HPL_MATERIAL_H
