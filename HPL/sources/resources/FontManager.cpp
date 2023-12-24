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
#include "resources/FontManager.h"
#include "system/String.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/FontData.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFontManager::cFontManager(cTextureManager *textureMgr, cGraphicsDrawer *drawer)
		: iResourceManager{"font"}
		, _textureMgr{textureMgr}
		, _drawer{drawer}
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cFontManager::LoadAsset(const tString &name, const tString &fullPath) {
		auto font = new FontData(name, _textureMgr, _drawer);
		if (font->LoadAngelBMFont(fullPath) == false) {
			delete font;
			return nullptr;
		}
		return font;
	}

	static const tString s_Extensions[] { "fnt" };

	std::span<const tString> cFontManager::SupportedExtensions() const {
		return { s_Extensions };
	}

	FontData* cFontManager::CreateFontData(const tString& name)
	{
		return static_cast<FontData*>(GetOrLoadResource(name));
	}

	//-----------------------------------------------------------------------
}
