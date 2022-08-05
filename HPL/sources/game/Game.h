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

#include "system/SystemTypes.h"


namespace hpl {
	class cUpdater;
	class iLowLevelGameSetup;
	class cLogicTimer;

	class cInput;
	class cResources;
	class cGraphics;
	class cScene;
	class cSound;
	class cScript;
	class cPhysics;
	class cAI;

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
		int Multisampling = 0;

		tString AudioDeviceName{};
		tString WindowCaption{};
	};

	//---------------------------------------------------

	class cGame
	{
	public:
		cGame(iLowLevelGameSetup *apGameSetup, GameSetupOptions &options);
		cGame(iLowLevelGameSetup *apGameSetup, int alWidth, int alHeight, bool abFullscreen, int alMultisampling=0);
		~cGame();
	private:
		void GameInit(iLowLevelGameSetup *apGameSetup, GameSetupOptions &options);

	public:

		/**
		 * Starts the game loop. To make stuff run they must be added as updatables..
		 */
		void Run();
		/**
		 * Exists the game.
		 * \todo is this a good way to do it? Should game be global. If so, make a singleton.
		 */
		void Exit();

		/**
		*
		* \return A pointer to Scene
		*/
		cScene* GetScene();
		/**
		 *
		 * \return A pointer to Resources
		 */
		cResources* GetResources();

		/**
		 *
		 * \return A pointer to the Updater
		 */
		cUpdater* GetUpdater();

		/**
		*
		* \return A pointer to the Input
		*/
		cInput* GetInput();

		/**
		*
		* \return A pointer to the Graphics
		*/
		cGraphics* GetGraphics();
		/**
		*
		* \return A pointer to the Sound
		*/
		cSound* GetSound();
		/**
		*
		* \return A pointer to the Physics
		*/
		cPhysics* GetPhysics();
		/**
		*
		* \return A pointer to the AI
		*/
		cAI* GetAI();

		cScript* GetScript();

		void ResetLogicTimer();
		void SetUpdatesPerSec(int alUpdatesPerSec);
		int GetUpdatesPerSec();
		float GetStepSize();

		cLogicTimer* GetLogicTimer(){ return mpLogicTimer;}

		float GetFPS();

		void SetFPSUpdateRate(float afSec);
		float GetFPSUpdateRate();

		void SetRenderOnce(bool abX){mbRenderOnce = abX;}

		float GetFrameTime(){ return mfFrameTime;}

		float GetUpdateTime(){ return mfUpdateTime;}

		double GetGameTime(){ return mfGameTime;}

		void SetLimitFPS(bool abX){ mbLimitFPS = abX;}
		bool GetLimitFPS(){ return mbLimitFPS;}

	private:
		bool mbGameIsDone;

		bool mbRenderOnce;

		float mfFrameTime;

		float mfUpdateTime;

		double mfGameTime;

		iLowLevelGameSetup *mpGameSetup;
		cUpdater *mpUpdater;
		cLogicTimer *mpLogicTimer;

		cFPSCounter* mpFPSCounter;

		bool mbLimitFPS;

		//Modules that Game connnect to:
		cResources *mpResources;
		cInput *mpInput;
		cGraphics *mpGraphics;
		cScene *mpScene;
		cSound *mpSound;
		cPhysics *mpPhysics;
		cAI *mpAI;
		cScript *mpScript;
	};

};
#endif // HPL_GAME_H
