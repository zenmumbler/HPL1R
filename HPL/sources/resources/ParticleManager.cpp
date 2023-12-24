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

	iResourceBase* cParticleManager::LoadAsset(const tString &name, const tString &fullPath) {
		auto psData = new cParticleSystemData3D(name, _llGfx, _materialMgr);
		if (! psData->LoadFromFile(fullPath)) {
			delete psData;
			return nullptr;
		}
		return psData;
	}

	static const tString s_Extensions[] { "ps" };

	std::span<const tString> cParticleManager::SupportedExtensions() const {
		return { s_Extensions };
	}

	//-----------------------------------------------------------------------

	cParticleSystem3D* cParticleManager::CreateParticleSystem(const tString& name, const tString& asType, cVector3f avSize,const cMatrixf& a_mtxTransform)
	{
		auto data = static_cast<cParticleSystemData3D*>(GetOrLoadResource(name));
		if (data) {
			cParticleSystem3D* system = data->Create(name, avSize, a_mtxTransform);
			system->SetDataName(asType);
			system->SetDataSize(avSize);
			system->SetParticleManager(this);
			return system;
		}

		return nullptr;
	}

	//-----------------------------------------------------------------------

	void cParticleManager::Preload(const tString& asFile)
	{
		GetOrLoadResource(asFile);
	}

	//-----------------------------------------------------------------------

}
