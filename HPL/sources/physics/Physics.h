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
#ifndef HPL_PHYSICS_H
#define HPL_PHYSICS_H

#include "game/Updateable.h"
#include "physics/PhysicsMaterial.h"
#include "physics/SurfaceData.h"
#include "system/StringTypes.h"

#include <list>
#include <map>

namespace hpl {

	class iLowLevelPhysics;
	class iPhysicsWorld;
	class cSurfaceData;
	class cWorld3D;
	class cResources;

	//------------------------------------------------

	typedef std::list<iPhysicsWorld*> tPhysicsWorldList;
	typedef tPhysicsWorldList::iterator tPhysicsWorldListIt;

	typedef std::map<tString, cSurfaceData*> tSurfaceDataMap;
	typedef tSurfaceDataMap::iterator tSurfaceDataMapIt;

	//------------------------------------------------

	class cPhysicsImpactCount
	{
	public:
		cPhysicsImpactCount(){ mfCount =0;}

		float mfCount;
	};

	typedef std::list<cPhysicsImpactCount> tPhysicsImpactCountList;
	typedef tPhysicsImpactCountList::iterator tPhysicsImpactCountListIt;

	//------------------------------------------------

	class cPhysics : public iUpdateable
	{
	public:
		cPhysics(iLowLevelPhysics *apLowLevelPhysics);
		~cPhysics();

		void Init(cResources *apResources);

		void Update(float afTimeStep);

		iPhysicsWorld* CreateWorld(bool abAddSurfaceData);
		void DestroyWorld(iPhysicsWorld* apWorld);

		cSurfaceData *CreateSurfaceData(const tString& asName);
		cSurfaceData *GetSurfaceData(const tString& asName);
		bool LoadSurfaceData(const tString& asFile);

		iLowLevelPhysics* GetLowLevel(){ return mpLowLevelPhysics;}

		void SetGameWorld(cWorld3D *apWorld){ mpGameWorld = apWorld;}
		cWorld3D* GetGameWorld(){ return mpGameWorld;}

		void SetImpactDuration(float afX){ mfImpactDuration = afX;}
		float GetImpactDuration(){ return mfImpactDuration;}

		void SetMaxImpacts(int alX){ mlMaxImpacts = alX;}
		int GetMaxImpacts(){ return mlMaxImpacts;}
		int GetNumOfImpacts(){ return (int)mlstImpactCounts.size();}

		bool CanPlayImpact();
		void AddImpact();

		void SetDebugLog(bool abX){ mbLog = abX;}
		bool GetDebugLog(){ return mbLog;}

	private:
		ePhysicsMaterialCombMode GetCombMode(const char *apName);

		void UpdateImpactCounts(float afTimeStep);

		iLowLevelPhysics *mpLowLevelPhysics;
		cResources *mpResources;

		cWorld3D *mpGameWorld;

		tPhysicsWorldList mlstWorlds;
		tSurfaceDataMap m_mapSurfaceData;

		tPhysicsImpactCountList mlstImpactCounts;
		float mfImpactDuration;
		int mlMaxImpacts;
		bool mbLog;
	};

};
#endif // HPL_Physics_H
