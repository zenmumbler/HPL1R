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
#include "resources/ParticleManager.h"
#include "resources/FileSearcher.h"
#include "graphics/ParticleSystem3D.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cParticleManager::cParticleManager(iLowLevelGraphics *llGfx, cMaterialManager *materialMgr)
		: iResourceManager{"particle"}
	{
		_llGfx = llGfx;
		_materialMgr = materialMgr;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cParticleSystem3D* cParticleManager::CreatePS3D(const tString& asName,const tString& asType,
												cVector3f avSize,const cMatrixf& a_mtxTransform)
	{
		tString sTypeName = cString::SetFileExt(cString::ToLowerCase(asType),"");

		cParticleSystemData3D *pData = static_cast<cParticleSystemData3D*>(GetByName(sTypeName));
		if(pData == NULL)
		{
			tString sFile = cString::SetFileExt(asType,"ps");

			tString sPath = FileSearcher::GetFilePath(sFile);

			if(sPath == "")
			{
				Error("Couldn't find particle system file '%s'\n",sFile.c_str());
				return NULL;
			}

			cParticleSystemData3D *pPSData = new cParticleSystemData3D(sTypeName, _llGfx, _materialMgr);

			if(pPSData->LoadFromFile(sPath)==false)
			{
				Error("Can't load data from particle system file '%s'\n",sTypeName.c_str());
				delete pPSData;
				return NULL;
			}

			AddResource(pPSData);

			pData = pPSData;
		}


		pData->IncUserCount();
		cParticleSystem3D* pPS = pData->Create(asName,avSize,a_mtxTransform);
		pPS->SetDataName(asType);
		pPS->SetDataSize(avSize);
		pPS->SetParticleManager(this);

		return pPS;
	}

	//-----------------------------------------------------------------------

	void cParticleManager::Preload(const tString& asFile)
	{
		tString sTypeName = cString::SetFileExt(cString::ToLowerCase(asFile),"");

		cParticleSystemData3D *pData = static_cast<cParticleSystemData3D*>(GetByName(sTypeName));
		if(pData == NULL)
		{
			tString sFile = cString::SetFileExt(asFile,"ps");
			tString sPath = FileSearcher::GetFilePath(sFile);
			if(sPath == "")
			{
				Error("Couldn't find particle system file '%s'\n",sFile.c_str());
				return;
			}

			cParticleSystemData3D *pPSData = new cParticleSystemData3D(sTypeName, _llGfx, _materialMgr);

			if(pPSData->LoadFromFile(sPath)==false)
			{
				Error("Can't load data from particle system file '%s'\n",sTypeName.c_str());
				delete pPSData;
				return;
			}

			AddResource(pPSData);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------
}
