/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_OAL_SOUND_CHANNEL_H
#define HPL_OAL_SOUND_CHANNEL_H

#include "sound/SoundChannel.h"
#include "sound/SoundData.h"

namespace hpl {

	class cOALSoundChannel : public iSoundChannel
	{
	public:
		cOALSoundChannel(iSoundData* apData, cSoundManager* apSoundManger);
		~cOALSoundChannel();

		void Play();
		void Stop();

		void SetPaused(bool abX);
		void SetSpeed(float afSpeed);
		void SetVolume (float afVolume);
		void SetLooping (bool abLoop);
		void SetPan (float afPan);
		void Set3D(bool ab3D);

		void SetPriority(int alX);
		int GetPriority();

		void SetPositionRelative(bool abRelative);
		void SetPosition(const cVector3f &avPos);
		void SetVelocity(const cVector3f &avVel);

		void SetMinDistance(float fMin);
		void SetMaxDistance(float fMax);

		bool IsPlaying();

		bool IsBufferUnderrun() { return false; }
		double GetElapsedTime()	{ return 0.0; }
		double GetTotalTime()	{ return 0.0; }

		int FeedNextBuffer(unsigned int buf);
		void Update();
		
		static void UpdateAll();

	private:
		unsigned int mBuffers[2] = { 0 };
		unsigned int mSource;
		short *mOutput;
	};
};
#endif // HPL_OAL_SOUND_CHANNEL_H
