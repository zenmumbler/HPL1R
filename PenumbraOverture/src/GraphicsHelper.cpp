/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of Penumbra Overture.
 *
 * Penumbra Overture is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Penumbra Overture is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Penumbra Overture.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "GraphicsHelper.h"

#include "Init.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cGraphicsHelper::cGraphicsHelper(cInit *apInit)
{
	mpInit = apInit; // for Translate
	mpLowLevelGfx = apInit->mpGame->GetGraphics()->GetLowLevel();
	mpTexManager = apInit->mpGame->GetResources()->GetTextureManager();
	mpDrawer = apInit->mpGame->GetGraphics()->GetDrawer();

	mpFont = apInit->mpGame->GetResources()->GetFontManager()->CreateFontData("font_menu_small.fnt");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGraphicsHelper::ClearScreen(const cColor &aColor)
{
	mpLowLevelGfx->SetClearColor(aColor);
	mpLowLevelGfx->ClearScreen();
}

//-----------------------------------------------------------------------

void cGraphicsHelper::DrawLoadingScreen(const tString &asFile)
{
	iTexture *pTex= NULL;

	ClearScreen(cColor::Black);

	if (asFile != "")
		pTex = mpTexManager->Create2D(asFile,false);
	if(pTex) {
		mpDrawer->DrawTexture(pTex, 0, {800,600});
	}

	mpFont->Draw(cVector3f(400,300,50),22,cColor(1,1),eFontAlign_Center,
				_W("%ls"),kTranslate("LoadTexts", "Loading").c_str());

	mpDrawer->DrawAll();

	SwapBuffers();

	if(pTex) mpTexManager->Destroy(pTex);
}

//-----------------------------------------------------------------------

void cGraphicsHelper::SwapBuffers()
{
	mpLowLevelGfx->SwapBuffers();
}


//-----------------------------------------------------------------------
