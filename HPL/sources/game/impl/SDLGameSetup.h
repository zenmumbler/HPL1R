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

namespace hpl {

	class cSDLGameSetup : public iLowLevelGameSetup
	{
	public:
		cSDLGameSetup();
		~cSDLGameSetup();

		iLowLevelGraphics* GetGraphicsDevice() const override { return _graphicsDevice; }
		iLowLevelInput* GetInputImpl() const override { return _inputImpl; }
		iLowLevelSound* GetSoundImpl() const override { return _soundImpl; }
		iLowLevelPhysics* GetPhysicsImpl() const override { return _physicsImpl; }

	private:
		iLowLevelGraphics *_graphicsDevice;
		iLowLevelInput *_inputImpl;
		iLowLevelSound *_soundImpl;
		iLowLevelPhysics *_physicsImpl;
	};
};

#endif // HPL_LOWLEVELGAMESETUP_SDL_H
