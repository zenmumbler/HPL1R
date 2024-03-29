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

#include "graphics/ParticleSystem3D.h"
#include "graphics/ParticleEmitter3D_UserData.h"
#include "resources/ParticleManager.h"
#include "game/Game.h"
#include "scene/Scene.h"
#include "scene/World3D.h"
#include "system/Log.h"
#include "system/STLHelpers.h"

#include "tinyXML/tinyxml.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CREATOR
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cParticleSystemData3D::cParticleSystemData3D(const tString &asName, iLowLevelGraphics *llGfx, cMaterialManager *materialMgr)
		: iResourceBase(asName)
	{
		_llGfx = llGfx;
		_materialMgr = materialMgr;
	}

	//-----------------------------------------------------------------------

	cParticleSystemData3D::~cParticleSystemData3D()
	{
		STLDeleteAll(mvEmitterData);
	}

	//-----------------------------------------------------------------------

	cParticleSystem3D* cParticleSystemData3D::Create(tString asName, cVector3f avSize, const cMatrixf& a_mtxTransform)
	{
		if(mvEmitterData.empty())
		{
			Warning("Particle system '%s' has no emitters.\n",msName.c_str());
			return NULL;
		}

		cParticleSystem3D *pPS = new cParticleSystem3D(asName, this);
		pPS->SetMatrix(a_mtxTransform);

		for(size_t i=0; i<mvEmitterData.size(); ++i)
		{
			///////////////////////////
			// Create and add
			iParticleEmitter3D *pPE = static_cast<iParticleEmitter3D*>(mvEmitterData[i]->Create(asName, avSize));
			pPS->AddEmitter(pPE);
			pPE->SetSystem(pPS);
		}

		return pPS;
	}

	//-----------------------------------------------------------------------

	bool cParticleSystemData3D::LoadFromFile(const tString &asFile)
	{
		TiXmlDocument* pXmlDoc = new TiXmlDocument(asFile.c_str());
		if(pXmlDoc->LoadFile()==false)
		{
			Warning("Couldn't open XML file %s\n",asFile.c_str());
			delete pXmlDoc;
			return false;
		}

		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		TiXmlElement *pEmitterElem = pRootElem->FirstChildElement("ParticleEmitter");
		for(; pEmitterElem != NULL; pEmitterElem = pEmitterElem->NextSiblingElement("ParticleEmitter"))
		{
			cParticleEmitterData3D_UserData *pPE = new cParticleEmitterData3D_UserData("", _llGfx, _materialMgr);

			pPE->LoadFromElement(pEmitterElem);

			mvEmitterData.push_back(pPE);
		}


		delete pXmlDoc;
		return true;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cParticleSystem3D::cParticleSystem3D(const tString asName, cParticleSystemData3D *apData)
		: iEntity3D(asName)
	{
		mpParticleManager = NULL;
		mpData = apData;

		mbFirstUpdate = true;
	}

	//-----------------------------------------------------------------------

	cParticleSystem3D::~cParticleSystem3D()
	{
		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			delete mvEmitters[i];
		}
		if(mpParticleManager) mpParticleManager->Destroy(mpData);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cParticleSystem3D::SetVisible(bool abVisible)
	{
		if(mbIsVisible == abVisible) return;

		mbIsVisible = abVisible;

		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			mvEmitters[i]->SetVisible(mbIsVisible);
		}

	}

	//-----------------------------------------------------------------------

	bool cParticleSystem3D::IsDead()
	{
		size_t lCount =0;

		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter3D *pPE = mvEmitters[i];

			if(pPE->IsDead()) lCount++;
		}

		if(lCount == mvEmitters.size()) return true;

		return false;
	}

	bool cParticleSystem3D::IsDying()
	{
		size_t lCount =0;

		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter3D *pPE = mvEmitters[i];

			if(pPE->IsDying()) lCount++;
		}

		if(lCount == mvEmitters.size()) return true;

		return false;
	}

	//-----------------------------------------------------------------------

	void cParticleSystem3D::Kill()
	{
		SetIsSaved(false);
		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter3D *pPE = mvEmitters[i];

			pPE->Kill();
		}
	}

	//-----------------------------------------------------------------------

	void cParticleSystem3D::KillInstantly()
	{
		SetIsSaved(false);
		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter3D *pPE = mvEmitters[i];

			pPE->KillInstantly();
		}
	}

	//-----------------------------------------------------------------------

	void cParticleSystem3D::UpdateLogic(float afTimeStep)
	{
		if(IsActive()==false) return;

		for(size_t i=0; i< mvEmitters.size(); ++i)
		{
			iParticleEmitter3D *pPE = mvEmitters[i];

			//////////////////////////
			//Warm Up
			if(mbFirstUpdate)
			{
				iParticleEmitterData *pData =  mpData->GetEmitterData((int)i);

				float fTime = pData->GetWarmUpTime();
				if (fTime > 0)
				{
					float fStepSize = 1.0f / pData->GetWarmUpStepsPerSec();

					while (fTime > 0)
					{
						pPE->UpdateLogic(fStepSize);
						fTime -= fStepSize;
					}
				}
				mbFirstUpdate = false;
			}

			//////////////////////////
			//Update
			pPE->UpdateLogic(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	void cParticleSystem3D::AddEmitter(iParticleEmitter3D* apEmitter)
	{
		mvEmitters.push_back(apEmitter);

		AddChild(apEmitter);
	}

	//-----------------------------------------------------------------------

	iParticleEmitter3D* cParticleSystem3D::GetEmitter(int alIdx)
	{
		return mvEmitters[alIdx];
	}

	//-----------------------------------------------------------------------

	int cParticleSystem3D::GetEmitterNum()
	{
		return (int)mvEmitters.size();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////////////////

	kBeginSerializeBase(cSaveData_ParticleEmitter3D)
		kSerializeVar(mbActive, eSerializeType_Bool)
		kSerializeVar(mbDying, eSerializeType_Bool)
	kEndSerialize()

	//-----------------------------------------------------------------------

	kBeginSerialize(cSaveData_cParticleSystem3D,cSaveData_iEntity3D)
		kSerializeVar(msDataName, eSerializeType_String)
		kSerializeVar(mvDataSize, eSerializeType_Vector3f)

		kSerializeClassContainer(mvEmitters,cSaveData_ParticleEmitter3D,eSerializeType_Class)
	kEndSerialize()

	//-----------------------------------------------------------------------

	iSaveObject* cSaveData_cParticleSystem3D::CreateSaveObject(cSaveObjectHandler *apSaveObjectHandler,cGame *apGame)
	{
		cWorld3D *pWorld = apGame->GetScene()->GetWorld3D();

		cParticleSystem3D *pPS = pWorld->CreateParticleSystem(msName,msDataName,mvDataSize,
															m_mtxLocalTransform);

		return pPS;
	}

	//-----------------------------------------------------------------------

	int cSaveData_cParticleSystem3D::GetSaveCreatePrio()
	{
		return 3;
	}

	//-----------------------------------------------------------------------

	iSaveData* cParticleSystem3D::CreateSaveData()
	{
		return new cSaveData_cParticleSystem3D();
	}

	//-----------------------------------------------------------------------

	void cParticleSystem3D::SaveToSaveData(iSaveData *apSaveData)
	{
		kSaveData_SaveToBegin(cParticleSystem3D);

		kSaveData_SaveTo(msDataName);
		kSaveData_SaveTo(mvDataSize);

		pData->mvEmitters.Resize(GetEmitterNum());
		for(int i=0; i< GetEmitterNum(); ++i)
		{
			iParticleEmitter3D *pPE = GetEmitter(i);

			pData->mvEmitters[i].mbDying = pPE->IsDying();
			pData->mvEmitters[i].mbActive = pPE->IsActive();
		}
	}

	//-----------------------------------------------------------------------

	void cParticleSystem3D::LoadFromSaveData(iSaveData *apSaveData)
	{
		kSaveData_LoadFromBegin(cParticleSystem3D);

		kSaveData_LoadFrom(msDataName);
		kSaveData_LoadFrom(mvDataSize);

		if(GetEmitterNum() != static_cast<int>(pData->mvEmitters.Size()))
		{
			Error("Saved emitter number in %s/%d does not match loaded, killing system!\n",
																			GetName().c_str(),
																			GetSaveObjectId());

			for(int i=0; i<GetEmitterNum();++i)
			{
				iParticleEmitter3D *pPE = GetEmitter(i);
				pPE->KillInstantly();
			}
			return;
		}

		for(int i=0; i< GetEmitterNum(); ++i)
		{
			iParticleEmitter3D *pPE = GetEmitter(i);

			pPE->SetActive(pData->mvEmitters[i].mbActive);

			if(pData->mvEmitters[i].mbDying) pPE->KillInstantly();
		}
	}

	//-----------------------------------------------------------------------

	void cParticleSystem3D::SaveDataSetup(cSaveObjectHandler *apSaveObjectHandler, cGame *apGame)
	{
		kSaveData_SetupBegin(cParticleSystem3D);
	}

	//-----------------------------------------------------------------------
}
