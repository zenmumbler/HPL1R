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

#include "resources/MeshManager.h"
#include "resources/MeshLoaderHandler.h"
#include "graphics/Mesh.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMeshManager::cMeshManager(cMeshLoaderHandler *meshLoadHandler)
		: iResourceManager{"mesh"}
	{
		_meshLoadHandler = meshLoadHandler;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cMeshManager::LoadAsset(const tString &name, const tString &fullPath) {
		return _meshLoadHandler->LoadMesh(fullPath, 0);
	}

	std::span<const tString> cMeshManager::SupportedExtensions() const {
		return { *(_meshLoadHandler->GetSupportedTypes()) };
	}

	cMesh* cMeshManager::CreateMesh(const tString &name)
	{
		return static_cast<cMesh*>(GetOrLoadResource(name));
	}

	//-----------------------------------------------------------------------

}
