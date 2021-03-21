/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#include <vector>
#include <algorithm>
#include <AL/al.h>

#include "system/LowLevelSystem.h"
#include "resources/SoundManager.h"
#include "math/Math.h"

#include "impl/OALSoundChannel.h"
#include "impl/OALSoundData.h"
#include "impl/stb_vorbis.h"

static const int STREAM_BUF_SAMPLES = 22050;

namespace hpl {

	static std::vector<cOALSoundChannel*> s_AllChannels {};

	void cOALSoundChannel::UpdateAll() {
		for (const auto channel : s_AllChannels) {
			channel->Update();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cOALSoundChannel::cOALSoundChannel(iSoundData* apData,cSoundManager* apSoundManger)
	: iSoundChannel(apData, apSoundManger)
	{
		alGenSources(1, &mSource);
		
		const auto sound = dynamic_cast<cOALSoundData*>(apData);
		if (sound->mVorbis) {
			alGenBuffers(2, mBuffers);
			mOutput = new short[STREAM_BUF_SAMPLES * sound->mChannels];

			FeedNextBuffer(mBuffers[0]);
			FeedNextBuffer(mBuffers[1]);
		}
		else {
			alGenBuffers(1, mBuffers);
			mOutput = nullptr;
			alBufferData(mBuffers[0], sound->mFormat, sound->mSampleData, sound->mByteSize, sound->mRate);
			alSourcei(mSource, AL_BUFFER, mBuffers[0]);
		}
		alSourcePause(mSource);
		
		s_AllChannels.push_back(this);
	}

	//-----------------------------------------------------------------------

	cOALSoundChannel::~cOALSoundChannel()
	{
		alSourceStop(mSource);
		alSourcei(mSource, AL_BUFFER, 0);

		alDeleteSources(1, &mSource);
		alDeleteBuffers(mOutput ? 2 : 1, mBuffers);

		if (mOutput) {
			delete[] mOutput;
		}
		
		s_AllChannels.erase(std::remove(std::begin(s_AllChannels), std::end(s_AllChannels), this));

		// [ZM] this is the counterpart to the IncUserCount call in SoundData::CreateChannel
		if (mpSoundManger) {
			mpSoundManger->Destroy(mpData);
		}
	}

	//-----------------------------------------------------------------------

	int cOALSoundChannel::FeedNextBuffer(unsigned int buf) {
		const auto sound = dynamic_cast<cOALSoundData*>(mpData);
		int samples = stb_vorbis_get_samples_short_interleaved(sound->mVorbis, sound->mChannels, mOutput, STREAM_BUF_SAMPLES * sound->mChannels);
		if (samples < STREAM_BUF_SAMPLES && sound->GetLoopStream()) {
			int remainder = STREAM_BUF_SAMPLES - samples;

			stb_vorbis_seek_start(sound->mVorbis);
			samples += stb_vorbis_get_samples_short_interleaved(sound->mVorbis, sound->mChannels, mOutput + (samples * sound->mChannels), remainder * sound->mChannels);
		}
		if (samples > 0) {
			alBufferData(buf, sound->mFormat, mOutput, samples * sizeof(short) * sound->mChannels, sound->mRate);
			alSourceQueueBuffers(mSource, 1, &buf);
		}
		return samples;
	}

	void cOALSoundChannel::Update() {
		const auto sound = dynamic_cast<cOALSoundData*>(mpData);
		if (! (IsPlaying() && sound->IsStream())) {
			return;
		}

		int buffersProcessed = 0;
		alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &buffersProcessed);
		while (buffersProcessed--) {
			ALuint cycleBuf;
			alSourceUnqueueBuffers(mSource, 1, &cycleBuf);
			int filledLen = FeedNextBuffer(cycleBuf);
			// Log("Stream buf #%d was recycles with %d samples.\n", cycleBuf, filledLen);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cOALSoundChannel::Play()
	{
		SetPaused(false);
		mbStopUsed = false;
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::Stop()
	{
		alSourceStop(mSource);
		mbStopUsed = true;
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetPaused(bool abX)
	{
		if (mbStopUsed) {
			return;
		}
		mbPaused = abX;
		if (abX) {
			alSourcePause(mSource);
		}
		else {
			alSourcePlay(mSource);
		}
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetSpeed(float afSpeed)
	{
		mfSpeed = afSpeed;

//		OAL_Source_SetPitch ( mlChannel, afSpeed );
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetVolume(float afVolume)
	{
		mfVolume = afVolume;
		alSourcef(mSource, AL_GAIN, afVolume);
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetLooping(bool abLoop)
	{
		mbLooping = abLoop;
		alSourcei(mSource, AL_LOOPING, abLoop ? 1 : 0);
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetPan(float afPan)
	{
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::Set3D(bool ab3D)
	{
		mb3D = ab3D;
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetPositionRelative(bool abRelative)
	{
		mbPositionRelative = abRelative;
//		OAL_Source_SetPositionRelative ( mlChannel, abRelative );
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetPosition(const cVector3f &avPos)
	{
		mvPosition = avPos;
		alSourcefv(mSource, AL_POSITION, avPos.v);
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetVelocity(const cVector3f &avVel)
	{
		mvVelocity = avVel;
		alSourcefv(mSource, AL_VELOCITY, avVel.v);
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetMinDistance(float afMin)
	{
		mfMinDistance = afMin;
	}

	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetMaxDistance(float afMax)
	{
		mfMaxDistance = afMax;
	}
	//-----------------------------------------------------------------------

	bool cOALSoundChannel::IsPlaying()
	{
		int state;
		alGetSourcei(mSource, AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING) return true;
		
		const auto sound = dynamic_cast<cOALSoundData*>(mpData);
		if (sound->IsStream()) {
			// handle a case where the queued buffers have been processed but
			// this check is performed before the next set of buffers could be
			// queued. This will happen especially during setup/loading times.
			int bufferCount;
			alGetSourcei(mSource, AL_BUFFERS_QUEUED, &bufferCount);
			if (bufferCount > 0) {
				return true;
			}
		}

		return false;
	}
	//-----------------------------------------------------------------------

	void cOALSoundChannel::SetPriority(int alX)
	{
		int lPrio = alX+mlPriorityModifier;
		if(lPrio>255)lPrio = 255;

		// OAL_Source_SetPriority ( mlChannel, lPrio );
	}

	//-----------------------------------------------------------------------

	int cOALSoundChannel::GetPriority()
	{
		return mlPriorityModifier;
	}

}
