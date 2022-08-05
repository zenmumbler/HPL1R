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
#include "input/impl/LowLevelInputSDL.h"

#include "input/impl/MouseSDL.h"
#include "input/impl/KeyboardSDL.h"
#include "graphics/impl/LowLevelGraphicsSDL.h"

#include "imgui/backends/imgui_impl_sdl.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelInputSDL::cLowLevelInputSDL(iLowLevelGraphics *apLowLevelGraphics)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
		LockInput(true);
	}

	//-----------------------------------------------------------------------

	cLowLevelInputSDL::~cLowLevelInputSDL()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLowLevelInputSDL::LockInput(bool abX)
	{
		cLowLevelGraphicsSDL *pGraphics = static_cast<cLowLevelGraphicsSDL*>(mpLowLevelGraphics);
		pGraphics->SetInputGrab(abX);
	}

	//-----------------------------------------------------------------------

	void cLowLevelInputSDL::BeginInputUpdate()
	{
		//SDL_PumpEvents();

		SDL_Event sdlEvent;

		while(SDL_PollEvent(&sdlEvent)!=0)
		{
			ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

			// SDL_MOUSE* events are 0x04nn, SDL_KEY* events are 0x03nn
			// don't forward mouse or key events to the game when the debug console
			// is trying to capture them.
			bool skipLocal =
				(((sdlEvent.type & 0xFF00) == 0x400) && ImGui::GetIO().WantCaptureMouse) ||
				(((sdlEvent.type & 0xFF00) == 0x300) && ImGui::GetIO().WantCaptureKeyboard);
			if (! skipLocal) {
				mlstEvents.push_back(sdlEvent);
			}
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelInputSDL::EndInputUpdate()
	{
		mlstEvents.clear();
	}

	//-----------------------------------------------------------------------

	iMouse* cLowLevelInputSDL::CreateMouse()
	{
		return new cMouseSDL(this,mpLowLevelGraphics);
	}

	//-----------------------------------------------------------------------

	iKeyboard* cLowLevelInputSDL::CreateKeyboard()
	{
		return new cKeyboardSDL(this);
	}

	//-----------------------------------------------------------------------

}
