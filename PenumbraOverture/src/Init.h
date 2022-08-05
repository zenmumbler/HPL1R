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
#ifndef GAME_INIT_H
#define GAME_INIT_H

#include "StdAfx.h"

using namespace hpl;

class cMapHandler;
class cButtonHandler;
class cPlayer;
class cGameScripts;
class cGameMessageHandler;
class cInventory;
class cFadeHandler;
class cSaveHandler;
class cTriggerHandler;
class cAttackHandler;
class cNotebook;
class cNumericalPanel;
class cDeathMenu;
class cGraphicsHelper;
class cEffectHandler;
class cPlayerHands;
class cGameMusicHandler;
class cRadioHandler;
class cMapLoadText;
class cPreMenu;
class cCredits;
class cDebugMenu;

class cIntroStory;

class cMainMenu;

#include "GameTypes.h"

#define GAME_SERIES _W("Penumbra")
#define GAME_NAME _W("Overture")

#if defined(WIN32)
	#define PERSONAL_RELATIVEROOT
	#define PERSONAL_RELATIVEPIECES
	#define PERSONAL_RELATIVEPIECES_COUNT 0
	#define PERSONAL_RELATIVEGAME_PARENT GAME_SERIES _W(" ") _W(GAME_NAME) _W("/")
	#define PERSONAL_RELATIVEGAME GAME_SERIES _W(" ") GAME_NAME _W("/Episode1/")
#elif defined(__linux__)
	#define PERSONAL_RELATIVEROOT _W(".frictionalgames/")
	#define PERSONAL_RELATIVEPIECES _W(".frictionalgames"),
	#define PERSONAL_RELATIVEPIECES_COUNT 1
	#define PERSONAL_RELATIVEGAME_PARENT GAME_SERIES _W("/")
	#define PERSONAL_RELATIVEGAME GAME_SERIES _W("/") GAME_NAME _W("/")
#else
	#define PERSONAL_RELATIVEROOT _W("Library/Preferences/Frictional Games/")
	#define PERSONAL_RELATIVEPIECES _W("Library"), _W("Library/Preferences"), _W("Library/Preferences/Frictional Games"),
	#define PERSONAL_RELATIVEPIECES_COUNT 3
	#define PERSONAL_RELATIVEGAME_PARENT GAME_SERIES _W("/")
	#define PERSONAL_RELATIVEGAME GAME_SERIES _W("/") GAME_NAME _W("/")
#endif

//--------------------------------------------------------

class cInit : public iUpdateable
{
public:
	cInit();
	~cInit();

	bool Init(tString asCommandLine);
	void Run();
	void Exit();
	
	void OnStart();
	void Update(float afTimeStep);
	void Reset();

	void ResetGame(bool abRunInitScript);

	void RunScriptCommand(const tString& asCommand);

	void CreateHardCodedPS(iParticleEmitterData *apPE);

	void PreloadSoundEntityData(const tString &asFile);
	void PreloadParticleSystem(const tString &asFile);
	
	//Main objects
	cConfigFile* mpConfig;
	cConfigFile* mpGameConfig;
	cGame *mpGame;
	cScriptModule *mpGlobalScript;

	FontData *mpDefaultFont;
	
	//Main properties
	cVector2l mvScreenSize;
	bool mbFullScreen;
	bool mbVsync;
	tString msGlobalScriptFile;
	tString msLanguageFile;
	bool mbLogResources;
	bool mbShowPreMenu;
	bool mbShowMenu;
	bool mbShowIntro;
	int mlFSAA;
	eGameDifficulty mDifficulty;

	tString msDeviceName;

	ePhysicsAccuracy mPhysicsAccuracy;
	float mfPhysicsUpdatesPerSec;

	tString msStartMap;
	tString msStartLink;
	
	bool mbSubtitles;
	bool mbDisablePersonalNotes;
	bool mbAllowQuickSave;
	bool mbFlashItems;
	bool mbShowCrossHair;

	tWString msErrorMessage;

	bool mbDebugInteraction;
	float mfMaxPhysicsTimeStep;
	bool mbDestroyGraphics; //If graphics should be destroyed by the entities. Set to false at map change.
	bool mbResetCache;
	bool mbPostEffects;
	int mlMaxSoundDataNum;
	int mlMaxPSDataNum;

	bool mbWeaponAttacking; //To see if it is a weapon that is dealing the damage.

	tString msCurrentUser;

	//Game objects
	cMapHandler *mpMapHandler;
	cButtonHandler *mpButtonHandler;
	cPlayer *mpPlayer;
	//cGameScripts *mpGameScripts;
	cGameMessageHandler *mpGameMessageHandler;
	cInventory* mpInventory;
	cFadeHandler* mpFadeHandler;
	cSaveHandler* mpSaveHandler;
	cTriggerHandler* mpTriggerHandler;
	cAttackHandler* mpAttackHandler;
	cNotebook* mpNotebook;
	cNumericalPanel* mpNumericalPanel;
	cDeathMenu* mpDeathMenu;
	cGraphicsHelper *mpGraphicsHelper;
	cEffectHandler *mpEffectHandler;
	cPlayerHands *mpPlayerHands;
	cGameMusicHandler *mpMusicHandler;
	cRadioHandler *mpRadioHandler;
	cMapLoadText *mpMapLoadText;
	cPreMenu *mpPreMenu;
	cCredits *mpCredits;
	cDebugMenu *mpDebugMenu;
	
	cMainMenu *mpMainMenu;

	cIntroStory* mpIntroStory;	

};


#endif // GAME_INIT_H
