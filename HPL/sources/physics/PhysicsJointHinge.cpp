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
#include "physics/PhysicsJointHinge.h"

#include "scene/World3D.h"
#include "scene/Scene.h"
#include "game/Game.h"

#include "physics/PhysicsBody.h"
#include "physics/PhysicsWorld.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	kBeginSerialize(cSaveData_iPhysicsJointHinge, cSaveData_iPhysicsJoint)
	kSerializeVar(mfMaxAngle, eSerializeType_Float32)
	kSerializeVar(mfMinAngle, eSerializeType_Float32)
	kEndSerialize()

	//-----------------------------------------------------------------------

	iSaveObject* cSaveData_iPhysicsJointHinge::CreateSaveObject(cSaveObjectHandler *apSaveObjectHandler,cGame *apGame)
	{
		iPhysicsWorld *apWorld = apGame->GetScene()->GetWorld3D()->GetPhysicsWorld();

		cMatrixf mtxChildTemp, mtxParentTemp;

		iPhysicsBody *pChildBody = static_cast<iPhysicsBody*>(apSaveObjectHandler->Get(mlChildBodyId));
		if(pChildBody==NULL) return NULL;

		iPhysicsBody *pParentBody = NULL;
		if(mlParentBodyId>0) pParentBody = static_cast<iPhysicsBody*>(apSaveObjectHandler->Get(mlParentBodyId));

		mtxChildTemp = pChildBody->GetLocalMatrix();
		if(pParentBody) mtxParentTemp = pParentBody->GetLocalMatrix();

		pChildBody->SetMatrix(m_mtxChildBodySetup);
		if(pParentBody) pParentBody->SetMatrix(m_mtxParentBodySetup);

		// Rehatched: to match the direction of the pinDir in HPL1 maps, it is negated here
		// as it will be negated again after loading
		cVector3f negPinDir{ -mvPinDir.x, -mvPinDir.y, -mvPinDir.z };
		iPhysicsJointHinge *pJoint = apWorld->CreateJointHinge(msName, mvStartPivotPoint, negPinDir, pParentBody, pChildBody);

		pChildBody->SetMatrix(mtxChildTemp);
		if(pParentBody) pParentBody->SetMatrix(mtxParentTemp);

		return pJoint;
	}

	//-----------------------------------------------------------------------

	int cSaveData_iPhysicsJointHinge::GetSaveCreatePrio()
	{
		return 1;
	}

	//-----------------------------------------------------------------------

	iSaveData* iPhysicsJointHinge::CreateSaveData()
	{
		return new cSaveData_iPhysicsJointHinge();
	}

	//-----------------------------------------------------------------------

	void iPhysicsJointHinge::SaveToSaveData(iSaveData *apSaveData)
	{
		kSaveData_SaveToBegin(iPhysicsJointHinge);

		kSaveData_SaveTo(mfMaxAngle);
		kSaveData_SaveTo(mfMinAngle);
	}

	//-----------------------------------------------------------------------

	void iPhysicsJointHinge::LoadFromSaveData(iSaveData *apSaveData)
	{
		kSaveData_LoadFromBegin(iPhysicsJointHinge);

		kSaveData_LoadFrom(mfMaxAngle);
		kSaveData_LoadFrom(mfMinAngle);
	}

	//-----------------------------------------------------------------------

	void iPhysicsJointHinge::SaveDataSetup(cSaveObjectHandler *apSaveObjectHandler, cGame *apGame)
	{
		kSaveData_SetupBegin(iPhysicsJointHinge);
	}

	//-----------------------------------------------------------------------
}
