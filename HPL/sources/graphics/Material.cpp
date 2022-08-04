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
#include "graphics/Material.h"
#include "graphics/Renderer3D.h"
#include "resources/TextureManager.h"
#include "resources/GpuProgramManager.h"
#include "graphics/GPUProgram.h"
#include "scene/Camera.h"
#include "system/String.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	eMaterialQuality iMaterial::mQuality = eMaterialQuality_Classic;

	//-----------------------------------------------------------------------

	iMaterial::iMaterial(const tString& asName,iLowLevelGraphics* apLowLevelGraphics,
		cTextureManager *apTextureManager, cGpuProgramManager* apProgramManager,
		cRenderer3D *apRenderer3D)
		: iResourceBase(asName, 0)
	{
		mvTexture.resize(eMaterialTexture_LastEnum);
		mvTexture.assign(mvTexture.size(), NULL);

		mpLowLevelGraphics = apLowLevelGraphics;
		mpTextureManager = apTextureManager;
		mpRenderer3D = apRenderer3D;
		mpRenderSettings = mpRenderer3D->GetRenderSettings();
		mpProgramManager = apProgramManager;

		mbUsesLights = false;
		mbIsTransperant = false;
		mbHasAlpha = false;
		mbDepthTest = true;
		mfValue = 1;

		for(int j=0;j<kMaxProgramNum;j++) mpProgram[j]=NULL;

		mlPassCount=0;

		mlId = -1;
	}

	iMaterial::~iMaterial()
	{
		int i;

		for(i=0;i<(int)mvTexture.size();i++){
			if(mvTexture[i])
				mpTextureManager->Destroy(mvTexture[i]);
		}

		for(int j=0;j<kMaxProgramNum;j++)
		{
			if(mpProgram[j])
				mpProgramManager->Destroy(mpProgram[j]);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/*void iMaterial::Destroy()
	{

	}*/

	//-----------------------------------------------------------------------

	iTexture* iMaterial::GetTexture(eMaterialTexture aType)
	{
		return mvTexture[aType];
	}

	//-----------------------------------------------------------------------

	cRect2f iMaterial::GetTextureOffset(eMaterialTexture aType)
	{
		cRect2f SizeRect;

		SizeRect.x=0;SizeRect.y=0;
		SizeRect.w = 1;//(float) mvTexture[aType]->GetWidth();
		SizeRect.h = 1;//(float) mvTexture[aType]->GetHeight();

		return SizeRect;
	}

	//-----------------------------------------------------------------------

}
