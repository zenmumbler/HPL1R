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
#include "resources/GpuProgramManager.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/GPUProgram.h"
#include "system/String.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGpuProgramManager::cGpuProgramManager(cFileSearcher *apFileSearcher,iLowLevelGraphics *apLowLevelGraphics)
		: iResourceManager(apFileSearcher)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
	}

	cGpuProgramManager::~cGpuProgramManager()
	{
		DestroyAll();

		Log(" Done with Gpu programs\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	 * Since further parameters are needed for the gpu prog this does not work...
	 * For now at least.
	 * \param asName
	 * \return
	 */
	iResourceBase* cGpuProgramManager::Create(const tString& asName)
	{
		return NULL;
	}

	//-----------------------------------------------------------------------

	iGpuProgram* cGpuProgramManager::CreateProgram(const tString& asVertexName,
												   const tString& asFragmentName)
	{
		auto combinedName = asVertexName + "__" + asFragmentName;
		tString dummyPath, vertexPath, fragmentPath;
		iGpuProgram* pProgram;
		pProgram = static_cast<iGpuProgram*>(FindLoadedResource(combinedName, dummyPath));
		FindLoadedResource(asVertexName, vertexPath);
		FindLoadedResource(asFragmentName, fragmentPath);

		BeginLoad(combinedName);

		if (pProgram == NULL && vertexPath.length() > 0 && fragmentPath.length() > 0) {
			pProgram = mpLowLevelGraphics->CreateGpuProgram(combinedName);

			if(pProgram->CreateFromFile(vertexPath, fragmentPath) == false)
			{
				Error("Couldn't create program '%s'\n",combinedName.c_str());
				delete pProgram;
				EndLoad();
				return NULL;
			}

			AddResource(pProgram);
		}

		if(pProgram)pProgram->IncUserCount();
		else Error("Couldn't load program '%s'\n",combinedName.c_str());

		EndLoad();
		return pProgram;
	 }

	//-----------------------------------------------------------------------

	void cGpuProgramManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if(apResource->HasUsers()==false){
			RemoveResource(apResource);
			delete apResource;
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
