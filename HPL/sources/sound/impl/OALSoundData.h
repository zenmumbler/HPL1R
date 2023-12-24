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
		cOALSoundData(const tString& name);
		~cOALSoundData();

		bool CreateFromFile(const tString &fullPath, bool streaming) override;
		iSoundChannel* CreateChannel(int priority) override;
		bool IsStereo() override;
		bool IsStream() override { return _vorbis != nullptr; };

		stb_vorbis *_vorbis = nullptr;
		short *_sampleData = nullptr;
		int _rate = 0, _channels = 0, _byteSize = 0, _format = 0;
	};
};

#endif // HPL_OAL_SOUND_DATA_H
