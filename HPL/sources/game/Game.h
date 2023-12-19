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
#ifndef HPL_GAME_H
#define HPL_GAME_H

#include "system/StringTypes.h"

namespace hpl {
	class cUpdater;
	class iLowLevelGameSetup;
	class cLogicTimer;

	class cInput;
	class cResources;
	class cImageManager;
	class cGpuProgramManager;
	class cTextureManager;
	class iLowLevelGraphics;
	class cRenderer3D;
	class cGraphicsDrawer;
	class cScene;
	class cSound;
	class cScript;
	class cPhysics;

	class cFPSCounter
	{
	public:
		cFPSCounter();

		void AddFrame();

		float mfFPS;
		float mfUpdateRate;
	private:
		int mlFramecounter;
		float mfFrametimestart;
		float mfFrametime;
	};

	//---------------------------------------------------

	struct GameSetupOptions {
		int ScreenWidth = 800;
		int ScreenHeight = 600;
		bool Fullscreen = false;
		tString AudioDeviceName{};
		tString WindowCaption{};
	};

	//---------------------------------------------------

	class cGame
	{
	public:
		cGame(iLowLevelGameSetup *setup, GameSetupOptions &options);
		~cGame();
	private:
		void GameInit(iLowLevelGameSetup *setup, GameSetupOptions &options);

	public:
		void Run();
		void Exit();

		// all the managers and modules
		iLowLevelGraphics* GetLowLevelGraphics() { return _llGfx; }
		cRenderer3D* GetRenderer() { return _renderer; }
		cGraphicsDrawer* GetDrawer() { return _drawer; }
		cImageManager* GetImageManager() { return _imageMgr; }
		cGpuProgramManager* GetShaderManager() { return _shaderMgr; }
		cTextureManager* GetTextureManager() { return _textureMgr; }

		cScene* GetScene() { return mpScene; }
		cResources* GetResources() { return mpResources; }
		cUpdater* GetUpdater() { return mpUpdater; }
		cInput* GetInput() { return mpInput; }
		cSound* GetSound() { return mpSound; }
		cPhysics* GetPhysics() { return mpPhysics; }
		cScript* GetScript() { return mpScript; }

		// logic update timing
		void ResetLogicTimer();
		void SetUpdatesPerSec(int alUpdatesPerSec);
		int GetUpdatesPerSec();
		float GetStepSize();
		cLogicTimer* GetLogicTimer(){ return mpLogicTimer;}

		// fps stuff
		float GetFPS();
		void SetFPSUpdateRate(float afSec);
		float GetFPSUpdateRate();
		void SetLimitFPS(bool abX){ mbLimitFPS = abX;}
		bool GetLimitFPS(){ return mbLimitFPS;}

		// render timing
		void SetRenderOnce(bool abX){mbRenderOnce = abX;}
		float GetFrameTime(){ return mfFrameTime;}
		float GetUpdateTime(){ return mfUpdateTime;}
		double GetGameTime(){ return mfGameTime;}


	private:
		bool mbGameIsDone;
		bool mbRenderOnce;

		float mfFrameTime;
		float mfUpdateTime;
		double mfGameTime;

		cUpdater *mpUpdater;
		cLogicTimer *mpLogicTimer;

		cFPSCounter* mpFPSCounter;
		bool mbLimitFPS;

		//Modules that Game connnect to:
		iLowLevelGraphics *_llGfx;
		cImageManager *_imageMgr;
		cGpuProgramManager *_shaderMgr;
		cTextureManager *_textureMgr;

		cRenderer3D *_renderer;
		cGraphicsDrawer *_drawer;
		cResources *mpResources;
		cInput *mpInput;
		cScene *mpScene;
		cSound *mpSound;
		cPhysics *mpPhysics;
		cScript *mpScript;
	};

};
#endif // HPL_GAME_H
