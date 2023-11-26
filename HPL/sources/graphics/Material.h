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

	enum eMaterialTexture
	{
		eMaterialTexture_Diffuse,
		eMaterialTexture_Normal,
		eMaterialTexture_Specular,
		eMaterialTexture_Refraction,	// used once (more often in black plague)
		eMaterialTexture_None
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
		eMaterialAlphaMode_Trans
	};

	//! Determines what color channels are going to be affected
	enum eMaterialChannelMode
	{
		eMaterialChannelMode_RGBA,
		eMaterialChannelMode_Z
	};

	//---------------------------------------------------

	class cRenderSettings;
	class cTextureManager;
	class cGpuProgramManager;

	//---------------------------------------------------------------

	class iMaterialProgramSetup
	{
	public:
		virtual ~iMaterialProgramSetup() = default;
		virtual void Setup(iGpuProgram *apProgram,cRenderSettings* apRenderSettings)=0;
		virtual void SetupMatrix(cMatrixf *apModelMatrix, cRenderSettings* apRenderSettings){}
	};

	//---------------------------------------------------

	class iMaterial : public iResourceBase
	{
	public:
		iMaterial(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
				cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager);
		virtual ~iMaterial();

		//resources stuff.
		void Destroy(){}

		virtual void Update(float afTimeStep){}

		//The new render system stuff
		virtual iGpuProgram* GetProgramEx(){return NULL;}

		virtual iMaterialProgramSetup * GetProgramSetup(){return NULL;}

		virtual eMaterialAlphaMode GetAlphaMode(){return eMaterialAlphaMode_Solid;}
		virtual eMaterialBlendMode GetBlendMode(){return eMaterialBlendMode_Replace;}
		virtual eMaterialChannelMode GetChannelMode(){return eMaterialChannelMode_RGBA;}

		virtual iTexture* GetTexture(int alUnit){return NULL;}

		bool HasAlpha(){ return mbHasAlpha;}
		void SetHasAlpha(bool abX){ mbHasAlpha= abX; }

		virtual bool LoadData(TiXmlElement* apRootElem){ return true; }

		iTexture* GetTexture(eMaterialTexture aType) { return mvTexture[aType]; }
		void SetTexture(iTexture* apTex,eMaterialTexture aType){ mvTexture[aType] = apTex; }

		/**
		 * return true if the material is transparent
		 * \return
		 */
		virtual bool IsTransperant() {return mbIsTransperant;}

		const tString& GetPhysicsMaterial(){ return msPhysicsMaterial;}
		void SetPhysicsMaterial(const tString& asName){ msPhysicsMaterial = asName;}

	protected:
		iLowLevelGraphics* mpLowLevelGraphics;
		cTextureManager* mpTextureManager;
		cGpuProgramManager* mpProgramManager;

		bool mbIsTransperant;
		bool mbHasAlpha;

		tString msPhysicsMaterial;

		tTextureVec mvTexture;
	};

	typedef std::vector<iMaterial*> tMaterialVec;
	typedef tMaterialVec::iterator tMaterialVecIt;

	class iMaterialType
	{
	public:
		virtual ~iMaterialType() {}
		virtual bool IsCorrect(tString asName)=0;
		virtual iMaterial* Create(const tString& asName, iLowLevelGraphics* apLowLevelGraphics,
			cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager)=0;
	};

};
#endif // HPL_MATERIAL_H
