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
#include "scene/Scene.h"
#include "game/Updater.h"

#include "resources/Resources.h"
#include "sound/Sound.h"
#include "sound/LowLevelSound.h"
#include "sound/SoundHandler.h"
#include "scene/Camera.h"
#include "scene/World3D.h"
#include "graphics/Renderer3D.h"
#include "graphics/GraphicsDrawer.h"
#include "graphics/RenderList.h"
#include "resources/ScriptManager.h"
#include "script/Script.h"

#include "physics/Physics.h"

#include "resources/FileSearcher.h"
#include "resources/MeshLoaderHandler.h"

#include "system/Log.h"
#include "system/String.h"
#include "system/STLHelpers.h"
#include "system/UpdateTimerMacros.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cScene::cScene(iLowLevelGraphics *llGfx, cRenderer3D *renderer, cGraphicsDrawer *drawer, cTextureManager *apTextureMgr, cResources *apResources, cSound* apSound, cPhysics *apPhysics)
		: iUpdateable("HPL_Scene")
	{
		_llGfx = llGfx;
		_renderer = renderer;
		_drawer = drawer;
		_textureMgr = apTextureMgr;
		mpResources = apResources;
		mpSound = apSound;
		mpPhysics = apPhysics;

		mpCurrentWorld3D = NULL;

		mbDrawScene = true;
		mbUpdateMap = true;

		mpActiveCamera = NULL;
	}

	//-----------------------------------------------------------------------

	cScene::~cScene()
	{
		Log("Exiting Scene Module\n");
		Log("--------------------------------------------------------\n");

		STLDeleteAll(mlstWorld3D);
		STLDeleteAll(mlstCamera);

		Log("--------------------------------------------------------\n\n");

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cCamera *cScene::CreateCamera(eCameraMoveMode aMoveMode)
	{
		auto pCamera = new cCamera();
		pCamera->SetAspect(_llGfx->GetScreenSize().x / _llGfx->GetScreenSize().y);

		//Add Camera to list
		mlstCamera.push_back(pCamera);

		return pCamera;
	}


	//-----------------------------------------------------------------------

	void cScene::DestroyCamera(cCamera* apCam)
	{
		for(tCameraListIt it=mlstCamera.begin(); it!=mlstCamera.end();it++)
		{
			if(*it == apCam){
				delete *it;
				mlstCamera.erase(it);
				break;
			}
		}
	}

	//-----------------------------------------------------------------------
	void cScene::SetCamera(cCamera* pCam)
	{
		mpActiveCamera = pCam;
	}

	//-----------------------------------------------------------------------

	cScriptVar* cScene::CreateLocalVar(const tString& asName)
	{
		cScriptVar* pVar = GetLocalVar(asName);
		if(pVar==NULL)
		{
			cScriptVar Var;
			Var.msName = asName;
			m_mapLocalVars.insert(tScriptVarMap::value_type(cString::ToLowerCase(asName),Var));
			pVar= GetLocalVar(asName);
			if(pVar==NULL)FatalError("Very strange error when creating script var!\n");
		}
		return pVar;
	}

	//-----------------------------------------------------------------------

	cScriptVar* cScene::GetLocalVar(const tString& asName)
	{
		tScriptVarMapIt it = m_mapLocalVars.find(cString::ToLowerCase(asName));
		if(it==m_mapLocalVars.end()) return NULL;

		return &it->second;
	}

	//-----------------------------------------------------------------------

	tScriptVarMap* cScene::GetLocalVarMap()
	{
		return &m_mapLocalVars;
	}

	//-----------------------------------------------------------------------

	cScriptVar* cScene::CreateGlobalVar(const tString& asName)
	{
		cScriptVar* pVar;
		pVar= GetGlobalVar(asName);
		if(pVar==NULL)
		{
			cScriptVar Var;
			Var.msName = asName;
			m_mapGlobalVars.insert(tScriptVarMap::value_type(cString::ToLowerCase(asName),Var));
			pVar= GetGlobalVar(asName);
			if(pVar==NULL)FatalError("Very strange error when creating script var!\n");
		}
		return pVar;
	}

	//-----------------------------------------------------------------------

	cScriptVar* cScene::GetGlobalVar(const tString& asName)
	{
		tScriptVarMapIt it = m_mapGlobalVars.find(cString::ToLowerCase(asName));
		if(it==m_mapGlobalVars.end()) return NULL;

		return &it->second;
	}

	//-----------------------------------------------------------------------

	tScriptVarMap* cScene::GetGlobalVarMap()
	{
		return &m_mapGlobalVars;
	}

	//-----------------------------------------------------------------------

	void cScene::UpdateRenderList(float afFrameTime)
	{
		if(mbDrawScene && mpActiveCamera)
		{
			if(mpCurrentWorld3D)
				_renderer->UpdateRenderList(mpCurrentWorld3D, mpActiveCamera, afFrameTime);
		}
	}

	//-----------------------------------------------------------------------

	void cScene::SetDrawScene(bool abX)
	{
		mbDrawScene = abX;
		_renderer->GetRenderList()->Clear();
	}

	//-----------------------------------------------------------------------

	void cScene::Render(cUpdater* apUpdater, float afFrameTime)
	{
		if(mbDrawScene && mpActiveCamera)
		{
			if(mpCurrentWorld3D)
			{
				START_TIMING(RenderWorld)
				_renderer->RenderWorld(mpCurrentWorld3D, mpActiveCamera, afFrameTime);
				STOP_TIMING(RenderWorld)
			}

			START_TIMING(PostSceneDraw)
			apUpdater->OnPostSceneDraw();
			STOP_TIMING(PostSceneDraw)

			START_TIMING(PostEffects)
			// GetRendererPostEffects()->Render();
			STOP_TIMING(PostEffects)
		}
		else
		{
			apUpdater->OnPostSceneDraw();
		}
		_drawer->Render();

		apUpdater->OnPostGUIDraw();
	}

	//-----------------------------------------------------------------------

	void cScene::Update(float afTimeStep)
	{
		if(mpActiveCamera==NULL)return;

		mpSound->GetLowLevel()->SetListenerAttributes(
				mpActiveCamera->GetPosition(),
				cVector3f(0,0,0),
				mpActiveCamera->GetForward()*-1.0f,
				mpActiveCamera->GetUp());

		if(mbUpdateMap && mpCurrentWorld3D)
		{
			mpCurrentWorld3D->Update(afTimeStep);

			if(mpCurrentWorld3D->GetScript())
			{
				mpCurrentWorld3D->GetScript()->Run("OnUpdate()");
			}
		}
	}

	//-----------------------------------------------------------------------

	void cScene::Reset()
	{
		m_mapLocalVars.clear();
		m_mapGlobalVars.clear();
		m_setLoadedMaps.clear();
	}

	//-----------------------------------------------------------------------

	cWorld3D* cScene::LoadWorld3D(const tString& asFile, bool abLoadScript, tWorldLoadFlag aFlags)
	{
		//Clear the local script
		m_mapLocalVars.clear();

		///////////////////////////////////
		// Load the map file
		tString asPath = FileSearcher::GetFilePath(asFile);
		if(asPath == ""){
			Error("World '%s' doesn't exist\n",asFile.c_str());
			return NULL;
		}

		cWorld3D* pWorld = mpResources->GetMeshLoaderHandler()->LoadWorld(asPath, aFlags);
		if(pWorld==NULL){
			Error("Couldn't load world from '%s'\n",asPath.c_str());
			return NULL;
		}

		////////////////////////////////////////////////////////////
		//Load the script
		cScriptModule* pScript = NULL;
		if(abLoadScript)
		{
			tString sScriptFile = cString::SetFileExt(asFile,"hps");
			pScript = mpResources->GetScriptManager()->CreateScript(sScriptFile);
			if(pScript==NULL){
				Error("Couldn't load script '%s'\n",sScriptFile.c_str());
			}
			else
			{
				pWorld->SetScript(pScript);
			}
		}

		SetWorld3D(pWorld);

		////////////////////////////
		//Add to loaded maps
		
		tString sName = cString::ToLowerCase(cString::SetFileExt(asFile,""));
		if (m_setLoadedMaps.find(sName) == m_setLoadedMaps.end()) // when in cpp20 use .contains
		{
			m_setLoadedMaps.insert(sName);
		}

		////////////////////////////////////////////////////////////
		//Run script start functions
		/*if(pScript)
		{
			//Check if the map has been loaded before, if not run OnStart script.
			tString sName = cString::ToLowerCase(cString::SetFileExt(asFile,""));
			auto it = m_setLoadedMaps.find(sName);
			if(it == m_setLoadedMaps.end())
			{
				m_setLoadedMaps.insert(sName);
				pScript->Run("OnStart()");
			}
		}*/

		return pWorld;
	}

	//-----------------------------------------------------------------------

	cWorld3D* cScene::CreateWorld3D(const tString& asName)
	{
		cWorld3D* pWorld = new cWorld3D(asName, _llGfx, _textureMgr, mpResources, mpSound, mpPhysics, this);

		mlstWorld3D.push_back(pWorld);

		return pWorld;
	}

	//-----------------------------------------------------------------------

	void cScene::DestroyWorld3D(cWorld3D* apWorld)
	{
		STLFindAndDelete(mlstWorld3D,apWorld);
	}

	//-----------------------------------------------------------------------

	void cScene::SetWorld3D(cWorld3D* apWorld)
	{
		mpCurrentWorld3D = apWorld;

		//Set the world the sound handler uses.
		mpSound->GetSoundHandler()->SetWorld3D(mpCurrentWorld3D);
		//Set the world for physics.
		mpPhysics->SetGameWorld(mpCurrentWorld3D);

	}

	//-----------------------------------------------------------------------

	bool cScene::HasLoadedWorld(const tString &asFile)
	{
		tString sName = cString::ToLowerCase(cString::SetFileExt(asFile,""));
		return m_setLoadedMaps.find(sName) != m_setLoadedMaps.end();
	}

	//-----------------------------------------------------------------------
}
