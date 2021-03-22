/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#include <AL/al.h>
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "impl/OALSoundData.h"
#include "impl/OALSoundChannel.h"
#include "impl/stb_vorbis.h"
#include "impl/WAVFile.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cOALSoundData::cOALSoundData(tString asName, bool abStream) : iSoundData(asName,abStream)
	{
	}

	//-----------------------------------------------------------------------

	cOALSoundData::~cOALSoundData()
	{
		if (mVorbis) {
			stb_vorbis_close(mVorbis);
		}
		if (mSampleData) {
			// use free because of stb_vorbis_decode_file usage
			free(mSampleData);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cOALSoundData::CreateFromFile(const tString &asFile)
	{
		auto ext = cString::GetFileExt(asFile);
		if (ext == "wav") {
			int samples = LoadWAVFile(asFile, &mChannels, &mFormat, &mRate, &mSampleData);
			if (samples == 0) {
				Error("Cannot load WAV file '%s'\n", asFile.c_str());
				return false;
			}
			mByteSize = samples * sizeof(short);

			mbStream = false;
		}
		else if (ext == "ogg") {
			int error;

			if (mbStream) {
				mVorbis = stb_vorbis_open_filename(asFile.c_str(), &error, nullptr);
				if (mVorbis == nullptr) {
					Error("Cannot open Ogg file '%s' for streaming, error code: %d\n", asFile.c_str(), error);
					return false;
				}
				
				auto info = stb_vorbis_get_info(mVorbis);
				mRate = info.sample_rate;
				mChannels = info.channels;
				mByteSize = 0;
			}
			else {
				int samples = stb_vorbis_decode_filename(asFile.c_str(), &mChannels, &mRate, &mSampleData);
				if (samples == 0) {
					Error("Cannot load Ogg file '%s'\n", asFile.c_str());
					return false;
				}
				mByteSize = samples * sizeof(short);
			}
			mFormat = mChannels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		}
		else {
			Error("Unknown file extension '%s'\n", asFile.c_str());
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	iSoundChannel* cOALSoundData::CreateChannel(int alPriority)
	{
		if ((mVorbis == nullptr) && (mSampleData == nullptr)) {
			return nullptr;
		}

		iSoundChannel *pSoundChannel = hplNew(cOALSoundChannel, (this, mpSoundManger));
		IncUserCount();

		return pSoundChannel;
	}

	//-----------------------------------------------------------------------

	bool cOALSoundData::IsStereo()
	{
		return mChannels == 2;
	}

	//-----------------------------------------------------------------------

}
