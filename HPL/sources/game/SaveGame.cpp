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

#include "game/SaveGame.h"
#include "system/Log.h"

namespace hpl {



	//////////////////////////////////////////////////////////////////////////
	// SAVE GAME DATA
	//////////////////////////////////////////////////////////////////////////

	//------------------------------------------------------------------------

	//Serialize iSaveGame
	kBeginSerializeBaseVirtual(iSaveData)
	kSerializeVar(mlSaveDataId, eSerializeType_Int32)
	kEndSerialize()

	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SAVE GAME OBJECT
	//////////////////////////////////////////////////////////////////////////

	int iSaveObject::_mlGlobalIdCount =0;

	//------------------------------------------------------------------------

	iSaveObject::iSaveObject()
	{
		mlSaveObjectId = _mlGlobalIdCount++;
		if(_mlGlobalIdCount <0) _mlGlobalIdCount=0;

		mbIsSaved = true;
	}

	iSaveObject::~iSaveObject()
	{

	}

	//------------------------------------------------------------------------

	void iSaveObject::SaveToSaveData(iSaveData *apSaveData)
	{
		apSaveData->mlSaveDataId = mlSaveObjectId;
	}

	//------------------------------------------------------------------------

	void iSaveObject::LoadFromSaveData(iSaveData *apSaveData)
	{
		mlSaveObjectId = apSaveData->mlSaveDataId;
		mpSaveData = apSaveData;
	}

	//------------------------------------------------------------------------

	void iSaveObject::SaveDataSetup(cSaveObjectHandler *apSaveObjectHandler, cGame *apGame)
	{

	}

	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SAVE GAME CONTAINER
	//////////////////////////////////////////////////////////////////////////

	//------------------------------------------------------------------------

	cSaveObjectHandler::cSaveObjectHandler()
	{

	}

	cSaveObjectHandler::~cSaveObjectHandler()
	{

	}

	//------------------------------------------------------------------------

	void cSaveObjectHandler::Add(iSaveObject *pObject)
	{
		m_mapSaveObjects.insert(tSaveObjectMap::value_type(pObject->GetSaveObjectId(),pObject));
	}

	//------------------------------------------------------------------------

	iSaveObject* cSaveObjectHandler::Get(int alId)
	{
		tSaveObjectMapIt it = m_mapSaveObjects.find(alId);
		if(it== m_mapSaveObjects.end()){
			Warning("Couldn't find save object with id %d\n",alId);
			return NULL;
		}

		return it->second;
	}

	//------------------------------------------------------------------------

	cSaveObjectIterator cSaveObjectHandler::GetIterator()
	{
		return cSaveObjectIterator(&m_mapSaveObjects);
	}

	//------------------------------------------------------------------------

	void cSaveObjectHandler::SetUpAll(cGame *apGame)
	{
		int lMaxId =0;

		tSaveObjectMapIt it = m_mapSaveObjects.begin();
		for(; it != m_mapSaveObjects.end(); ++it)
		{
			iSaveObject *pObject = it->second;

			if(pObject->GetSaveObjectId() > lMaxId) lMaxId = pObject->GetSaveObjectId();

			pObject->SaveDataSetup(this,apGame);
		}

		iSaveObject::_mlGlobalIdCount = lMaxId;
	}

	//------------------------------------------------------------------------

	void cSaveObjectHandler::Clear()
	{
		m_mapSaveObjects.clear();
	}

	//------------------------------------------------------------------------

	size_t cSaveObjectHandler::Size()
	{
		return m_mapSaveObjects.size();
	}

	//------------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// SAVE DATA CONTAINER
	//////////////////////////////////////////////////////////////////////////

	//------------------------------------------------------------------------

	cSaveDataHandler::cSaveDataHandler()
	{

	}

	cSaveDataHandler::~cSaveDataHandler()
	{

	}

	//------------------------------------------------------------------------

	void cSaveDataHandler::Add(iSaveData *pData)
	{
		m_mapSaveData.insert(tSaveDataMap::value_type(pData->GetSaveCreatePrio(), pData));
	}

	cSaveDataIterator cSaveDataHandler::GetIterator()
	{
		return cSaveDataIterator(&m_mapSaveData);
	}

	void cSaveDataHandler::Clear()
	{
		m_mapSaveData.clear();
	}

	size_t cSaveDataHandler::Size()
	{
		return m_mapSaveData.size();
	}

	//------------------------------------------------------------------------

	void cSaveDataHandler::AddVoidPtr(void **apPtr)
	{
		iSaveData** pDataPtr = (iSaveData**)apPtr;
		Add(*pDataPtr);
	}
	void cSaveDataHandler::AddVoidClass(void *apClass)
	{
		iSaveData* pData = (iSaveData*)(apClass);
		Add(pData);
	}

	iContainerIterator* cSaveDataHandler::CreateIteratorPtr()
	{
		return new cSaveDataIterator(&m_mapSaveData);
	}

	//------------------------------------------------------------------------


}
