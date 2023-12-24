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
#include "resources/SoundManager.h"
#include "resources/FileSearcher.h"
#include "sound/LowLevelSound.h"
#include "sound/SoundData.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSoundManager::cSoundManager(iLowLevelSound* soundDevice)
		: iResourceManager{"sound"}
	{
		_soundDevice = soundDevice;
		_soundDevice->GetSupportedFormats(_fileFormats);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cSoundManager::LoadAsset(const tString &name, const tString &fullPath) {
		return nullptr;
	}

	std::span<const tString> cSoundManager::SupportedExtensions() const {
		return { _fileFormats };
	}

	//-----------------------------------------------------------------------

	iSoundData* cSoundManager::CreateSoundData(const tString &name, bool stream)
	{
		tString fullPath;
		auto qualifiedName = FileSearcher::ResolveAssetName(name, SupportedExtensions());
		auto sound = static_cast<iSoundData*>(FindLoadedResource(qualifiedName, fullPath));
		if (! sound && fullPath.length() > 0) {
			sound = _soundDevice->CreateSoundData(name);
			if (sound->CreateFromFile(fullPath, stream) == false)
			{
				delete sound;
				return nullptr;
			}
			AddResource(sound);
		}

		if (sound) sound->IncUserCount();
		return sound;
	}

	//-----------------------------------------------------------------------

}
