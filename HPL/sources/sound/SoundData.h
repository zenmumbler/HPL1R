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
#ifndef HPL_SOUND_DATA_H
#define HPL_SOUND_DATA_H

#include "resources/ResourceBase.h"
#include "system/StringTypes.h"

namespace hpl {

	class cSoundManager;
	class iSoundChannel;

	class iSoundData : public iResourceBase
	{
	public:
		iSoundData(const tString& name) : iResourceBase{name}, _loopStream{false}, mpSoundManger{nullptr} {}
		virtual ~iSoundData() = default;

		virtual bool CreateFromFile(const tString &fullPath, bool streaming)=0;
		virtual iSoundChannel* CreateChannel(int alPriority)=0;

		virtual bool IsStereo()=0;
		virtual bool IsStream()=0;
		void SetLoopStream(bool abX){ _loopStream = abX; }
		bool GetLoopStream(){ return _loopStream; }

		void SetSoundManager(cSoundManager* apSoundManager){
			mpSoundManger = apSoundManager;
		}

	protected:
		bool _loopStream;
		cSoundManager* mpSoundManger;
	};
};
#endif // HPL_SOUND_DATA_H
