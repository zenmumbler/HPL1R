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
#ifndef GAME_INTRO_STORY_H
#define GAME_INTRO_STORY_H

#include "StdAfx.h"

#include "GameTypes.h"
#include "GraphicsHelper.h"

using namespace hpl;

class cInit;

#define INTRO_IMAGE_NUM (6)

//----------------------------------------

class cIntroImage
{
public:
	cIntroImage();

	void Update(float afTimeStep);

	void OnDraw(cGraphicsDrawer *drawer);

	void FadeBrightnessTo(float afDarkness,float afTime);
	void FadeAlphaTo(float afDarkness,float afTime);

	void MoveTo(const cVector3f &avPos,float afTime);

	void Reset();
	
	//////////////
	//Variables
    cVector3f mvCameraPosition;
	cVector3f mvFinalPos;
	cVector3f mvPosStep;
	float mfPosCount;
	cVector3f mvPosDistMul;

	tVector3fList mlstPrevPos;
	int mlMaxPrevPos;
	
	iTexture *mpTexture;

	bool mbActive;

	float mfBrightness;
	float mfBrightnessStep;
	float mfFinalBrightness;

	float mfAlpha;
	float mfAlphaStep;
	float mfFinalAlpha;

	int index;
};


//----------------------------------------

class cIntroStory : public iUpdateable
{
public:
	cIntroStory(cInit *apInit);

	void Update(float afTimeStep);
	void Reset();
	void OnDraw();
	void OnPostSceneDraw();

	void Exit();
	
	bool IsActive(){ return mbActive;}
	void SetActive(bool abX);
    
private:
	void SetMessage(const tWString &asMess);
	
	void Image00();
	void Image01();
	void Image02();
	void Image03();
	void Image04();
	void Image05();
	
	bool mbSearchNext;

	cInit *mpInit;
	cTextureManager *mpTexManager;
	cSoundHandler *mpSoundHandler;

	iTexture *mpBlackTexture;

	cIntroImage mvImages[INTRO_IMAGE_NUM];

	float mfTimerCount;
	int mlNextStop;

	float mfVoiceVol;

	FontData *mpFont;

	bool mbActive;

	tWString msMessage;
	tWString msCentreText;

	eButtonHandlerState mLastButtonState;
};

//----------------------------------------


#endif // GAME_INTRO_STORY_H
