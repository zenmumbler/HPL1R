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

#include "resources/MaterialManager.h"
#include "system/String.h"
#include "system/Log.h"
#include "graphics/Graphics.h"
#include "resources/TextureManager.h"
#include "resources/Resources.h"
#include "graphics/Material.h"
#include "graphics/MaterialHandler.h"
#include "tinyXML/tinyxml.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterialManager::cMaterialManager(cGraphics* apGraphics,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher())
	{
		mpGraphics = apGraphics;
		mpResources = apResources;
		mfTextureAnisotropy = 8.0f;
	}

	cMaterialManager::~cMaterialManager()
	{
		DestroyAll();

		Log(" Done with materials\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iMaterial* cMaterialManager::CreateMaterial(const tString& asName)
	{
		tString sPath;
		iMaterial* pMaterial;
		tString asNewName;

		BeginLoad(asName);

		asNewName = cString::SetFileExt(asName,"mat");

		pMaterial = static_cast<iMaterial*>(this->FindLoadedResource(asNewName,sPath));

		if(pMaterial==NULL && sPath!="")
		{
			pMaterial = LoadFromFile(asNewName,sPath);

			if(pMaterial==NULL){
				Error("Couldn't load material '%s'\n",asNewName.c_str());
				EndLoad();
				return NULL;
			}

			AddResource(pMaterial);
		}

		if(pMaterial)pMaterial->IncUserCount();
		else Error("Couldn't create material '%s'\n",asNewName.c_str());

		EndLoad();
		return pMaterial;
	}

	//-----------------------------------------------------------------------

	iResourceBase* cMaterialManager::Create(const tString& asName)
	{
		return CreateMaterial(asName);
	}

	//-----------------------------------------------------------------------

	void cMaterialManager::Update(float afTimeStep)
	{
		tResourceHandleMapIt it = m_mapHandleResources.begin();
		for(; it != m_mapHandleResources.end(); ++it)
		{
			iResourceBase *pBase = it->second;
			iMaterial *pMat = static_cast<iMaterial*>(pBase);

			pMat->Update(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	void cMaterialManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if(apResource->HasUsers()==false){
			RemoveResource(apResource);
			delete apResource;
		}
	}

	//-----------------------------------------------------------------------

	void cMaterialManager::SetTextureAnisotropy(float afX)
	{
		if(mfTextureAnisotropy == afX) return;
		mfTextureAnisotropy = afX;

		tResourceHandleMapIt it = m_mapHandleResources.begin();
		for(; it != m_mapHandleResources.end(); ++it)
		{
			iMaterial *pMat = static_cast<iMaterial*>(it->second);

			for(int i=0; i<eMaterialTexture_None; ++i)
			{
				iTexture *pTex = pMat->GetTexture((eMaterialTexture)i);
				if(pTex)pTex->SetAnisotropyDegree(mfTextureAnisotropy);
			}
		}
	}

	//-----------------------------------------------------------------------

	tString cMaterialManager::GetPhysicsMaterialName(const tString& asName)
	{
		tString sPath;
		iMaterial* pMaterial;
		tString asNewName;

		asNewName = cString::SetFileExt(asName,"mat");

		pMaterial = static_cast<iMaterial*>(this->FindLoadedResource(asNewName,sPath));

		if(pMaterial==NULL && sPath!="")
		{
			TiXmlDocument *pDoc = new TiXmlDocument(sPath.c_str());
			if(!pDoc->LoadFile()){
				return "";
			}

			TiXmlElement *pRoot = pDoc->RootElement();

			TiXmlElement *pMain = pRoot->FirstChildElement("Main");
			if(pMain==NULL){
				delete pDoc;
				Error("Main child not found in '%s'\n",sPath.c_str());
				return "";
			}

			tString sPhysicsName = cString::ToString(pMain->Attribute("PhysicsMaterial"),"Default");

			delete pDoc;

			return sPhysicsName;
		}

		if(pMaterial)
			return pMaterial->GetPhysicsMaterial();
		else
			return "";
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	iMaterial* cMaterialManager::LoadFromFile(const tString& asName,const tString& asPath)
	{
		TiXmlDocument *pDoc = new TiXmlDocument(asPath.c_str());
		if(!pDoc->LoadFile()){
			delete pDoc;
			return NULL;
		}

		TiXmlElement *pRoot = pDoc->RootElement();

		///////////////////////////
		//Main
		TiXmlElement *pMain = pRoot->FirstChildElement("Main");
		if(pMain==NULL){
			delete pDoc;
			Error("Main child not found.\n");
			return NULL;
		}

		const char* sType = pMain->Attribute("Type");
		if(sType ==NULL){
			delete pDoc;
			Error("Type not found.\n");
			return NULL;
		}

		bool bUseAlpha = cString::ToBool(pMain->Attribute("UseAlpha"), false);
		bool bDepthTest = cString::ToBool(pMain->Attribute("DepthTest"), true);
		tString sPhysicsMatName = cString::ToString(pMain->Attribute("PhysicsMaterial"),"Default");

		iMaterial* pMat = mpGraphics->GetMaterialHandler()->Create(asName,sType);
		if(pMat==NULL){
			Error("Invalid material type '%s'\n",sType);
			delete pDoc;
			return NULL;
		}

		pMat->SetHasAlpha(bUseAlpha);
		pMat->SetPhysicsMaterial(sPhysicsMatName);

		///////////////////////////
		//Textures
		TiXmlElement *pTexRoot = pRoot->FirstChildElement("TextureUnits");
		if(pTexRoot==NULL){
			delete pDoc;
			Error("TextureUnits child not found.\n");
			return NULL;
		}

		tTextureTypeList lstTexTypes = pMat->GetTextureTypes();
		tTextureTypeListIt it = lstTexTypes.begin();
		for(;it != lstTexTypes.end();it++)
		{
			iTexture *pTex;

			TiXmlElement *pTexChild = pTexRoot->FirstChildElement(GetTextureString(it->mType).c_str());
			if(pTexChild==NULL){
				/*Error("Texture unit missing!");
				delete pMat;
				return NULL;*/
				continue;
			}

			eTextureTarget target = eTextureTarget_2D;
			tString sFile = cString::ToString(pTexChild->Attribute("File"),"");
			eTextureWrap wrap = GetWrap(cString::ToString(pTexChild->Attribute("Wrap"),""));

			eTextureAnimMode animMode = GetAnimMode(cString::ToString(pTexChild->Attribute("AnimMode"),"None"));
			float fFrameTime = cString::ToFloat(pTexChild->Attribute("AnimFrameTime"),1.0f);

			if(sFile=="")
			{
				continue;
			}

			if(animMode != eTextureAnimMode_None)
			{
				pTex = mpResources->GetTextureManager()->CreateAnim2D(sFile);
			}
			else
			{
				pTex = mpResources->GetTextureManager()->Create2D(sFile);
			}

			if(pTex==NULL){
				Error("Couldn't load texture!!\n");
				delete pDoc;
				delete pMat;
				return NULL;
			}

			pTex->SetFrameTime(fFrameTime);
			pTex->SetAnimMode(animMode);

			pTex->SetWrapAll(wrap);
			pTex->SetFilter(eTextureFilter_Trilinear);
			pTex->SetAnisotropyDegree(mfTextureAnisotropy);

			pMat->SetTexture(pTex, it->mType);
		}

		///////////////////////////
		//Custom
		pMat->LoadData(pRoot);

		delete pDoc;

		return pMat;
	}

	//-----------------------------------------------------------------------

	tString cMaterialManager::GetTextureString(eMaterialTexture aType)
	{
		switch(aType)
		{
			case eMaterialTexture_Diffuse: return "Diffuse";
			case eMaterialTexture_Normal: return "NMap";
			case eMaterialTexture_Specular: return "Specular";
			case eMaterialTexture_Refraction: return "Refraction";
			default: break;
		}

		return "";
	}

	eMaterialTexture cMaterialManager::GetTextureType(const tString& type)
	{
		auto typeLower = cString::ToLowerCase(type);
		if(typeLower == "diffuse") return eMaterialTexture_Diffuse;
		else if(typeLower == "nmap") return eMaterialTexture_Normal;
		else if(typeLower == "specular") return eMaterialTexture_Specular;
		else if(typeLower == "refraction") return eMaterialTexture_Refraction;

		Warning("Skipping unsupported texture type `%s`\n", type.c_str());
		return eMaterialTexture_None;
	}

	//-----------------------------------------------------------------------

	eTextureWrap cMaterialManager::GetWrap(const tString& asType)
	{
		auto typeLower = cString::ToLowerCase(asType);
		if(typeLower == "repeat") return eTextureWrap_Repeat;
		else if(typeLower == "clamptoedge") return eTextureWrap_ClampToEdge;

		Warning("Ignoring unsupported texture wrap mode `%s`\n", asType.c_str());
		return eTextureWrap_Repeat;
	}

	eTextureAnimMode cMaterialManager::GetAnimMode(const tString& asType)
	{
		auto typeLower = cString::ToLowerCase(asType);
		if(typeLower == "none") return eTextureAnimMode_None;
		else if(typeLower == "loop") return eTextureAnimMode_Loop;
		else if(typeLower == "oscillate") return eTextureAnimMode_Oscillate;

		Warning("Ignoring unsupported animation mode `%s`\n", asType.c_str());
		return eTextureAnimMode_None;
	}

	//-----------------------------------------------------------------------
}
