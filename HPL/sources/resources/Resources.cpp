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
#include "resources/Resources.h"

#include "resources/FileSearcher.h"
#include "resources/ImageManager.h"
#include "resources/GpuProgramManager.h"
#include "resources/ParticleManager.h"
#include "resources/SoundManager.h"
#include "resources/FontManager.h"
#include "resources/ScriptManager.h"
#include "resources/TextureManager.h"
#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/SoundEntityManager.h"
#include "resources/AnimationManager.h"
#include "resources/ConfigFile.h"
#include "resources/LanguageFile.h"
#include "resources/impl/MeshLoaderGLTF2.h"
#include "resources/impl/MeshLoaderCollada.h"

#include "system/Log.h"
#include "system/Files.h"

#include "tinyXML/tinyxml.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cResources::cResources(iLowLevelGraphics *apLowLevelGraphics)
		: iUpdateable("Resources")
	{
		mpLowLevelGraphics = apLowLevelGraphics;

		mpFileSearcher = new cFileSearcher();

		mpDefaultEntity3DLoader = NULL;
		mpDefaultArea3DLoader = NULL;

		mpLanguageFile = NULL;
	}

	//-----------------------------------------------------------------------

	cResources::~cResources()
	{
		Log("Exiting Resources Module\n");
		Log("--------------------------------------------------------\n");

		STLMapDeleteAll(m_mEntity3DLoaders);
		STLMapDeleteAll(m_mArea3DLoaders);

		delete mpFontManager;
		delete mpScriptManager;
		delete mpParticleManager;
		delete mpSoundManager;
		delete mpMeshManager;
		delete mpMaterialManager;
		delete mpGpuProgramManager;
		delete mpImageManager;
		delete mpTextureManager;
		delete mpSoundEntityManager;
		delete mpAnimationManager;

		Log(" All resources deleted\n");

		delete mpFileSearcher;
		delete mpMeshLoaderHandler;

		if(mpLanguageFile) delete mpLanguageFile;

		mlstManagers.clear();
		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cResources::Init(cGraphics* apGraphics, cSound *apSound, cScript *apScript, cScene *apScene)
	{
		Log("Initializing Resources Module\n");
		Log("--------------------------------------------------------\n");

		Log(" Setting default directories\n");
		AddBaseDirectories();

		Log(" Creating resource managers\n");

		mpImageManager = new cImageManager(mpFileSearcher, mpLowLevelGraphics);
		mlstManagers.push_back(mpImageManager);
		mpGpuProgramManager = new cGpuProgramManager(mpFileSearcher, mpLowLevelGraphics);
		mlstManagers.push_back(mpGpuProgramManager);
		mpParticleManager = new cParticleManager(apGraphics, this);
		mlstManagers.push_back(mpParticleManager);
		mpSoundManager = new cSoundManager(apSound, this);
		mlstManagers.push_back(mpParticleManager);
		mpFontManager = new cFontManager(apGraphics, this);
		mlstManagers.push_back(mpFontManager);
		mpScriptManager = new cScriptManager(apScript, this);
		mlstManagers.push_back(mpScriptManager);
		mpTextureManager = new cTextureManager(apGraphics, this);
		mlstManagers.push_back(mpTextureManager);
		mpMaterialManager = new cMaterialManager(apGraphics, this);
		mlstManagers.push_back(mpMaterialManager);
		mpMeshManager = new cMeshManager(apGraphics, this);
		mlstManagers.push_back(mpMeshManager);
		mpSoundEntityManager = new cSoundEntityManager(apSound, this);
		mlstManagers.push_back(mpSoundEntityManager);
		mpAnimationManager = new cAnimationManager(apGraphics, this);
		mlstManagers.push_back(mpAnimationManager);

		Log(" Misc Creation\n");

		mpMeshLoaderHandler = new cMeshLoaderHandler(this, apScene);
		mpMeshLoaderHandler->AddLoader(new cMeshLoaderGLTF2(mpLowLevelGraphics));
		mpMeshLoaderHandler->AddLoader(new cMeshLoaderCollada(mpLowLevelGraphics));

		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	void cResources::Update(float afTimeStep)
	{
		tResourceManagerListIt it = mlstManagers.begin();
		for(; it != mlstManagers.end(); ++it)
		{
			iResourceManager *pManager = *it;

			pManager->Update(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	cFileSearcher* cResources::GetFileSearcher()
	{
		return mpFileSearcher;
	}

	//-----------------------------------------------------------------------

	bool cResources::LoadResourceDirsFile(const tString &asFile)
	{
		TiXmlDocument* pXmlDoc = new TiXmlDocument(asFile.c_str());
		if(pXmlDoc->LoadFile()==false)
		{
			Error("Couldn't load XML file '%s'!\n",asFile.c_str());
			delete  pXmlDoc;
			return false;
		}

		//Get the root.
		TiXmlElement* pRootElem = pXmlDoc->RootElement();

		TiXmlElement* pChildElem = pRootElem->FirstChildElement();
		for(; pChildElem != NULL; pChildElem = pChildElem->NextSiblingElement())
		{
			tString sPath = cString::ToString(pChildElem->Attribute("Path"),"");
			if(sPath==""){
				continue;
			}

			if(sPath[0]=='/' || sPath[0]=='\\') sPath = sPath.substr(1);

			AddResourceDir(sPath);
		}

		delete pXmlDoc;
		return true;
	}

	/**
	 * \todo File searcher should check so if the dir is allready added and if so return false and not add
	 * \param &asDir
	 * \param &asMask
	 * \return
	 */
	bool cResources::AddResourceDir(const tString &asDir, const tString &asMask)
	{
		mpFileSearcher->AddDirectory(asDir, asMask);
		if(iResourceBase::GetLogCreateAndDelete())
			Log(" Added resource directory '%s'\n",asDir.c_str());
		return true;
	}

	void cResources::AddBaseDirectories() {
		// core graphics files
		AddResourceDir("core/programs");
		AddResourceDir("core/textures");

		// rehatched core graphics overrides
		AddResourceDir("rehatched/core/programs");
		AddResourceDir("rehatched/core/textures");
	}

	/**
	 * Reset resource directory search paths to built-ins + game specific
	 * paths specified in a resources.cfg file.
	 * Ideally resource paths would be grouped and individually reset
	 * but that is not the HPL1 way.
	 */
	void cResources::SetupResourceDirsForLanguage(const tString &asLangFile)
	{
		mpFileSearcher->ClearDirectories();
		AddBaseDirectories();
		LoadResourceDirsFile("resources.cfg");
		LoadResourceDirsFile("rehatched/resources.cfg");
		SetLanguageFile(asLangFile);
	}

	//-----------------------------------------------------------------------

	bool cResources::SetLanguageFile(const tString &asFile)
	{
		// [ZM] made the decision to not use the file searcher for language files.
		// /config was already the de-facto only position for them and with resource
		// overloading this makes path handling setup less awkward.
		tString sOrigPath = "config/" + asFile;
		tString sRehatchedPath = "rehatched/config/" + asFile;

		if (FileExists(cString::To16Char(sOrigPath)) == false)
		{
			Error("Couldn't find language file '%s'\n",asFile.c_str());
			return false;
		}
		if (FileExists(cString::To16Char(sRehatchedPath)) == false)
		{
			sRehatchedPath = "rehatched/config/English.lang";
			if(FileExists(cString::To16Char(sRehatchedPath)) == false) {
				Error("Couldn't load language extensions file '%s'\n", sRehatchedPath.c_str());
				return false;
			}
			
			Warning("No localised language extensions file found for language '%s', using English fallback\n", asFile.c_str());
		}

		cLanguageFile *pNewLangFile = new cLanguageFile(this);

		bool bSuccess = pNewLangFile->LoadFromFile(sOrigPath);
		if (bSuccess==false) {
			delete pNewLangFile;
			return false;
		}
		bSuccess = pNewLangFile->LoadFromFile(sRehatchedPath);
		if (bSuccess==false) {
			delete pNewLangFile;
			return false;
		}

		// everything loaded, exchange old for new
		if(mpLanguageFile){
			delete mpLanguageFile;
		}
		mpLanguageFile = pNewLangFile;

		return true;
	}

	const tWString& cResources::Translate(const tString& asCat, const tString& asName)
	{
		if(mpLanguageFile)
		{
			return mpLanguageFile->Translate(asCat,asName);
		}
		else
		{
			return mwsEmptyString;
		}
	}

	//-----------------------------------------------------------------------

	void cResources::AddEntity3DLoader(iEntity3DLoader* apLoader, bool abSetAsDefault)
	{
		m_mEntity3DLoaders.insert(tEntity3DLoaderMap::value_type(apLoader->GetName(), apLoader));

		if(abSetAsDefault){
			mpDefaultEntity3DLoader = apLoader;
		}
	}

	iEntity3DLoader* cResources::GetEntity3DLoader(const tString& asName)
	{
		tEntity3DLoaderMapIt it = m_mEntity3DLoaders.find(asName);
		if(it == m_mEntity3DLoaders.end()){
			Warning("No loader for type '%s' found!\n",asName.c_str());

			if(mpDefaultEntity3DLoader){
				Log("Using default loader!\n");
				return mpDefaultEntity3DLoader;
			}
			else {
				return NULL;
			}
		}

		return it->second;
	}

	//-----------------------------------------------------------------------


	void cResources::AddArea3DLoader(iArea3DLoader* apLoader, bool abSetAsDefault)
	{
		m_mArea3DLoaders.insert(tArea3DLoaderMap::value_type(apLoader->GetName(), apLoader));

		if(abSetAsDefault){
			mpDefaultArea3DLoader = apLoader;
		}
	}

	iArea3DLoader* cResources::GetArea3DLoader(const tString& asName)
	{
		tArea3DLoaderMapIt it = m_mArea3DLoaders.find(asName);
		if(it == m_mArea3DLoaders.end()){
			Warning("No loader for area type '%s' found!\n",asName.c_str());

			if(mpDefaultArea3DLoader){
				Log("Using default loader!\n");
				return mpDefaultArea3DLoader;
			}
			else {
				return NULL;
			}
		}

		return it->second;
	}

	//-----------------------------------------------------------------------

}
