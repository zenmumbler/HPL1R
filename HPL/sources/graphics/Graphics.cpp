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
#include "graphics/Graphics.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/GraphicsDrawer.h"
#include "graphics/Renderer3D.h"
#include "graphics/MaterialHandler.h"
#include "game/Updateable.h"
#include "resources/Resources.h"
#include "system/Log.h"

// Materials
#include "graphics/Material_Universal.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGraphics::cGraphics(iLowLevelGraphics *apLowLevelGraphics)
	{
		mpLowLevelGraphics = apLowLevelGraphics;

		mpDrawer = NULL;
		mpMaterialHandler = NULL;
		mpRenderer3D = NULL;
	}

	//-----------------------------------------------------------------------

	cGraphics::~cGraphics()
	{
		Log("Exiting Graphics Module\n");
		Log("--------------------------------------------------------\n");

		delete mpRenderer3D;
		delete mpDrawer;
		delete mpMaterialHandler;

		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cGraphics::Init(int alWidth, int alHeight, bool abFullscreen,
						 const tString &asWindowCaption, cResources* apResources)
	{
		Log("Initializing Graphics Module\n");
		Log("--------------------------------------------------------\n");

		Log(" Init low level graphics\n");
		mpLowLevelGraphics->Init(alWidth, alHeight, abFullscreen, asWindowCaption);

		Log(" Creating graphic systems\n");
		mpDrawer = new cGraphicsDrawer(mpLowLevelGraphics, apResources->GetImageManager(), apResources->GetGpuProgramManager());
		mpRenderer3D = new cRenderer3D(mpLowLevelGraphics, apResources->GetTextureManager(), apResources->GetGpuProgramManager());

		Log(" Adding engine materials\n");
		mpMaterialHandler = new cMaterialHandler(mpLowLevelGraphics, apResources->GetTextureManager(), apResources->GetGpuProgramManager());
		mpMaterialHandler->AddType(new MaterialType_Universal());

		Log("--------------------------------------------------------\n\n");

		return true;
	}
}
