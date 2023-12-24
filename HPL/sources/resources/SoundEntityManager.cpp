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
#include "resources/SoundEntityManager.h"
#include "sound/Sound.h"
#include "sound/SoundEntityData.h"
#include "sound/SoundHandler.h"
#include "sound/SoundChannel.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSoundEntityManager::cSoundEntityManager(cSound *apSound)
		: iResourceManager{"sound_entity"}
	{
		mpSound = apSound;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cSoundEntityManager::LoadAsset(const tString &name, const tString &fullPath) {
		auto snt = new cSoundEntityData(name);

		if (! snt->CreateFromFile(fullPath))
		{
			delete snt;
			return nullptr;
		}
		return snt;
	}

	static const tString s_Extensions[] { "snt" };

	std::span<const tString> cSoundEntityManager::SupportedExtensions() const {
		return { s_Extensions };
	}

	//-----------------------------------------------------------------------

	void cSoundEntityManager::Preload(const tString &name)
	{
		auto snt = CreateSoundEntity(name);
		if (! snt) {
			Warning("Couldn't preload sound '%s'\n", name.c_str());
			return;
		}

		if (snt->GetMainSoundName() != "") {
			iSoundChannel *channel = mpSound->GetSoundHandler()->CreateChannel(snt->GetMainSoundName(), 0);
			if(channel) delete channel;
		}
		if (snt->GetStartSoundName() != "") {
			auto channel = mpSound->GetSoundHandler()->CreateChannel(snt->GetStartSoundName(), 0);
			if (channel) delete channel;
		}
		if (snt->GetStopSoundName() != "") {
			iSoundChannel *channel = mpSound->GetSoundHandler()->CreateChannel(snt->GetStopSoundName(), 0);
			if (channel) delete channel;
		}
	}

	//-----------------------------------------------------------------------

	cSoundEntityData* cSoundEntityManager::CreateSoundEntity(const tString &name)
	{
		return static_cast<cSoundEntityData*>(GetOrLoadResource(name));
	}

	//-----------------------------------------------------------------------
}
