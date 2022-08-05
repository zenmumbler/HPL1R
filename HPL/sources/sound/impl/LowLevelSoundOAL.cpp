/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */

#include "system/Log.h"
#include "sound/impl/LowLevelSoundOAL.h"
#include "sound/impl/OALSoundData.h"
#include "sound/impl/OALSoundChannel.h"
#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelSoundOAL::cLowLevelSoundOAL()
	{
		mbEnvAudioEnabled = false;
		miDevice = 0;
		miContext = 0;
	}

	//-----------------------------------------------------------------------

	cLowLevelSoundOAL::~cLowLevelSoundOAL()
	{
		if (miContext != nullptr) {
			alcDestroyContext(miContext);
		}
		if (miDevice != nullptr) {
			alcCloseDevice(miDevice);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iSoundData* cLowLevelSoundOAL::LoadSoundData(const tString& asName, const tString& asFilePath,
												const tString& asType, bool abStream,bool abLoopStream)
	{
		cOALSoundData* pSoundData = new cOALSoundData(asName,abStream);

		pSoundData->SetLoopStream(abLoopStream);

		if(pSoundData->CreateFromFile(asFilePath)==false)
		{
			delete pSoundData;
			return NULL;
		}

		return pSoundData;
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::GetSupportedFormats(tStringList &alstFormats)
	{
		alstFormats.push_back("WAV");
		alstFormats.push_back("OGG");
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::GetOutputDeviceNames(tStringList &alstDevices)
	{
		const char *devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

		size_t len;
		while ((len = strlen(devices))) {
			alstDevices.push_back(devices);
			devices += len + 1;
		}
	}

	//-----------------------------------------------------------------------

	tString cLowLevelSoundOAL::GetCurrentDeviceName() {
		if (miDevice == nullptr) {
			return "";
		}
		return alcGetString(miDevice, ALC_DEVICE_SPECIFIER);
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::UpdateSound(float afTimeStep)
	{
		cOALSoundChannel::UpdateAll();
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::SetListenerAttributes(const cVector3f &avPos,const cVector3f &avVel,
							const cVector3f &avForward,const cVector3f &avUp)
	{
		mvListenerPosition = avPos;
		mvListenerVelocity = avVel;
		mvListenerForward = avForward;
		mvListenerUp = avUp;
		mvListenerRight = cMath::Vector3Cross(mvListenerForward,mvListenerUp);

		m_mtxListener = cMatrixf::Identity;
		m_mtxListener.SetRight(mvListenerRight);
		m_mtxListener.SetUp(mvListenerUp);
		m_mtxListener.SetForward(mvListenerForward*-1);
		m_mtxListener = cMath::MatrixInverse(m_mtxListener);
		m_mtxListener.SetTranslation(mvListenerPosition);

		alListenerfv(AL_POSITION, avPos.v);
		alListenerfv(AL_VELOCITY, avVel.v);
		auto fwd = avForward * -1;
		float orientation[6] = { fwd.x, fwd.y, fwd.z, avUp.x, avUp.y, avUp.z };
		alListenerfv(AL_ORIENTATION, orientation);
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::SetListenerPosition(const cVector3f &avPos)
	{
		mvListenerPosition = avPos;
		alListenerfv(AL_POSITION, avPos.v);
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::SetListenerAttenuation (bool abEnabled)
	{
		mbListenerAttenuation = abEnabled;
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::SetVolume(float afVolume)
	{
		mfVolume = afVolume;
		alListenerf(AL_GAIN, afVolume);
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::Init(const tString &asDeviceName)
	{
		Log(" Initializing Direct OpenAL\n");
		
		miDevice = alcOpenDevice(nullptr);
		if (miDevice != nullptr) {
			ALCint attrs[] = { ALC_FREQUENCY, 44100, 0 };
			miContext = alcCreateContext(miDevice, attrs);
			alcMakeContextCurrent(miContext);
		}

		if (miContext == nullptr)
		{
			FatalError("Failed! Check your OpenAL installation\n");
			return;
		}

		alDistanceModel(AL_NONE);

		Log("  Device name: %s\n", alcGetString(miDevice, ALC_DEVICE_SPECIFIER));
		// Log("  Number of mono sources: %d\n", OAL_Info_GetNumSources());
		Log("  Vendor name: %s\n", alGetString(AL_VENDOR));
		Log("  Renderer: %s\n", alGetString(AL_RENDERER));
		Log("  OpenAL Version Supported: %s\n", alGetString(AL_VERSION));

		SetListenerAttributes({0,0,0}, {0,0,0}, {0,0,1}, {0,1,0});
		SetVolume(1.0f);
	}

	//-----------------------------------------------------------------------


	void cLowLevelSoundOAL::SetEnvVolume(float afEnvVolume)
	{
	}

	//-----------------------------------------------------------------------

	iSoundEnvironment* cLowLevelSoundOAL::LoadSoundEnvironment(const tString &asFilePath)
	{
		return nullptr;

	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::SetSoundEnvironment ( iSoundEnvironment* apSoundEnv )
	{
	}

	//-----------------------------------------------------------------------

	void cLowLevelSoundOAL::FadeSoundEnvironment( iSoundEnvironment* apSourceSoundEnv, iSoundEnvironment* apDestSoundEnv, float afT )
	{
	}

}
