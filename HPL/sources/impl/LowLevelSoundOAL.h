/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_LOWLEVELSOUND_OAL_H
#define HPL_LOWLEVELSOUND_OAL_H

#include "sound/LowLevelSound.h"

#include <AL/al.h>
#include <AL/alc.h>

namespace hpl
{

	class cLowLevelSoundOAL : public iLowLevelSound
	{
	public:
		cLowLevelSoundOAL();
		~cLowLevelSoundOAL();

		void GetSupportedFormats(tStringList &alstFormats) override;
		void GetOutputDeviceNames(tStringList &alstDevices) override;
		tString GetCurrentDeviceName() override;

		iSoundData* LoadSoundData(const tString& asName,const tString& asFilePath,
									const tString& asType, bool abStream,bool abLoopStream) override;

		void UpdateSound(float afTimeStep) override;

		void SetListenerAttributes (const cVector3f &avPos,const cVector3f &avVel,
								const cVector3f &avForward,const cVector3f &avUp) override;
		void SetListenerPosition(const cVector3f &avPos) override;

		void SetListenerAttenuation (bool abEnabled) override;

		void Init ( bool abUseHardware, bool abForceGeneric, bool abUseEnvAudio, int alMaxChannels,
					int alStreamUpdateFreq, bool abUseThreading, bool abUseVoiceManagement,
					int alMaxMonoSourceHint, int alMaxStereoSourceHint,
					int alStreamingBufferSize, int alStreamingBufferCount, bool abEnableLowLevelLog,
					tString asDeviceName
					) override;

		void SetVolume(float afVolume) override;

		void SetEnvVolume( float afEnvVolume ) override;

		iSoundEnvironment* LoadSoundEnvironment (const tString& asFilePath) override;
		void SetSoundEnvironment ( iSoundEnvironment* apSoundEnv ) override;
		void FadeSoundEnvironment( iSoundEnvironment* apSourceSoundEnv, iSoundEnvironment* apDestSoundEnv, float afT ) override;

	private:
		ALCdevice *miDevice = nullptr;
		ALCcontext *miContext = nullptr;
	};

};
#endif // HPL_LOWLEVELSOUND_OAL_H
