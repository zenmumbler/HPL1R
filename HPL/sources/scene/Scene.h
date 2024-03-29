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
#ifndef HPL_SCENE_H
#define HPL_SCENE_H

#include "script/ScriptVar.h"

#include "game/Updateable.h"
#include "scene/Camera.h"

#include "resources/MeshLoader.h"
#include "system/StringTypes.h"

#include <list>
#include <set>

namespace hpl {

	class iLowLevelGraphics;
	class cTextureManager;
	class cRenderer3D;
	class cGraphicsDrawer;
	class cResources;
	class cSound;
	class cPhysics;
	class cCamera;
	class cUpdater;
	class cWorld3D;

	typedef std::list<cCamera*> tCameraList;
	typedef tCameraList::iterator tCameraListIt;

	typedef std::list<cWorld3D*> tWorld3DList;
	typedef tWorld3DList::iterator tWorld3DListIt;

	class cScene : public iUpdateable
	{
	public:
		cScene(iLowLevelGraphics *llGfx, cRenderer3D *renderer, cGraphicsDrawer *drawer, cTextureManager *apTextureMgr, cResources *apResources, cSound* apSound, cPhysics *apPhysics);
		~cScene();

		void Reset();

		/**
		* Called by cGame
		*/
		void UpdateRenderList(float afFrameTime);
		/**
		 * Called by cGame
		 */
		void Render(cUpdater* apUpdater, float afFrameTime);

		void Update(float afTimeStep);

		void SetDrawScene(bool abX);
		bool GetDrawScene(){ return mbDrawScene;}

		///// SCRIPT VAR METHODS ////////////////////

		cScriptVar* CreateLocalVar(const tString& asName);
		cScriptVar* GetLocalVar(const tString& asName);
		tScriptVarMap* GetLocalVarMap();
		cScriptVar* CreateGlobalVar(const tString& asName);
		cScriptVar* GetGlobalVar(const tString& asName);
		tScriptVarMap* GetGlobalVarMap();

		///// CAMERA METHODS ////////////////////

		cCamera* CreateCamera(eCameraMoveMode aMoveMode);

		void DestroyCamera(cCamera* apCam);

		/**
		 * This sets the current camera
		 * \param pCam
		 */
		void SetCamera(cCamera* pCam);
		cCamera* GetCamera(){ return mpActiveCamera; }

		///// WORLD METHODS ////////////////////

		cWorld3D* LoadWorld3D(const tString& asFile, bool abLoadScript, tWorldLoadFlag aFlags);
		cWorld3D* CreateWorld3D(const tString& asName);
		void DestroyWorld3D(cWorld3D* apWorld);

		void SetWorld3D(cWorld3D* apWorld);
		cWorld3D* GetWorld3D(){ return mpCurrentWorld3D;}

		void ClearLoadedMaps(){m_setLoadedMaps.clear();}
		std::set<tString>* GetLoadedMapsSet(){ return &m_setLoadedMaps;}
		bool HasLoadedWorld(const tString &asFile);

		void SetUpdateMap(bool abX){ mbUpdateMap = abX;}
		bool GetUpdateMap(){return mbUpdateMap;}

	private:
		iLowLevelGraphics *_llGfx;
		cTextureManager *_textureMgr;
		cRenderer3D *_renderer;
		cGraphicsDrawer *_drawer;
		cResources *mpResources;
		cSound *mpSound;
		cPhysics *mpPhysics;

		bool mbDrawScene;
		bool mbUpdateMap;

		tWorld3DList mlstWorld3D;
		cWorld3D* mpCurrentWorld3D;

		tCameraList mlstCamera;
		cCamera *mpActiveCamera;

		tScriptVarMap m_mapLocalVars;
		tScriptVarMap m_mapGlobalVars;

		std::set<tString> m_setLoadedMaps;
	};

};
#endif // HPL_SCENE_H
