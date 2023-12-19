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
#include "graphics/ParticleEmitter.h"
#include "resources/MaterialManager.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// DATA LOADER
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iParticleEmitterData::iParticleEmitterData(const tString &asName, iLowLevelGraphics *llGfx, cMaterialManager *materialMgr)
	{
		msName = asName;
		_llGfx = llGfx;
		_materialMgr = materialMgr;

		mfWarmUpTime =0;
		mfWarmUpStepsPerSec = 20;
	}

	//-----------------------------------------------------------------------

	iParticleEmitterData::~iParticleEmitterData()
	{
		for(int i=0;i<(int)mvMaterials.size();i++)
		{
			if(mvMaterials[i]) _materialMgr->Destroy(mvMaterials[i]);
		}
	}
	//-----------------------------------------------------------------------

	void iParticleEmitterData::AddMaterial(iMaterial *apMaterial)
	{
		mvMaterials.push_back(apMaterial);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iParticleEmitter::iParticleEmitter(tMaterialVec *avMaterials, unsigned int alMaxParticles, cVector3f avSize)
	{
		mvParticles.resize(alMaxParticles);
		mlMaxParticles = alMaxParticles;
		mlNumOfParticles =0;

		mvMaterials = avMaterials;

		//Update vars:
		mbDying = false;
		mfTime =0;
		mfFrame =0;

		mbUpdateGfx = true;
		mbUpdateBV = true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iParticleEmitter::Update(float afTimeStep)
	{
		UpdateMotion(afTimeStep);

		mfTime++;
		mbUpdateGfx = true;
		mbUpdateBV = true;
	}

	//-----------------------------------------------------------------------

	void iParticleEmitter::KillInstantly()
	{
		mlMaxParticles = 0;
		mlNumOfParticles = 0;
		mbDying = true;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cParticle& iParticleEmitter::CreateParticle()
	{
		// [Rehatched]: if too many particles are created, the last one will be overwritten
		// while this is not amazing, the original implementation return NULL in this case (return type was ptr)
		// but this was not actually checked at the call site, meaning it would have crashed if this occured in-game
		if(mlNumOfParticles < mlMaxParticles) ++mlNumOfParticles;
		return mvParticles[mlNumOfParticles - 1];
	}

	//-----------------------------------------------------------------------

	void iParticleEmitter::SwapRemove(unsigned int alIndex)
	{
		if (alIndex < mlNumOfParticles-1)
		{
			// just overwrite the particle to be removed with the data of the last one
			mvParticles[alIndex] = mvParticles[mlNumOfParticles-1];
		}
		mlNumOfParticles--;
	}

	//-----------------------------------------------------------------------

}
