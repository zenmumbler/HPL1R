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
#include "system/LogicTimer.h"
#include "system/System.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLogicTimer::cLogicTimer(int alUpdatesPerSec)
	{
		mlMaxUpdates = alUpdatesPerSec;
		mlUpdateCount = 0;

		SetUpdatesPerSec(alUpdatesPerSec);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	void cLogicTimer::Reset()
	{
		mlLocalTime = (double)GetAppTimeMS();
	}

	//-----------------------------------------------------------------------

	bool cLogicTimer::WantUpdate()
	{
		++mlUpdateCount;
		if(mlUpdateCount > mlMaxUpdates) return false;

		if(mlLocalTime< (double)GetAppTimeMS())
		{
			Update();
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	void cLogicTimer::EndUpdateLoop()
	{
		if(mlUpdateCount > mlMaxUpdates){
			Reset();
		}

		mlUpdateCount=0;
	}

	//-----------------------------------------------------------------------

	void cLogicTimer::SetUpdatesPerSec(int alUpdatesPerSec)
	{
		mlLocalTimeAdd = 1000.0 / ((double)alUpdatesPerSec);
		Reset();
	}

	//-----------------------------------------------------------------------

	void cLogicTimer::SetMaxUpdates(int alMax)
	{
		mlMaxUpdates = alMax;
	}

	//-----------------------------------------------------------------------

	int cLogicTimer::GetUpdatesPerSec()
	{
		return (int)(1000.0 / ((double)mlLocalTimeAdd));
	}

	//-----------------------------------------------------------------------

	float cLogicTimer::GetStepSize()
	{
		return ((float)mlLocalTimeAdd)/1000.0f;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLogicTimer::Update()
	{
		mlLocalTime += mlLocalTimeAdd;
	}

	//-----------------------------------------------------------------------

}
