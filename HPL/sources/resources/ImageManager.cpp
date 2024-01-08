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
#include "resources/ImageManager.h"
#include "graphics/Image.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cImageManager::cImageManager()
		: iResourceManager{"image"}
	{
	   GetSupportedImageFormatsX(_fileFormats);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cImageManager::LoadAsset(const tString &name, const tString &fullPath) {
		auto image = new Image{name};
		if (! image->CreateFromFile(fullPath)) {
			Error("ImageManager couldn't load '%s'\n", fullPath.c_str());
			delete image;
			return nullptr;
		}
		return image;
	}

	std::span<const tString> cImageManager::SupportedExtensions() const {
		return { _fileFormats };
	}

	//-----------------------------------------------------------------------

	Image* cImageManager::CreateImage(const tString& name)
	{
		return static_cast<Image*>(GetOrLoadResource(name));
	}

	//-----------------------------------------------------------------------
}
