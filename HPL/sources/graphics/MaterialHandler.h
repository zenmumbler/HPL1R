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
#ifndef HPL_MATERIALHANDLER_H
#define HPL_MATERIALHANDLER_H

#include "graphics/Material.h"

#include <vector>

namespace hpl {

	class iLowLevelGraphics;
	class cTextureManager;
	class cGpuProgramManager;

	class cMaterialHandler
	{
	public:
		cMaterialHandler(iLowLevelGraphics *llGfx, cTextureManager* textureManager, cGpuProgramManager* programManager);
		~cMaterialHandler();

		void AddType(iMaterialType* apTypedata);
		iMaterial* Create(const tString& asName,tString asMatName);

	private:
		std::vector<iMaterialType*> _materialTypes;
		iLowLevelGraphics* _llGfx;
		cTextureManager* _textureManager;
		cGpuProgramManager* _programManager;
	};

};

#endif // HPL_MATERIALHANDLER_H
