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

	cMaterialManager::cMaterialManager(cGraphics* apGraphics, cTextureManager* textureManager, cGpuProgramManager* programManager)
		: iResourceManager{"material"}
		, _textureManager{textureManager}
		, _programManager{programManager}
	{
		mpGraphics = apGraphics;
		mfTextureAnisotropy = 8.0f;
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

		if (pMaterial==NULL && sPath!="")
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

	void cMaterialManager::Update(float afTimeStep)
	{
		for(auto [_, resource] : _handleToResourceMap)
		{
			iMaterial *material = static_cast<iMaterial*>(resource);
			material->Update(afTimeStep);
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

		for (auto [_, resource] : _handleToResourceMap)
		{
			iMaterial *pMat = static_cast<iMaterial*>(resource);

			for (int i=0; i < eMaterialTexture_None; ++i)
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

	static eMaterialTexture GetTextureType(const tString& type)
	{
		auto typeLower = cString::ToLowerCase(type);
		if (typeLower == "diffuse") return eMaterialTexture_Diffuse;
		else if (typeLower == "nmap") return eMaterialTexture_Normal;
		else if (typeLower == "specular") return eMaterialTexture_Specular;
		else if (typeLower == "refraction") return eMaterialTexture_Refraction;
		// Special case: "illumination" is unused but is present without data in many .mat files
		else if (typeLower == "illumination") return eMaterialTexture_None;

		Warning("Skipping unsupported texture type `%s`\n", type.c_str());
		return eMaterialTexture_None;
	}

	static eTextureWrap GetWrap(const tString& asType)
	{
		auto typeLower = cString::ToLowerCase(asType);
		if (typeLower == "repeat") return eTextureWrap_Repeat;
		else if (typeLower == "clamptoedge") return eTextureWrap_ClampToEdge;
		else if (typeLower == "clamp") return eTextureWrap_ClampToBorder;

		Warning("Ignoring unsupported texture wrap mode `%s`\n", asType.c_str());
		return eTextureWrap_Repeat;
	}

	static eTextureAnimMode GetAnimMode(const tString& asType)
	{
		auto typeLower = cString::ToLowerCase(asType);
		if (typeLower.length() == 0 || typeLower == "none") return eTextureAnimMode_None;
		else if (typeLower == "loop") return eTextureAnimMode_Loop;
		else if (typeLower == "oscillate") return eTextureAnimMode_Oscillate;

		Warning("Ignoring unsupported animation mode `%s`\n", asType.c_str());
		return eTextureAnimMode_None;
	}

	//-----------------------------------------------------------------------

	struct TextureWithType {
		iTexture* texture;
		eMaterialTexture type;
	};

	TextureWithType CreateTextureFromElement(TiXmlElement* element, cTextureManager* texMgr, float anisotropy) {
		tString sFile = cString::ToString(element->Attribute("File"),"");
		eTextureAnimMode animMode = GetAnimMode(cString::ToString(element->Attribute("AnimMode"), "None"));
		eMaterialTexture textureType = GetTextureType(element->Value());

		// ignore placeholder/empty nodes
		if (textureType == eMaterialTexture_None || sFile=="")
		{
			return { nullptr, eMaterialTexture_None };
		}

		iTexture* texture;
		if (animMode != eTextureAnimMode_None)
			// TODO: [Rehatched] unify these two APIs, this is a mess
			texture = texMgr->CreateAnim2D(sFile, animMode);
		else
			texture = texMgr->Create2D(sFile);

		if (texture == nullptr) {
			Error("Couldn't load material texture!\n");
			return { nullptr, eMaterialTexture_None };
		}

		float fFrameTime = cString::ToFloat(element->Attribute("AnimFrameTime"), 1.0f);
		eTextureWrap wrap = GetWrap(cString::ToString(element->Attribute("Wrap"),""));
		texture->SetFrameTime(fFrameTime);

		texture->SetWrapAll(wrap);
		texture->SetFilter(eTextureFilter_Trilinear);
		texture->SetAnisotropyDegree(anisotropy);

		return { .texture = texture, .type = textureType };
	}

	//-----------------------------------------------------------------------

	iMaterial* cMaterialManager::LoadFromFile(const tString& asName, const tString& asPath)
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
		tString sPhysicsMatName = cString::ToString(pMain->Attribute("PhysicsMaterial"),"Default");

		iMaterial* pMat = mpGraphics->GetMaterialHandler()->Create(asName, sType);
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

		for (auto texElem = pTexRoot->FirstChildElement(); texElem != nullptr; texElem = texElem->NextSiblingElement())
		{
			auto [texture, texType] = CreateTextureFromElement(texElem, _textureManager, mfTextureAnisotropy);
			if (texture)
				pMat->SetTexture(texture, texType);
		}

		///////////////////////////
		//Custom
		pMat->LoadData(pRoot);

		delete pDoc;

		return pMat;
	}

	//-----------------------------------------------------------------------
}
