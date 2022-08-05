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
#include "sound/Sound.h"
#include "sound/LowLevelSound.h"
#include "resources/Resources.h"
#include "sound/SoundHandler.h"
#include "sound/MusicHandler.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSound::cSound(iLowLevelSound *apLowLevelSound) : iUpdateable("HPL_Sound")
	{
		mpLowLevelSound = apLowLevelSound;
	}

	//-----------------------------------------------------------------------

	cSound::~cSound()
	{
		Log("Exiting Sound Module\n");
		Log("--------------------------------------------------------\n");

		delete mpSoundHandler;
		delete mpMusicHandler;

		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSound::Update(float afTimeStep)
	{
		mpSoundHandler->Update(afTimeStep);
		mpMusicHandler->Update(afTimeStep);

		mpLowLevelSound->UpdateSound(afTimeStep);
	}

	//-----------------------------------------------------------------------

	void cSound::Init(cResources *apResources, const tString &asDeviceName)
	{
		mpResources = apResources;

		Log("Initializing Sound Module\n");
		Log("--------------------------------------------------------\n");

		mpLowLevelSound->Init(asDeviceName);

		mpSoundHandler = new cSoundHandler(mpLowLevelSound, mpResources);
		mpMusicHandler = new cMusicHandler(mpLowLevelSound, mpResources);

		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

}
