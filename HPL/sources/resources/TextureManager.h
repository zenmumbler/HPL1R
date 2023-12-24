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
#ifndef HPL_TEXTURE_MANAGER_H
#define HPL_TEXTURE_MANAGER_H

#include "game/Updateable.h"
#include "resources/ResourceManager.h"
#include "graphics/Texture.h"

namespace hpl {

	class Bitmap;

	class cTextureManager : public iResourceManager, public iUpdateable
	{
	public:
		cTextureManager(iLowLevelGraphics *llGfx);

		iResourceBase* LoadAsset(const tString &name, const tString &fullPath) override;
		std::span<const tString> SupportedExtensions() const override;

		iTexture* Create1D(const tString &name);
		iTexture* Create2D(const tString &name);

		/**
		 * Creates an animated texture. The name must be [name].[ext]. And then the textures in the animtion must
		 * be named [name]01.[ext], [name]02.[ext], etc
		 */
		iTexture* CreateAnim2D(const tString &name, eTextureAnimMode animMode);
		iTexture* CreateCubeMap(const tString &name);

		iTexture* CreateFromBitmap(const tString &name, const Bitmap& bitmap);

		void Update(float afTimeStep) override;

	private:
		iTexture* CreateFlatTexture(const tString &name, eTextureTarget target);

		iTexture* FindTexture2D(const tString &name, tString &fullPath);

		tStringVec mvFileFormats;

		iLowLevelGraphics *_llGfx;
	};

}

#endif // HPL_TEXTURE_MANAGER_H
