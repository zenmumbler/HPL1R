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
#include "game/Game.h"

#include "system/System.h"
#include "system/Log.h"
#include "system/LogicTimer.h"
#include "system/String.h"
#include "input/Input.h"
#include "input/Mouse.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"
#include "graphics/LowLevelGraphics.h"
#include "game/Updater.h"
#include "script/ScriptFuncs.h"
#include "graphics/Renderer3D.h"

#include "game/LowLevelGameSetup.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// FPS COUNTER
	//////////////////////////////////////////////////////////////////////////

	cFPSCounter::cFPSCounter()
	{
		mfFPS = 60;

		mlFramecounter=0;
		mfFrametimestart=0;
		mfFrametime=0;

		mfUpdateRate = 1;

		mfFrametimestart = GetAppTimeFloat();
	}

	void cFPSCounter::AddFrame()
	{
		mlFramecounter++;

		mfFrametime = GetAppTimeFloat() - mfFrametimestart;

		// update the timer
		if (mfFrametime >= mfUpdateRate)
		{
			mfFPS = ((float)mlFramecounter)/mfFrametime;
			mlFramecounter = 0;
			mfFrametimestart = GetAppTimeFloat();
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGame::cGame(iLowLevelGameSetup *apGameSetup, GameSetupOptions &options)
	{
		GameInit(apGameSetup, options);
	}


	//-----------------------------------------------------------------------

	cGame::cGame(iLowLevelGameSetup *apGameSetup, int alWidth, int alHeight, bool abFullscreen, int alMultisampling)
	{
		GameSetupOptions options{};
		options.ScreenWidth = alWidth;
		options.ScreenHeight = alHeight;
		options.Fullscreen = abFullscreen;
		options.Multisampling = alMultisampling;
		GameInit(apGameSetup, options);
	}

	//-----------------------------------------------------------------------

	void cGame::GameInit(iLowLevelGameSetup *apGameSetup, GameSetupOptions &options)
	{
		mpGameSetup = apGameSetup;

		Log("Creating Engine Modules\n");
		Log("--------------------------------------------------------\n");

		//Create the modules that game connects to and init them!
		Log(" Creating graphics module\n");
		mpGraphics = mpGameSetup->CreateGraphics();

		Log(" Creating resource module\n");
		mpResources = mpGameSetup->CreateResources();

		Log(" Creating input module\n");
		mpInput = mpGameSetup->CreateInput();

		Log(" Creating sound module\n");
		mpSound = mpGameSetup->CreateSound();

		Log(" Creating physics module\n");
		mpPhysics = mpGameSetup->CreatePhysics();

		Log(" Creating ai module\n");
		mpAI = mpGameSetup->CreateAI();

		Log(" Creating script module\n");
		mpScript = mpGameSetup->CreateScript(mpResources);

		Log(" Creating scene module\n");
		mpScene = new cScene(mpGraphics, mpResources, mpSound,mpPhysics, mpAI);

		Log("--------------------------------------------------------\n\n");


		//Init the resources
		mpResources->Init(mpGraphics, mpSound, mpScript, mpScene);

		//Init the graphics
		mpGraphics->Init(options.ScreenWidth,
			options.ScreenHeight,
			options.Fullscreen,
			options.Multisampling,
			options.WindowCaption,
			mpResources);

		//Init Sound
		mpSound->Init(mpResources, options.AudioDeviceName);

		//Init physics
		mpPhysics->Init(mpResources);

		//Init AI
		mpAI->Init();

		Log("Initializing Game Module\n");
		Log("--------------------------------------------------------\n");
		//Create the updatehandler
		Log(" Adding engine updates\n");
		mpUpdater = new cUpdater();

		//Add some loaded modules to the updater
		mpUpdater->AddGlobalUpdate(mpInput);
		mpUpdater->AddGlobalUpdate(mpPhysics);
		mpUpdater->AddGlobalUpdate(mpScene);
		mpUpdater->AddGlobalUpdate(mpSound);
		mpUpdater->AddGlobalUpdate(mpAI);
		mpUpdater->AddGlobalUpdate(mpResources);
		mpUpdater->AddGlobalUpdate(mpScript);

		//Setup the "default" updater container
		mpUpdater->AddContainer("Default");
		mpUpdater->SetContainer("Default");

		//Create the logic timer.
		mpLogicTimer = new cLogicTimer(60);

		//Init some standard script funcs
		Log(" Initializing script functions\n");
		RegisterCoreFunctions(mpScript,mpGraphics,mpResources,mpInput,mpScene,mpSound,this);

		//Since game is not done:
		mbGameIsDone = false;

		mbRenderOnce = false;

		mfUpdateTime =0;
		mfGameTime =0;

		mbLimitFPS = true;

		mpFPSCounter = new cFPSCounter();
		Log("--------------------------------------------------------\n\n");

		Log("User Initialization\n");
		Log("--------------------------------------------------------\n");
	}

	//-----------------------------------------------------------------------

	cGame::~cGame()
	{
		Log("--------------------------------------------------------\n\n");

		delete mpLogicTimer;
		delete mpFPSCounter;

		delete mpUpdater;

		delete mpScene;
		delete mpInput;
		delete mpSound;
		delete mpGraphics;
		delete mpResources;
		delete mpPhysics;
		delete mpAI;

		Log(" Deleting game setup provided by user\n");
		delete mpGameSetup;

		Log("HPL Exit was successful!\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	int glClearUpdateCheck=0;
	void cGame::Run()
	{
		//Log line that ends user init.
		Log("--------------------------------------------------------\n\n");

		bool bDone = false;
		double fNumOfTimes=0;
		double fMediumTime=0;

		mpUpdater->OnStart();

		mpLogicTimer->Reset();

		//reset the mouse, really reset the damn thing :P
		for(int i=0;i<10;i++) mpInput->GetMouse()->Reset();


		Log("Game Running\n");
		Log("--------------------------------------------------------\n");

		mfFrameTime = 0;
		float tempFrameTime = GetAppTimeFloat();

		bool mbIsUpdated = true;

		while(!mbGameIsDone)
		{
			//Log("-----------------\n");
			//////////////////////////
			//Update logic.
			while(mpLogicTimer->WantUpdate() && !mbGameIsDone)
			{
				float updateTime = GetAppTimeFloat();
				mpUpdater->Update(GetStepSize());
				mfUpdateTime = GetAppTimeFloat() - updateTime;

				mbIsUpdated = true;

				glClearUpdateCheck++;
				if(glClearUpdateCheck % 500 == 0)
				{
					if(mpUpdater->GetCurrentContainerName() == "Default") ClearUpdateLogFile();
				}

				mfGameTime += GetStepSize();
			}
			mpLogicTimer->EndUpdateLoop();


			//If not making a single rendering is better to use gpu and
			//cpu at the same time and make query checks etc after logic update.
			//If any delete has occured in the update this might crash. so skip it for now.
			/*if(mbRenderOnce==false)	{
				mpGraphics->GetRenderer3D()->FetchOcclusionQueries();
				mpUpdater->OnPostBufferSwap();
			}*/

			//Draw graphics!
			if(mbRenderOnce && bDone)continue;
			if(mbRenderOnce)bDone = true;


			if(mbIsUpdated)
			{
				mpScene->UpdateRenderList(mfFrameTime);
				if(mbLimitFPS==false) mbIsUpdated = false;
			}

			if(mbLimitFPS==false || mbIsUpdated)
			{
				//LogUpdate("----------- RENDER GFX START --------------\n");

				mbIsUpdated = false;

				//Get the the from the last frame.
				mfFrameTime = GetAppTimeFloat() - tempFrameTime;
				tempFrameTime = GetAppTimeFloat();

				//Draw this frame
				mpGraphics->GetLowLevel()->StartFrame();
				mpUpdater->OnDraw();
				mpScene->Render(mpUpdater,mfFrameTime);

				//Update fps counter.
				mpFPSCounter->AddFrame();

				//Update the screen.
				mpGraphics->GetLowLevel()->EndFrame();
				//if(mbRenderOnce)
				{
					mpGraphics->GetRenderer3D()->FetchOcclusionQueries();
					mpUpdater->OnPostBufferSwap();
				}

				fNumOfTimes++;
			}
		}
		Log("--------------------------------------------------------\n\n");

		Log("User Exit\n");
		Log("--------------------------------------------------------\n");

		mpUpdater->OnExit();
	}
	//-----------------------------------------------------------------------

	void cGame::Exit()
	{
		mbGameIsDone = true;
	}
	//-----------------------------------------------------------------------

	void cGame::ResetLogicTimer()
	{
		mpLogicTimer->Reset();
	}

	void cGame::SetUpdatesPerSec(int alUpdatesPerSec)
	{
		mpLogicTimer->SetUpdatesPerSec(alUpdatesPerSec);
	}

	int cGame::GetUpdatesPerSec()
	{
		return mpLogicTimer->GetUpdatesPerSec();
	}

	float cGame::GetStepSize()
	{
		return mpLogicTimer->GetStepSize();
	}

	//-----------------------------------------------------------------------

	cScene* cGame::GetScene()
	{
		return mpScene;
	}

	//-----------------------------------------------------------------------

	cResources* cGame::GetResources()
	{
		return mpResources;
	}

	//-----------------------------------------------------------------------

	cGraphics* cGame::GetGraphics()
	{
		return mpGraphics;
	}

	//-----------------------------------------------------------------------

	cInput* cGame::GetInput()
	{
		return mpInput;
	}

	//-----------------------------------------------------------------------

	cSound* cGame::GetSound()
	{
		return mpSound;
	}

	//-----------------------------------------------------------------------

	cPhysics* cGame::GetPhysics()
	{
		return mpPhysics;
	}

	//-----------------------------------------------------------------------

	cAI* cGame::GetAI()
	{
		return mpAI;
	}

	//-----------------------------------------------------------------------

	cScript* cGame::GetScript()
	{
		return mpScript;
	}

	//-----------------------------------------------------------------------

	cUpdater* cGame::GetUpdater()
	{
		return mpUpdater;
	}

	float cGame::GetFPS()
	{
		return mpFPSCounter->mfFPS;
	}

	//-----------------------------------------------------------------------

	void cGame::SetFPSUpdateRate(float afSec)
	{
		mpFPSCounter->mfUpdateRate = afSec;
	}
	float cGame::GetFPSUpdateRate()
	{
		return mpFPSCounter->mfUpdateRate;
	}

	//-----------------------------------------------------------------------
}
