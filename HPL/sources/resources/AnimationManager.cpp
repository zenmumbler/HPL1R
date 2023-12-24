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
#include "resources/AnimationManager.h"
#include "graphics/Mesh.h"
#include "graphics/Animation.h"
#include "resources/MeshLoaderHandler.h"
#include "system/Log.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cAnimationManager::cAnimationManager(cMeshLoaderHandler *meshLoadHandler)
		: iResourceManager{"animation"}
	{
		_meshLoadHandler = meshLoadHandler;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cAnimationManager::LoadAsset(const tString &_name, const tString &fullPath) {
		auto mesh = _meshLoadHandler->LoadMesh(fullPath, 0);
		if (mesh == nullptr) {
			return nullptr;
		}

		if (mesh->GetAnimationNum() <= 0)
		{
			Error("No animations found in '%s'\n", fullPath.c_str());
			delete mesh;
			return nullptr;
		}

		auto animation = mesh->GetAnimation(0);
		mesh->ClearAnimations(false);
		delete mesh;

		return animation;
	}

	std::span<const tString> cAnimationManager::SupportedExtensions() const {
		return { *(_meshLoadHandler->GetSupportedTypes()) };
	}

	//-----------------------------------------------------------------------

	cAnimation* cAnimationManager::CreateAnimation(const tString& name)
	{
		return static_cast<cAnimation*>(GetOrLoadResource(name));
	}

	//-----------------------------------------------------------------------
}
