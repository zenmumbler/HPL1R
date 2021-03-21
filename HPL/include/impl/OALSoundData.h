/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_OAL_SOUND_DATA_H
#define HPL_OAL_SOUND_DATA_H

#include "sound/SoundData.h"

struct stb_vorbis;

namespace hpl {

	class cOALSoundData : public iSoundData
	{
	public:
		cOALSoundData(tString asName, bool abStream);
		~cOALSoundData();

		bool CreateFromFile(const tString &asFile);
		iSoundChannel* CreateChannel(int alPriority);
		bool IsStereo();
		
		stb_vorbis *mVorbis = nullptr;
		short *mSampleData = nullptr;
		int mRate = 0, mChannels = 0, mByteSize = 0, mFormat = 0;
	};
};

#endif // HPL_OAL_SOUND_DATA_H
