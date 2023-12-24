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
#include "resources/FileSearcher.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/GPUProgram.h"
#include "system/String.h"
#include "system/Log.h"

using namespace std::string_literals;

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGpuProgramManager::cGpuProgramManager(iLowLevelGraphics *llGfx)
		: iResourceManager{"shader"}
	{
		_llGfx = llGfx;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cGpuProgramManager::LoadAsset(const tString &name, const tString &fullPath) {
		// FIXME: this is unused currently
		// FIXME: this will be removed when we've moved to shader objects / separate functions
		auto separator = name.find("__"s);
		if (separator == tString::npos) {
			Error("Invalid program name '%s'\n", name.c_str());
			return nullptr;
		}
		auto vertexName = name.substr(0, separator) + ".vert";
		auto fragmentName = name.substr(separator + 2) + ".frag";
		auto vertexPath = FileSearcher::GetFilePath(vertexName);
		auto fragmentPath = FileSearcher::GetFilePath(fragmentName);

		// FIXME: normal creation continues here
		auto shader = _llGfx->CreateGpuProgram(name);
		if (shader->CreateFromFile(vertexPath, fragmentPath) == false)
		{
			Error("Couldn't create program '%s'\n", name.c_str());
			delete shader;
			return nullptr;
		}
		return shader;
	}

	static const tString s_Extensions[] { "vert", "frag" };

	std::span<const tString> cGpuProgramManager::SupportedExtensions() const {
		return { s_Extensions };
	}

	//-----------------------------------------------------------------------

	iGpuProgram* cGpuProgramManager::CreateProgram(const tString &vertexName, const tString &fragmentName)
	{
		auto combinedName = vertexName + "__" + fragmentName;
		tString dummyPath, vertexPath, fragmentPath;
		iGpuProgram* pProgram;
		pProgram = static_cast<iGpuProgram*>(FindLoadedResource(combinedName, dummyPath));
		FindLoadedResource(vertexName, vertexPath);
		FindLoadedResource(fragmentName, fragmentPath);

		BeginLoad(combinedName);

		if (pProgram == NULL && vertexPath.length() > 0 && fragmentPath.length() > 0) {
			pProgram = _llGfx->CreateGpuProgram(combinedName);

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

}
