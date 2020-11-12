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
#ifndef HPL_MATERIAL_DIFFUSE_ADDITIVE2D_H
#define HPL_MATERIAL_DIFFUSE_ADDITIVE2D_H

#include "graphics/GfxObject.h"


namespace hpl {

	class cMaterial_DiffuseAdditive2D : public iOldMaterial
	{
	public:
		cMaterial_DiffuseAdditive2D(cImageManager* apImageManager);
		// diffadditive2d

		void StartRendering(iLowLevelGraphics* apLowLevelGraphics) const;
		void EndRendering(iLowLevelGraphics* apLowLevelGraphics) const;
	};

};
#endif // HPL_MATERIAL_DIFFUSE_ADDITIVE2D_H
