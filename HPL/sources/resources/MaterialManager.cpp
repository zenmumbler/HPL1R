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
#include "resources/TextureManager.h"
#include "resources/GpuProgramManager.h"
#include "resources/FileSearcher.h"

#include "graphics/Material.h"
#include "graphics/MaterialHandler.h"
#include "graphics/Material_Universal.h"

#include "system/String.h"
#include "system/Log.h"

#include "tinyXML/tinyxml.h"


namespace hpl {

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// STATIC METHODS
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

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterialManager::cMaterialManager(iLowLevelGraphics *llGfx, cTextureManager* textureManager, cGpuProgramManager* programManager)
		: iResourceManager{"material"}
		, _textureManager{textureManager}
		, _programManager{programManager}
	{
		mfTextureAnisotropy = 8.0f;

		_matHandler = new cMaterialHandler(llGfx, textureManager, programManager);
		_matHandler->AddType(new MaterialType_Universal());
	}

	cMaterialManager::~cMaterialManager() {
		delete _matHandler;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cMaterialManager::LoadAsset(const tString &name, const tString &fullPath) {
		auto doc = std::unique_ptr<TiXmlDocument>(new TiXmlDocument(fullPath.c_str()));
		if (! doc->LoadFile()) {
			return nullptr;
		}

		TiXmlElement *pRoot = doc->RootElement();

		///////////////////////////
		//Main
		TiXmlElement *pMain = pRoot->FirstChildElement("Main");
		if(pMain == nullptr){
			Error("Main child not found.\n");
			return nullptr;
		}

		auto sType = pMain->Attribute("Type");
		if (sType == nullptr){
			Error("Type not found.\n");
			return nullptr;
		}

		bool bUseAlpha = cString::ToBool(pMain->Attribute("UseAlpha"), false);
		tString sPhysicsMatName = cString::ToString(pMain->Attribute("PhysicsMaterial"),"Default");

		iMaterial* pMat = _matHandler->Create(name, sType);
		if (pMat == nullptr){
			Error("Invalid material type '%s'\n",sType);
			return nullptr;
		}

		pMat->SetHasAlpha(bUseAlpha);
		pMat->SetPhysicsMaterial(sPhysicsMatName);

		///////////////////////////
		//Textures
		auto texRoot = pRoot->FirstChildElement("TextureUnits");
		if (texRoot == nullptr){
			Error("TextureUnits child not found.\n");
			return nullptr;
		}

		for (auto texElem = texRoot->FirstChildElement(); texElem != nullptr; texElem = texElem->NextSiblingElement())
		{
			auto [texture, texType] = CreateTextureFromElement(texElem, _textureManager, mfTextureAnisotropy);
			if (texture)
				pMat->SetTexture(texture, texType);
		}

		///////////////////////////
		//Custom
		pMat->LoadData(pRoot);

		return pMat;
	}

	static const tString s_Extensions[] { "mat" };

	std::span<const tString> cMaterialManager::SupportedExtensions() const {
		return { s_Extensions };
	}

	//-----------------------------------------------------------------------

	iMaterial* cMaterialManager::CreateMaterial(const tString& name)
	{
		return static_cast<iMaterial*>(GetOrLoadResource(name));
	}

	//-----------------------------------------------------------------------

	void cMaterialManager::Update(float afTimeStep)
	{
		// [Rehatched]: Not currently called and no materials use or used this
		ForEachResource([=](iResourceBase *resource) {
			auto material = static_cast<iMaterial*>(resource);
			material->Update(afTimeStep);
		});
	}

	//-----------------------------------------------------------------------

	void cMaterialManager::SetTextureAnisotropy(float afX)
	{
		if(mfTextureAnisotropy == afX) return;
		mfTextureAnisotropy = afX;

		ForEachResource([this](iResourceBase *resource) {
			iMaterial *pMat = static_cast<iMaterial*>(resource);

			for (int i=0; i < eMaterialTexture_None; ++i)
			{
				iTexture *pTex = pMat->GetTexture((eMaterialTexture)i);
				if(pTex)pTex->SetAnisotropyDegree(mfTextureAnisotropy);
			}
		});
	}

	//-----------------------------------------------------------------------

	tString cMaterialManager::GetPhysicsMaterialName(const tString& name)
	{
		tString fullPath;
		auto qualifiedName = FileSearcher::ResolveAssetName(name, SupportedExtensions());
		auto material = static_cast<iMaterial*>(FindLoadedResource(qualifiedName, fullPath));

		if (material == nullptr && fullPath.length() > 0)
		{
			auto doc = new TiXmlDocument(fullPath.c_str());
			if (! doc->LoadFile()) {
				return "";
			}

			auto root = doc->RootElement();
			auto main = root->FirstChildElement("Main");
			if (main == NULL) {
				delete doc;
				Error("Main child not found in '%s'\n", fullPath.c_str());
				return "";
			}

			auto physicsMaterial = cString::ToString(main->Attribute("PhysicsMaterial"), "Default");
			delete doc;
			return physicsMaterial;
		}

		if (material)
			return material->GetPhysicsMaterial();
		else
			return "";
	}

}
