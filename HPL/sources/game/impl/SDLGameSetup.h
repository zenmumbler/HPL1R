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
#ifndef HPL_SDL_GAMESETUP_H
#define HPL_SDL_GAMESETUP_H

#include "game/LowLevelGameSetup.h"
#include "graphics/LowLevelGraphics.h"
#include "input/LowLevelInput.h"
#include "physics/LowLevelPhysics.h"
#include "sound/LowLevelSound.h"
#include "script/Script.h"

namespace hpl {

	class cSDLGameSetup : public iLowLevelGameSetup
	{
	public:
		cSDLGameSetup();
		~cSDLGameSetup();

		cGraphics* CreateGraphics();
		cInput* CreateInput();
		cResources* CreateResources();
		cSound* CreateSound();
		cPhysics* CreatePhysics();
		cAI* CreateAI();
		cScript* CreateScript(cResources *apResources);

	private:
		iLowLevelGraphics *mpLowLevelGraphics;
		iLowLevelInput *mpLowLevelInput;
		iLowLevelSound*	mpLowLevelSound;
		iLowLevelPhysics* mpLowLevelPhysics;
	};
};
#endif // HPL_LOWLEVELGAMESETUP_SDL_H
