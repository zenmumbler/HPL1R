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
// #include "graphics/RendererPostEffects.h"
#include "graphics/RenderList.h"
#include "graphics/MaterialHandler.h"
#include "graphics/MeshCreator.h"
#include "game/Updateable.h"
#include "resources/Resources.h"
#include "system/Log.h"

// Materials
#ifdef OLD_MATS
#include "graphics/Material_Alpha.h"
#include "graphics/Material_Diffuse.h"
#include "graphics/Material_DiffuseSpec.h"
#include "graphics/Material_Bump.h"
#include "graphics/Material_BumpSpec.h"
#include "graphics/Material_BumpColorSpec.h"
#include "graphics/Material_Additive.h"
#include "graphics/Material_Flat.h"
#include "graphics/Material_Modulative.h"
#include "graphics/Material_ModulativeX2.h"
#include "graphics/Material_Alpha.h"
#include "graphics/Material_EnvMap_Reflect.h"
#include "graphics/Material_Water.h"
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGraphics::cGraphics(iLowLevelGraphics *apLowLevelGraphics)
	{
		mpLowLevelGraphics = apLowLevelGraphics;

		mpDrawer = NULL;
		mpMeshCreator = NULL;
		mpMaterialHandler = NULL;
		mpRenderer3D = NULL;
		// mpRendererPostEffects = NULL;
	}

	//-----------------------------------------------------------------------

	cGraphics::~cGraphics()
	{
		Log("Exiting Graphics Module\n");
		Log("--------------------------------------------------------\n");

		delete mpRenderer3D;
		// delete mpRendererPostEffects;
		delete mpDrawer;
		delete mpMeshCreator;
		delete mpMaterialHandler;
		delete mpRenderList;

		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cGraphics::Init(int alWidth, int alHeight, bool abFullscreen,
						 int alMultisampling, const tString &asWindowCaption,
						 cResources* apResources)
	{
		Log("Initializing Graphics Module\n");
		Log("--------------------------------------------------------\n");

		Log(" Init low level graphics\n");
		mpLowLevelGraphics->Init(alWidth, alHeight, abFullscreen, alMultisampling, asWindowCaption);

		Log(" Creating graphic systems\n");
		mpMaterialHandler = new cMaterialHandler(this, apResources);
		mpDrawer = new cGraphicsDrawer(mpLowLevelGraphics, apResources->GetImageManager());
		mpRenderList = new cRenderList(this);
		mpMeshCreator = new cMeshCreator(mpLowLevelGraphics, apResources);
		mpRenderer3D = new cRenderer3D(mpLowLevelGraphics,apResources,mpMeshCreator,mpRenderList);
//		mpRendererPostEffects = new cRendererPostEffects,(mpLowLevelGraphicsapResources, mpRenderList, mpRenderer3D);
//		mpRenderer3D->SetPostEffects(mpRendererPostEffects);


		//Add all the materials.
		Log(" Adding engine materials\n");
		/*
		mpMaterialHandler->Add(new cMaterialType_Diffuse());
		mpMaterialHandler->Add(new cMaterialType_Bump());
		mpMaterialHandler->Add(new cMaterialType_DiffuseSpec());
		mpMaterialHandler->Add(new cMaterialType_BumpSpec());
		mpMaterialHandler->Add(new cMaterialType_BumpColorSpec());

		mpMaterialHandler->Add(new cMaterialType_Additive());
		mpMaterialHandler->Add(new cMaterialType_Alpha());
		mpMaterialHandler->Add(new cMaterialType_Flat());
		mpMaterialHandler->Add(new cMaterialType_Modulative());
		mpMaterialHandler->Add(new cMaterialType_ModulativeX2());

		mpMaterialHandler->Add(new cMaterialType_EnvMap_Reflect());

		mpMaterialHandler->Add(new cMaterialType_Water());
		*/

		Log("--------------------------------------------------------\n\n");

		return true;
	}

	//-----------------------------------------------------------------------

	iLowLevelGraphics* cGraphics::GetLowLevel()
	{
		return mpLowLevelGraphics;
	}

	//-----------------------------------------------------------------------

	cGraphicsDrawer* cGraphics::GetDrawer()
	{
		return mpDrawer;
	}

	//-----------------------------------------------------------------------

}
