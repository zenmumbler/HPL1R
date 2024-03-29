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
#ifndef HPL_SOUNDHANDLER_H
#define HPL_SOUNDHANDLER_H

#include "physics/PhysicsWorld.h"
#include "math/MathTypes.h"
#include "system/StringTypes.h"

#include <list>

namespace hpl {

	class iLowLevelSound;
	class iSoundChannel;
	class cWorld3D;

	//----------------------------------------

	enum eSoundDest // bitmask
	{
		eSoundDest_World = 1,
		eSoundDest_Gui = 2,
		eSoundDest_All = 3
	};

	//----------------------------------------

	class cSoundRayCallback :public iPhysicsRayCallback
	{
	public:
		void Reset();
		bool HasCollided(){ return mbHasCollided;}

		bool BeforeIntersect(iPhysicsBody *pBody);
		bool OnIntersect(iPhysicsBody *pBody,cPhysicsRayParams *apParams);

	private:
		bool mbHasCollided;
		int mlCount;
	};

	//----------------------------------------


	////////////////////////////////////////////////////
	//////////// SOUND ENTRY ///////////////////////////
	////////////////////////////////////////////////////

	class cSoundEntry
	{
	public:
		cSoundEntry() : mfNormalVolume(1), mfNormalVolumeFadeDest(1),
						mfNormalVolumeMul(1), mfNormalVolumeFadeSpeed(0), mbStream(false),
						mlCount(0){}

		void Update(float afTimeStep);

		tString msName;
		iSoundChannel* mpSound;

		float mfNormalVolume;
		float mfNormalVolumeMul;
		float mfNormalVolumeFadeDest;
		float mfNormalVolumeFadeSpeed;

		float mfNormalSpeed;

		bool mbFirstTime;

		float mfBlockMul;
		float mfBlockFadeDest;
		float mfBlockFadeSpeed;

		bool mbStream;

		long int mlCount;

		eSoundDest mEffectType;
	};

	typedef std::list<cSoundEntry> tSoundEntryList;
	typedef tSoundEntryList::iterator tSoundEntryListIt;

	typedef cSTLIterator<cSoundEntry,tSoundEntryList,tSoundEntryListIt> tSoundEntryIterator;


	////////////////////////////////////////////////////
	//////////// SOUND HANDLER ///////////////////////
	////////////////////////////////////////////////////

	//----------------------------------------

	typedef std::map<tString, int> tPlayedSoundNumMap;
	typedef tPlayedSoundNumMap::iterator tPlayedSoundNumMapIt;

	//----------------------------------------

	class cResources;

	//----------------------------------------

	class cSoundHandler
	{
	public:
		cSoundHandler(iLowLevelSound* apLowLevelSound, cResources* apResources);
		~cSoundHandler();

		iSoundChannel* Play(const tString& asName,bool abLoop,float afVolume,const cVector3f& avPos,
				float afMinDist,float afMaxDist, eSoundDest mType, bool abRelative, bool ab3D = false,
				int alPriorityModifier=0,eSoundDest aEffectType=eSoundDest_World);

		iSoundChannel* Play3D(const tString& asName,bool abLoop,float afVolume,const cVector3f& avPos,
			float afMinDist,float afMaxDist, eSoundDest mType, bool abRelative,int alPriorityModifier=0,
			eSoundDest aEffectType=eSoundDest_World)
		{
			return Play(asName,abLoop,afVolume,avPos,afMinDist,afMaxDist,mType,abRelative,true,
						alPriorityModifier, aEffectType);
		}

		iSoundChannel* PlayStream(const tString& asFileName,bool abLoop,float afVolume,bool ab3D = false,
									eSoundDest aEffectType=eSoundDest_Gui);

		iSoundChannel* PlayGui(const tString& asName,bool abLoop,float afVolume,const cVector3f& avPos=cVector3f(0,0,1),
								eSoundDest aEffectType=eSoundDest_Gui);

		void SetSilent(bool abX){ mbSilent = abX; }
		bool GetSilent(){ return mbSilent; }

		bool Stop(const tString& asName);
		bool StopAllExcept(const tString& asName);

		void StopAll(tFlag mTypes);
		void PauseAll(tFlag mTypes);
		void ResumeAll(tFlag mTypes);

		bool IsPlaying(const tString& asName);

		bool IsValid(iSoundChannel* apChannel);
		bool IsValidId(iSoundChannel* apChannel, int alId);

		void Update(float afTimeStep);

		void SetSpeed(float afSpeed,float afRate,tFlag mTypes);
		void SetVolume(float afVolume, float afRate, tFlag mTypes);

		float GetVolume(){ return mfVolume;}

		void SetWorld3D(cWorld3D *apWorld3D);

		cSoundEntry* GetEntryFromSound(iSoundChannel *apSound);

		iSoundChannel* CreateChannel(const tString& asName, int alPriority);

		tSoundEntryList* GetWorldEntryList();
		tSoundEntryList* GetGuiEntryList();
	private:
		iLowLevelSound* mpLowLevelSound;
		cResources* mpResources;

		tSoundEntryList mlstGuiSounds;
		tSoundEntryList mlstWorldSounds;

		bool mbSilent;

		float mfSpeed;
		float mfNewSpeed;
		float mfSpeedRate;
		tFlag mAffectedBySpeed;

		float mfVolume;
		float mfNewVolume;
		float mfVolumeRate;
		tFlag mAffectedByVolume;

		cWorld3D *mpWorld3D;

		cSoundRayCallback mSoundRayCallback;

		tPlayedSoundNumMap m_mapPlayedSound;

		cSoundEntry* GetEntry(const tString& asName);
		bool UpdateEntry(cSoundEntry* apEntry,float afTimeStep, tFlag aTypes);
		void UpdateDistanceVolume3D(cSoundEntry* apEntry,float afTimeStep,bool abFade,tFlag aTypes);

		int mlCount;
		int mlIdCount;
	};

};
#endif // HPL_SOUNDHANDLER_H
