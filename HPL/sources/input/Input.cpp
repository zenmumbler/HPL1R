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

#include "system/STLHelpers.h"
#include "system/Log.h"
#include "input/Input.h"
#include "input/Mouse.h"
#include "input/Keyboard.h"
#include "input/LowLevelInput.h"
#include "input/Action.h"
#include "input/ActionKeyboard.h"
#include "input/ActionMouseButton.h"

namespace hpl
{
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cInput::cInput(iLowLevelInput *apLowLevelInput) : iUpdateable("HPL_Input")
	{
		mpLowLevelInput = apLowLevelInput;

		mpKeyboard = mpLowLevelInput->CreateKeyboard();
		mpMouse = mpLowLevelInput->CreateMouse();

		mlstInputDevices.push_back(mpMouse);
		mlstInputDevices.push_back(mpKeyboard);
	}

	//-----------------------------------------------------------------------

	cInput::~cInput()
	{
		Log("Exiting Input Module\n");
		Log("--------------------------------------------------------\n");

		STLMapDeleteAll(m_mapActions);

		if(mpKeyboard)delete mpKeyboard;
		if(mpMouse)delete mpMouse;

		Log("--------------------------------------------------------\n\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cInput::Update(float afTimeStep)
	{
		mpLowLevelInput->BeginInputUpdate();

		for(tInputDeviceListIt it = mlstInputDevices.begin(); it!= mlstInputDevices.end();++it)
		{
			(*it)->Update();
		}

		mpLowLevelInput->EndInputUpdate();

		for(tActionMapIt it = m_mapActions.begin(); it!= m_mapActions.end();++it)
		{
			it->second->Update(afTimeStep);
		}
	}

	//-----------------------------------------------------------------------

	void cInput::AddAction(iAction *apAction)
	{
		tString sName = apAction->GetName();
		tActionMap::value_type val = tActionMap::value_type(sName,apAction);
		m_mapActions.insert(val);
	}

	//-----------------------------------------------------------------------

	bool cInput::IsTriggerd(tString asName)
	{
		iAction *pAction = GetAction(asName);
		if(pAction==NULL){return false;}//Log("doesn't exist!!!");return false;}

		return pAction->IsTriggerd();
	}

	//-----------------------------------------------------------------------

	bool cInput::WasTriggerd(tString asName)
	{
		iAction *pAction = GetAction(asName);
		if(pAction==NULL)return false;

		return pAction->WasTriggerd();
	}

	//-----------------------------------------------------------------------

	bool cInput::BecameTriggerd(tString asName)
	{
		iAction *pAction = GetAction(asName);
		if(pAction==NULL)return false;

		return pAction->BecameTriggerd();
	}

	//-----------------------------------------------------------------------

	bool cInput::DoubleTriggerd(tString asName, float afLimit)
	{
		iAction *pAction = GetAction(asName);
		if(pAction==NULL)return false;

		return pAction->DoubleTriggerd(afLimit);
	}

	//-----------------------------------------------------------------------

	iKeyboard* cInput::GetKeyboard()
	{
		return mpKeyboard;
	}

	//-----------------------------------------------------------------------

	iMouse* cInput::GetMouse()
	{
		return mpMouse;
	}

	//-----------------------------------------------------------------------

	iAction* cInput::GetAction(tString asName)
	{
		tActionMapIt it = m_mapActions.find(asName);
		if(it==m_mapActions.end())return NULL;

		return it->second;
	}

	//-----------------------------------------------------------------------

	void cInput::DestroyAction(tString asName)
	{
		iAction *pOldAction = GetAction(asName);
		if(pOldAction) delete pOldAction;
		m_mapActions.erase(asName);
	}

	//-----------------------------------------------------------------------

	bool cInput::CheckForInput()
	{
		//////////////////////
		//Keyboard
		for(int i=0; i< eKey_LastEnum; ++i)
		{
			if(mpKeyboard->KeyIsDown((eKey)i)) return true;
		}

		//////////////////////
		//Mouse
		for(int i=0; i< eMButton_LastEnum; ++i)
		{
			if(mpMouse->ButtonIsDown((eMButton)i)) return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------

	iAction* cInput::InputToAction(const tString &asName)
	{
		iAction *pAction=NULL;

		//////////////////////
		//Keyboard
		for(int i=0; i< eKey_LastEnum; ++i)
		{
			if(mpKeyboard->KeyIsDown((eKey)i))
			{
				pAction = new cActionKeyboard(asName,this,(eKey)i);
				break;
			}
		}

		//////////////////////
		//Mouse
		if(pAction==NULL)
		{
			for(int i=0; i< eMButton_LastEnum; ++i)
			{
				if(mpMouse->ButtonIsDown((eMButton)i))
				{
					pAction = new cActionMouseButton(asName,this,(eMButton)i);
					break;
				}
			}
		}

		if(pAction)
		{
			iAction *pOldAction = GetAction(asName);
			if(pOldAction) delete pOldAction;

			m_mapActions.erase(asName);

			AddAction(pAction);
		}

		return pAction;
	}

	//-----------------------------------------------------------------------

}
