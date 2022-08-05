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
#ifdef WIN32
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif

#include "game/impl/SDLGameSetup.h"
#include "input/impl/KeyboardSDL.h"
#include "input/impl/MouseSDL.h"
#include "graphics/impl/LowLevelGraphicsSDL.h"
#include "input/impl/LowLevelInputSDL.h"
#include "sound/impl/LowLevelSoundOAL.h"
#include "physics/impl/LowLevelPhysicsNewton.h"
#include "system/Log.h"

#include <SDL2/SDL.h>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSDLGameSetup::cSDLGameSetup()
	{
		if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0) {
			FatalError("Error Initializing Display: %s",SDL_GetError());
			exit(1);
		}

		mpLowLevelGraphics = new cLowLevelGraphicsSDL();
		mpLowLevelInput = new cLowLevelInputSDL(mpLowLevelGraphics);
		mpLowLevelSound	= new cLowLevelSoundOAL();
		mpLowLevelPhysics = new cLowLevelPhysicsNewton();
	}

	//-----------------------------------------------------------------------

	cSDLGameSetup::~cSDLGameSetup()
	{
		Log("- Deleting lowlevel stuff.\n");

		Log("  Physics\n");
		delete mpLowLevelPhysics;
		Log("  Sound\n");
		delete mpLowLevelSound;
		Log("  Input\n");
		delete mpLowLevelInput;
		Log("  Graphics\n");
		delete mpLowLevelGraphics;

		SDL_Quit();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cResources* cSDLGameSetup::CreateResources()
	{
		cResources *pResources = new cResources(mpLowLevelGraphics);
		return pResources;
	}

	//-----------------------------------------------------------------------

	cInput* cSDLGameSetup::CreateInput()
	{
		cInput *pInput = new cInput(mpLowLevelInput);
		return pInput;
	}

	//-----------------------------------------------------------------------

	cGraphics* cSDLGameSetup::CreateGraphics()
	{
		cGraphics *pGraphics = new cGraphics(mpLowLevelGraphics);
		return pGraphics;
	}
	//-----------------------------------------------------------------------

	cSound* cSDLGameSetup::CreateSound()
	{
		cSound *pSound = new cSound(mpLowLevelSound);
		return pSound;
	}

	//-----------------------------------------------------------------------

	cPhysics* cSDLGameSetup::CreatePhysics()
	{
		cPhysics *pPhysics = new cPhysics(mpLowLevelPhysics);
		return pPhysics;
	}

	//-----------------------------------------------------------------------

	cAI* cSDLGameSetup::CreateAI()
	{
		cAI *pAI = new cAI();
		return pAI;
	}

	//-----------------------------------------------------------------------

	cScript* cSDLGameSetup::CreateScript(cResources *apResources)
	{
		cScript *pScript = new cScript(apResources);
		return pScript;
	}

	//-----------------------------------------------------------------------

}
