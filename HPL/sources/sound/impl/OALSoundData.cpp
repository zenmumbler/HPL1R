/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#include "sound/impl/OALSoundData.h"
#include "sound/impl/OALSoundChannel.h"
#include "sound/impl/WAVFile.h"
#include "stb/stb_vorbis.h"
#include "system/String.h"
#include "system/Log.h"

#include <AL/al.h>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cOALSoundData::cOALSoundData(const tString& name) : iSoundData(name)
	{
	}

	//-----------------------------------------------------------------------

	cOALSoundData::~cOALSoundData()
	{
		if (_vorbis) {
			stb_vorbis_close(_vorbis);
		}
		if (_sampleData) {
			// use free because of stb_vorbis_decode_file usage
			free(_sampleData);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cOALSoundData::CreateFromFile(const tString &fullPath, bool streaming)
	{
		auto ext = cString::GetFileExt(fullPath);
		if (ext == "wav") {
			int samples = LoadWAVFile(fullPath, &_channels, &_format, &_rate, &_sampleData);
			if (samples == 0) {
				Error("Cannot load WAV file '%s'\n", fullPath.c_str());
				return false;
			}
			_byteSize = samples * sizeof(short);
		}
		else if (ext == "ogg") {
			int error;

			if (streaming) {
				_vorbis = stb_vorbis_open_filename(fullPath.c_str(), &error, nullptr);
				if (_vorbis == nullptr) {
					Error("Cannot open Ogg file '%s' for streaming, error code: %d\n", fullPath.c_str(), error);
					return false;
				}
				
				auto info = stb_vorbis_get_info(_vorbis);
				_rate = info.sample_rate;
				_channels = info.channels;
				_byteSize = 0;
			}
			else {
				int samples = stb_vorbis_decode_filename(fullPath.c_str(), &_channels, &_rate, &_sampleData);
				if (samples == 0) {
					Error("Cannot load Ogg file '%s'\n", fullPath.c_str());
					return false;
				}
				_byteSize = samples * sizeof(short);
			}
			_format = _channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		}
		else {
			Error("Unknown file extension '%s'\n", fullPath.c_str());
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	iSoundChannel* cOALSoundData::CreateChannel(int alPriority)
	{
		if ((_vorbis == nullptr) && (_sampleData == nullptr)) {
			return nullptr;
		}

		iSoundChannel *pSoundChannel = new cOALSoundChannel(this, mpSoundManger);
		IncUserCount();

		return pSoundChannel;
	}

	//-----------------------------------------------------------------------

	bool cOALSoundData::IsStereo()
	{
		return _channels == 2;
	}

	//-----------------------------------------------------------------------

}
