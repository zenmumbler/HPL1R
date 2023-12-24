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
#ifndef HPL_IMAGEMANAGER_H
#define HPL_IMAGEMANAGER_H

#include "resources/ResourceManager.h"
#include "resources/FrameBitmap.h"
#include "math/MathTypes.h"

namespace hpl {

	class cResourceImage;
	class iLowLevelGraphics;
	class Bitmap;

	class cImageManager : public iResourceManager
	{
	public:
		cImageManager(iLowLevelGraphics *apLowLevelGraphics);

		iResourceBase* LoadAsset(const tString &name, const tString &fullPath) override;
		std::span<const tString> SupportedExtensions() const override;

		cResourceImage* CreateImage(const tString& name);

	private:
		iLowLevelGraphics *mpLowLevelGraphics;
		std::vector<std::shared_ptr<cFrameBitmap>> _currentFrames;

		std::vector<tString> mvFileFormats;

		cResourceImage *AddToFrame(const tString &name, const Bitmap &bmp);
	};

};

#endif // HPL_RESOURCEMANAGER_H
