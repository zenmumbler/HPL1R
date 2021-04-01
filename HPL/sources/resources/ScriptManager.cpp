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
#include "resources/ScriptManager.h"
#include "system/String.h"
#include "system/System.h"
#include "resources/Resources.h"
#include "system/LowLevelSystem.h"



namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cScriptManager::cScriptManager(cScript* apScript, cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
							apResources->GetLowLevelSystem())
	{
		mpScript = apScript;
		mpResources = apResources;
	}

	cScriptManager::~cScriptManager()
	{
		DestroyAll();
		Log(" Done with scripts\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cScriptModule* cScriptManager::CreateScript(const tString& asName)
	{
		tString sPath;
		cScriptModule* pScript;
		tString asNewName;

		BeginLoad(asName);

		asNewName = cString::SetFileExt(asName,"hps");

		pScript = static_cast<cScriptModule*>(this->FindLoadedResource(asNewName,sPath));

		if (pScript == nullptr && sPath!="")
		{
			pScript = mpScript->CreateScript(asNewName);

			if (pScript->CreateFromFile(sPath)==false){
				hplDelete(pScript);
				EndLoad();
				return nullptr;
			}

			AddResource(pScript);
		}

		if (pScript) pScript->IncUserCount();
		else Error("Couldn't create script '%s'\n", asNewName.c_str());

		EndLoad();
		return pScript;
	}

	//-----------------------------------------------------------------------

	iResourceBase* cScriptManager::Create(const tString& asName)
	{
		return CreateScript(asName);
	}

	//-----------------------------------------------------------------------

	void cScriptManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if (apResource->HasUsers()==false) {
			RemoveResource(apResource);
			hplDelete(apResource);
		}
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------
}
