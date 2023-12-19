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
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
			FatalError("Error Initializing SDL: %s\n", SDL_GetError());
		}

		_graphicsDevice = new cLowLevelGraphicsSDL();
		_inputImpl = new cLowLevelInputSDL(_graphicsDevice);
		_soundImpl	= new cLowLevelSoundOAL();
		_physicsImpl = new cLowLevelPhysicsNewton();
	}

	//-----------------------------------------------------------------------

	cSDLGameSetup::~cSDLGameSetup()
	{
		Log("- Deleting lowlevel stuff.\n");

		Log("  Physics\n");
		delete _physicsImpl;
		Log("  Sound\n");
		delete _soundImpl;
		Log("  Input\n");
		delete _inputImpl;
		Log("  Graphics\n");
		delete _graphicsDevice;

		SDL_Quit();
	}

	//-----------------------------------------------------------------------

}
