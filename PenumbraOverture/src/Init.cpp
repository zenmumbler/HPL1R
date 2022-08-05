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
#include "Init.h"
#include "Player.h"
#include "ButtonHandler.h"
#include "MapHandler.h"
#include "GameObject.h"
#include "GameScripts.h"
#include "GameMessageHandler.h"
#include "GameSwingDoor.h"
#include "GameArea.h"
#include "GameStickArea.h"
#include "GameSaveArea.h"
#include "GameItem.h"
#include "GameLink.h"
#include "GameEnemy.h"
#include "GameLamp.h"
#include "GameLadder.h"
#include "GameDamageArea.h"
#include "GameForceArea.h"
#include "GameLiquidArea.h"

#include "Inventory.h"
#include "FadeHandler.h"
#include "SaveHandler.h"
#include "TriggerHandler.h"
#include "AttackHandler.h"
#include "Notebook.h"
#include "NumericalPanel.h"
#include "DeathMenu.h"
#include "GraphicsHelper.h"
#include "EffectHandler.h"
#include "PlayerHands.h"
#include "GameMusicHandler.h"
#include "RadioHandler.h"
#include "MapLoadText.h"
#include "PreMenu.h"
#include "Credits.h"
#include "DebugMenu.h"

#include "MainMenu.h"

#include "IntroStory.h"

#include "Version.h" // cool version .h that uses SVN revision #s

// MUST include Last as Unix X11 header defined DestroyAll which blows up MapHandler.h class definition
#include "game/impl/SDLGameSetup.h"

//Global init...
cInit* gpInit;

///////////////////////////////////////////////
// BEGIN CHECK SUPPORT

bool CheckSupport(cInit *apInit)
{
//	iLowLevelGraphics *pLowLevelGraphics = apInit->mpGame->GetGraphics()->GetLowLevel();
	Log("Success!\n");

	return true;
}

// END CHECK SUPPORT
///////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

tWString gsUserSettingsPath = _W("");
tWString gsDefaultSettingsPath = _W("config/default_settings.cfg");
bool gbUsingUserSettings=false;

//-----------------------------------------------------------------------

cInit::cInit(void)  : iUpdateable("Init")
{
	mbDestroyGraphics = true;
	mbWeaponAttacking = false;
	mbResetCache = true;
	mlMaxSoundDataNum = 120;
	mlMaxPSDataNum = 12;
	mbShowCrossHair = false;

	gpInit = this;
}

//-----------------------------------------------------------------------

cInit::~cInit(void)
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cInit::CreateHardCodedPS(iParticleEmitterData *apPE)
{
	cParticleSystemData3D *pPS = new cParticleSystemData3D(apPE->GetName(),mpGame->GetResources(),mpGame->GetGraphics());
	pPS->AddEmitterData(apPE);
	mpGame->GetResources()->GetParticleManager()->AddData3D(pPS);
}

//-----------------------------------------------------------------------

bool cInit::Init(tString asCommandLine)
{
	//iResourceBase::SetLogCreateAndDelete(true);

	// PERSONAL DIR /////////////////////
	tWString sPersonalDir = GetSystemSpecialPath(eSystemPath_Personal);
	if(	cString::GetLastCharW(sPersonalDir) != _W("/") && 
		cString::GetLastCharW(sPersonalDir) != _W("\\"))
	{
		sPersonalDir += _W("/");
	}

	// CREATE NEEDED DIRS /////////////////////
	gsUserSettingsPath = sPersonalDir + PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME _W("settings.cfg");

	tWString vDirs[] = { PERSONAL_RELATIVEPIECES //auto includes ,
			PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME_PARENT,
			PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME};
	int lDirNum = PERSONAL_RELATIVEPIECES_COUNT + 2;

	//Check if directories exist and if not create
	for(int i=0; i<lDirNum; ++i)
	{
		tWString sDir = sPersonalDir + vDirs[i];
		if(FolderExists(sDir)) continue;

		CreateFolder(sDir);
	}

	// LOG FILE SETUP /////////////////////
	SetLogFile(cString::To8Char(sPersonalDir+PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME _W("hpl.log")).c_str());
	SetUpdateLogFile(cString::To8Char(sPersonalDir+PERSONAL_RELATIVEROOT PERSONAL_RELATIVEGAME _W("hpl_update.log")).c_str());

	// MAIN INIT /////////////////////
	
	//Check for what settings file to use.
	if(FileExists(gsUserSettingsPath))
	{
		mpConfig = new cConfigFile(gsUserSettingsPath);
		gbUsingUserSettings = true;
	}
	else
	{
		mpConfig = new cConfigFile(gsDefaultSettingsPath);
		gbUsingUserSettings = false;
	}
	
	//Load config file
	mpConfig->Load();

#ifdef LAST_INIT_OK
	//If last init was not okay, reset all settings.
	if ( mpConfig->GetBool("Game","LastInitOK",true) == false )
	{
		delete  mpConfig ;
		mpConfig = new cConfigFile(gsDefaultSettingsPath);
		gbUsingUserSettings = false;
		CreateMessageBoxW(
			_W("Info"), 
			_W("Game did not end properly last run, resetting configuration"));
	}
#endif

	//Init is not done, so we do not know if it is okay.
	if (gbUsingUserSettings) {
		mpConfig->SetBool("Game","LastInitOK",false);
		mpConfig->Save();
	}

    mpGameConfig = new cConfigFile(_W("config/game.cfg"));
	mpGameConfig->Load();

	mvScreenSize.x = mpConfig->GetInt("Screen","Width",800);
	mvScreenSize.y = mpConfig->GetInt("Screen","Height",600);
	mbFullScreen = mpConfig->GetBool("Screen", "FullScreen", true);
	mbVsync = mpConfig->GetBool("Screen", "Vsync", false);
	mbLogResources = mpConfig->GetBool("Debug", "LogResources", false);
	mbDebugInteraction = mpConfig->GetBool("Debug", "DebugInteraction", false);
	
	mbSubtitles  = mpConfig->GetBool("Game","Subtitles",true); 
	mbAllowQuickSave  = mpConfig->GetBool("Game","AllowQuickSave",false);
	mbFlashItems  = mpConfig->GetBool("Game","FlashItems",true);
	mbShowCrossHair  = mpConfig->GetBool("Game","ShowCrossHair",false);

	msGlobalScriptFile = mpConfig->GetString("Map","GlobalScript","global_script.hps");
	msLanguageFile = mpConfig->GetString("Game","LanguageFile","english.lang");
	msCurrentUser = mpConfig->GetString("Game","CurrentUser","default");
	mDifficulty = (eGameDifficulty)mpConfig->GetInt("Game","Difficulty",1);

	msStartMap = mpConfig->GetString("Map","File","level00_01_boat_cabin.dae"); 
	msStartLink = mpConfig->GetString("Map","StartPos","link01");

	mlFSAA = mpConfig->GetInt("Graphics","FSAA",0);
	mbPostEffects = mpConfig->GetBool("Graphics","PostEffects",true);
	
	// Rehatched: High is now Classic, lower than Classic is no longer supported
	eMaterialQuality iQuality = static_cast<eMaterialQuality>(mpConfig->GetInt("Graphics","ShaderQuality",eMaterialQuality_Classic));
	if (iQuality != eMaterialQuality_Classic) {
		// until we do the work, _higher_ than classic is also not supported
		iQuality = eMaterialQuality_Classic;
	}
	iMaterial::SetQuality(iQuality);
	
	mPhysicsAccuracy = (ePhysicsAccuracy)mpConfig->GetInt("Physics","Accuracy",ePhysicsAccuracy_High);
	mfPhysicsUpdatesPerSec = mpConfig->GetFloat("Physics","UpdatesPerSec",60.0f);

	msDeviceName = mpConfig->GetString("Sound","DeviceName","NULL");

	iGpuProgram::SetLogDebugInformation(false);
	iResourceBase::SetLogCreateAndDelete(mbLogResources);

	GameSetupOptions options;
	options.ScreenWidth = mvScreenSize.x;
	options.ScreenHeight = mvScreenSize.y;
	options.Fullscreen = mbFullScreen;
	options.Multisampling = mlFSAA;
	options.AudioDeviceName = mpConfig->GetString("Sound", "DeviceName", "NULL");
	options.WindowCaption = "Penumbra Overture";

	iLowLevelGameSetup *pSetUp = NULL;

	pSetUp = new cSDLGameSetup();
	mpGame = new cGame(pSetUp, options);

	mpGame->GetGraphics()->GetLowLevel()->SetVsyncActive(mbVsync);

	mbShowPreMenu = mpConfig->GetBool("Game","ShowPreMenu",true);
	mbShowMenu = mpConfig->GetBool("Game","ShowMenu",true);
	mbShowIntro = mpConfig->GetBool("Game","ShowIntro",true);

	mfMaxPhysicsTimeStep = 1.0f / mfPhysicsUpdatesPerSec;
	
	cMath::Randomize();

	// RESOURCE AND LANGUAGE INIT //////////////////
	mpGame->GetResources()->SetupResourceDirsForLanguage(msLanguageFile);

	Log("Initializing " PRODUCT_NAME "\n  Version\t" PRODUCT_VERSION "\n  Date\t" PRODUCT_DATE "\n");
	///////////////////////////////////////////////
	// Check if computer supports game
	if(CheckSupport(this)==false) return false;

	
	//Add loaders
	mpGame->GetResources()->AddEntity3DLoader(new cEntityLoader_GameObject("Object",this));
	mpGame->GetResources()->AddEntity3DLoader(new cEntityLoader_GameItem("Item",this));
	mpGame->GetResources()->AddEntity3DLoader(new cEntityLoader_GameSwingDoor("SwingDoor",this));
	mpGame->GetResources()->AddEntity3DLoader(new cEntityLoader_GameLamp("Lamp",this));
	mpGame->GetResources()->AddEntity3DLoader(new cEntityLoader_GameEnemy("Enemy",this));

    mpGame->GetResources()->AddArea3DLoader(new cAreaLoader_GameArea("script",this));
	mpGame->GetResources()->AddArea3DLoader(new cAreaLoader_GameLink("link",this));
	mpGame->GetResources()->AddArea3DLoader(new cAreaLoader_GameSaveArea("save",this));
	mpGame->GetResources()->AddArea3DLoader(new cAreaLoader_GameLadder("ladder",this));
	mpGame->GetResources()->AddArea3DLoader(new cAreaLoader_GameDamageArea("damage",this));
	mpGame->GetResources()->AddArea3DLoader(new cAreaLoader_GameForceArea("force",this));
	mpGame->GetResources()->AddArea3DLoader(new cAreaLoader_GameLiquidArea("liquid",this));
	mpGame->GetResources()->AddArea3DLoader(new cAreaLoader_GameStickArea("stick",this));

		
	/// FIRST LOADING SCREEN ////////////////////////////////////
	mpGraphicsHelper = new cGraphicsHelper(this);
	mpGraphicsHelper->DrawLoadingScreen("");

	// SOUND ////////////////////////////////
	mpGame->GetSound()->GetLowLevel()->SetVolume(mpConfig->GetFloat("Sound","Volume",1));
		
	// PHYSICS INIT /////////////////////
	mpGame->GetPhysics()->LoadSurfaceData("materials.cfg");
	
	// EARLY GAME INIT /////////////////////
	mpEffectHandler = new cEffectHandler(this);
	
	// GRAPHICS INIT ////////////////////
//	mpGame->GetGraphics()->GetRendererPostEffects()->SetActive(mbPostEffects);
//	mpGame->GetGraphics()->GetRendererPostEffects()->SetBloomActive(mpConfig->GetBool("Graphics", "Bloom", true));
//	mpGame->GetGraphics()->GetRendererPostEffects()->SetBloomSpread(6);

//	mpGame->GetGraphics()->GetRendererPostEffects()->SetMotionBlurActive(mpConfig->GetBool("Graphics", "MotionBlur", false));
//	mpGame->GetGraphics()->GetRendererPostEffects()->SetMotionBlurAmount(mpConfig->GetFloat("Graphics", "MotionBlurAmount", 0.3f));

	mpGame->GetGraphics()->GetRenderer3D()->SetRefractionUsed(mpConfig->GetBool("Graphics", "Refractions", false));

	mpEffectHandler->GetDepthOfField()->SetDisabled(!mpConfig->GetBool("Graphics", "DepthOfField", true));

	// Rehatched: restrict texture scale to full original size
	int iTextureScale = mpConfig->GetInt("Graphics","TextureSizeLevel",0);
	if (iTextureScale != 0) {
		iTextureScale = 0;
	}
	mpGame->GetResources()->GetMaterialManager()->SetTextureSizeLevel(iTextureScale);

	mpGame->GetResources()->GetMaterialManager()->SetTextureFilter((eTextureFilter)mpConfig->GetInt("Graphics","TextureFilter",0));
	mpGame->GetResources()->GetMaterialManager()->SetTextureAnisotropy(mpConfig->GetFloat("Graphics","TextureAnisotropy",1.0f));

	mpGame->GetGraphics()->GetLowLevel()->SetGammaCorrection(mpConfig->GetFloat("Graphics","Gamma",1.0f));

	mpGame->GetGraphics()->GetRenderer3D()->SetShowShadows((eRendererShowShadows)mpConfig->GetInt("Graphics","Shadows",0));

	mpGame->SetLimitFPS( mpConfig->GetBool("Graphics","LimitFPS",true));

	// BASE GAME INIT /////////////////////
	mpMusicHandler = new cGameMusicHandler(this);
	mpPlayerHands = new cPlayerHands(this);
	mpButtonHandler = new cButtonHandler(this);
	mpMapHandler = new cMapHandler(this);
	mpGameMessageHandler = new cGameMessageHandler(this);
	mpRadioHandler = new cRadioHandler(this);
	mpInventory = new cInventory(this);
	mpFadeHandler = new cFadeHandler(this);
	mpSaveHandler = new cSaveHandler(this);
	mpTriggerHandler = new cTriggerHandler(this);
	mpAttackHandler = new cAttackHandler(this);
	mpNotebook = new cNotebook(this);
	mpNumericalPanel = new cNumericalPanel(this);
	mpDeathMenu = new cDeathMenu(this);
	mpPlayer = new cPlayer(this);
	mpMapLoadText = new cMapLoadText(this);
	mpPreMenu = new cPreMenu(this);
	mpCredits = new cCredits(this);
	mpDebugMenu = new cDebugMenu(this);
    
	mpIntroStory = new cIntroStory(this);

	mpMainMenu = new cMainMenu(this);

	// UPDATE STATES INIT /////////////////////
	//Add to the global state
	mpGame->GetUpdater()->AddGlobalUpdate(mpButtonHandler);
	mpGame->GetUpdater()->AddGlobalUpdate(mpSaveHandler);
	mpGame->GetUpdater()->AddGlobalUpdate(mpDebugMenu);

	//Add to the default state
	// mpGame->GetUpdater()->AddUpdate("Default", mpButtonHandler);
	mpGame->GetUpdater()->AddUpdate("Default", mpPlayer);
	mpGame->GetUpdater()->AddUpdate("Default", mpPlayerHands);
	mpGame->GetUpdater()->AddUpdate("Default", mpMusicHandler);
	mpGame->GetUpdater()->AddUpdate("Default", mpMapHandler);
	mpGame->GetUpdater()->AddUpdate("Default", mpGameMessageHandler);
	mpGame->GetUpdater()->AddUpdate("Default", mpRadioHandler);
	mpGame->GetUpdater()->AddUpdate("Default", mpInventory);
	mpGame->GetUpdater()->AddUpdate("Default", mpFadeHandler);
	mpGame->GetUpdater()->AddUpdate("Default", mpEffectHandler);
	mpGame->GetUpdater()->AddUpdate("Default", mpTriggerHandler);
	mpGame->GetUpdater()->AddUpdate("Default", mpAttackHandler);
	mpGame->GetUpdater()->AddUpdate("Default", mpNotebook);
	mpGame->GetUpdater()->AddUpdate("Default", mpNumericalPanel);
	mpGame->GetUpdater()->AddUpdate("Default", mpDeathMenu);
	mpGame->GetUpdater()->AddUpdate("Default", this); //need this last because of the init script.

	//Add to intro state
	mpGame->GetUpdater()->AddContainer("Intro");
	mpGame->GetUpdater()->AddUpdate("Intro",mpIntroStory);

	//Add to main menu state
	mpGame->GetUpdater()->AddContainer("MainMenu");
	mpGame->GetUpdater()->AddUpdate("MainMenu",mpMainMenu);

	//Add to map load text state
	mpGame->GetUpdater()->AddContainer("MapLoadText");
	mpGame->GetUpdater()->AddUpdate("MapLoadText",mpMapLoadText);

	//Add to pre menu state
	mpGame->GetUpdater()->AddContainer("PreMenu");
	mpGame->GetUpdater()->AddUpdate("PreMenu",mpPreMenu);

	//Add to credits text state
	mpGame->GetUpdater()->AddContainer("Credits");
	mpGame->GetUpdater()->AddUpdate("Credits", mpCredits);

	mpGame->GetUpdater()->SetContainer("Default");
	
	// SCRIPT INIT /////////////////////
	cGameScripts::Init();


	// MISC INIT ///////////////////////
	mpDefaultFont = mpGame->GetResources()->GetFontManager()->CreateFontData("verdana.fnt");
	
	//GLOBAL SCRIPT ////////////////////////////
	mpGlobalScript = mpGame->GetResources()->GetScriptManager()->CreateScript(msGlobalScriptFile);
	if(mpGlobalScript)
	{
		mpGlobalScript->Run("OnInit()");
	}
	
	//mpGraphicsHelper->DrawLoadingScreen("");

	//mpIntroStory->SetActive(true);
	//mpCredits->SetActive(true);

	//mpGame->SetRenderOnce(true);
	//mpGame->GetGraphics()->GetRenderer3D()->SetDebugFlags(eRendererDebugFlag_LogRendering);
	
	if(mbShowPreMenu)
	{
		mpPreMenu->SetActive(true);
	}
	else if(mbShowMenu)
	{
		mpMainMenu->SetActive(true);
	}
	else
	{
		mpMapHandler->Load(	msStartMap,msStartLink);
	}

	if (gbUsingUserSettings) {
		mpConfig->SetBool("Game","LastInitOK", true);
		mpConfig->Save();
	}

	//////////////////////////////////////////////////////////
	// Create newer settings file, if using default.
	if(gbUsingUserSettings == false)
	{
		if(mpConfig) delete  mpConfig ;
		mpConfig = new cConfigFile(gsUserSettingsPath);
		gbUsingUserSettings = true;
	}

	SetWindowCaption("Penumbra");
	
	return true;
}

//-----------------------------------------------------------------------

void cInit::ResetGame(bool abRunInitScript)
{
	mpGame->GetUpdater()->Reset();

	if(abRunInitScript)
	{
		if(mpGlobalScript)
		{
			mpGlobalScript->Run("OnInit()");
		}
	}

	mpGame->GetSound()->GetMusicHandler()->Stop(100);
	mpGame->GetSound()->GetSoundHandler()->StopAll(eSoundDest_All);
}

//-----------------------------------------------------------------------

void cInit::Run()
{
	mpGame->Run();
}

//-----------------------------------------------------------------------

void cInit::OnStart()
{

}

//-----------------------------------------------------------------------

void cInit::Update(float afTimeStep)
{
	if(mpGlobalScript)
	{
		mpGlobalScript->Run("OnUpdate()");
	}
}

//-----------------------------------------------------------------------

void cInit::Reset()
{
	
}

//-----------------------------------------------------------------------

void cInit::Exit()
{
	// PLAYER EXIT /////////////////////
	//Log(" Exit Save Handler\n");
	//delete  mpSaveHandler ;

	Log(" Reset maphandler\n");
	mpMapHandler->Reset();
	

	Log(" Exit Player\n");
	// PLAYER EXIT /////////////////////
	delete  mpPlayer ;
	
	// BASE GAME EXIT //////////////////
	Log(" Exit Button Handler\n");
	delete  mpButtonHandler ;
	Log(" Exit Map Handler\n");
	delete  mpMapHandler ;
	//Log(" Exit Game Scripts\n");
	//delete  mpGameScripts ;
	Log(" Exit Game Message Handler\n");
	delete  mpGameMessageHandler ;
	Log(" Exit Radio Handler\n");
	delete  mpRadioHandler ;
	Log(" Exit Inventory\n");
	delete  mpInventory ;
	Log(" Exit Fade Handler\n");
	delete  mpFadeHandler ;
	Log(" Exit Save Handler\n");
	delete  mpSaveHandler ;
	Log(" Exit Trigger Handler\n");
	delete  mpTriggerHandler ;
	Log(" Exit Attack Handler\n");
	delete  mpAttackHandler ;
	Log(" Exit Notebook\n");
	delete  mpNotebook ;
	Log(" Exit Numerical panel\n");
	delete  mpNumericalPanel ;
	Log(" Exit Intro story\n");
	delete  mpIntroStory ;
	Log(" Exit Death menu\n");
	delete  mpDeathMenu ;
	Log(" Exit Graphics helper\n");
	delete  mpGraphicsHelper ;
	Log(" Exit Main menu\n");
	delete  mpMainMenu ;
	Log(" Exit Player hands\n");
	delete  mpPlayerHands ;
	Log(" Exit Music handler\n");
	delete  mpMusicHandler ;
	Log(" Exit Map Load Text\n");
	delete  mpMapLoadText ;
	Log(" Exit PreMenu\n");
	delete  mpPreMenu ;
	Log(" Exit Credits\n");
	delete  mpCredits ;
	Log(" Exit DebugMenu\n");
	delete  mpDebugMenu ;

    Log(" Saving config\n");
	//Save engine stuff.
//	mpConfig->SetBool("Graphics", "Bloom",mpGame->GetGraphics()->GetRendererPostEffects()->GetBloomActive());
//	mpConfig->SetBool("Graphics", "MotionBlur",mpGame->GetGraphics()->GetRendererPostEffects()->GetMotionBlurActive());
//	mpConfig->SetFloat("Graphics", "MotionBlurAmount",mpGame->GetGraphics()->GetRendererPostEffects()->GetMotionBlurAmount());
	mpConfig->SetBool("Graphics", "DepthOfField",!mpEffectHandler->GetDepthOfField()->IsDisabled());
	mpConfig->GetBool("Graphics", "Refractions", mpGame->GetGraphics()->GetRenderer3D()->GetRefractionUsed());
	
	mpConfig->SetFloat("Sound","Volume",mpGame->GetSound()->GetLowLevel()->GetVolume());
	mpConfig->SetString("Sound","DeviceName",msDeviceName);

	mpConfig->SetInt("Graphics","TextureSizeLevel",mpGame->GetResources()->GetMaterialManager()->GetTextureSizeLevel());
	mpConfig->SetInt("Graphics","TextureFilter",mpGame->GetResources()->GetMaterialManager()->GetTextureFilter());
	mpConfig->SetFloat("Graphics","TextureAnisotropy",mpGame->GetResources()->GetMaterialManager()->GetTextureAnisotropy());

	mpConfig->SetFloat("Graphics","Gamma",mpGame->GetGraphics()->GetLowLevel()->GetGammaCorrection());

	mpConfig->SetInt("Graphics","FSAA",mlFSAA);
	mpConfig->SetBool("Graphics","PostEffects",mbPostEffects);
	mpConfig->SetInt("Graphics","ShaderQuality",iMaterial::GetQuality());
	mpConfig->SetBool("Graphics","LimitFPS",mpGame->GetLimitFPS());

	mpConfig->SetInt("Graphics","Shadows",mpGame->GetGraphics()->GetRenderer3D()->GetShowShadows());
		
	Log(" Exit Effect Handler\n");
	delete  mpEffectHandler ;

	Log(" Exit Game\n");

	// MAIN EXIT /////////////////////
	//Delete the game,  do this after all else is deleted.
	delete  mpGame ;
	
	Log(" Saving last config\n");
	//Save the stuff to the config file
	mpConfig->SetInt("Screen","Width",mvScreenSize.x);
	mpConfig->SetInt("Screen","Height",mvScreenSize.y);
	mpConfig->SetBool("Screen", "FullScreen", mbFullScreen);
	mpConfig->SetBool("Screen", "Vsync", mbVsync);
	
	mpConfig->SetString("Map","GlobalScript",msGlobalScriptFile);
	
	mpConfig->SetBool("Game","Subtitles",mbSubtitles);
	mpConfig->SetBool("Game","DisablePersonalNotes",mbDisablePersonalNotes);
	mpConfig->SetBool("Game","AllowQuickSave",mbAllowQuickSave); 
	mpConfig->SetBool("Game","FlashItems",mbFlashItems); 
	mpConfig->SetBool("Game","ShowCrossHair",mbShowCrossHair);
	
	mpConfig->SetString("Map","File",msStartMap); 
	mpConfig->SetString("Map","StartPos",msStartLink);
	
	mpConfig->SetInt("Game","Difficulty",mDifficulty);
	mpConfig->SetString("Game","CurrentUser",msCurrentUser);
	mpConfig->SetString("Game","LanguageFile",msLanguageFile);
	mpConfig->SetBool("Game","ShowPreMenu",mbShowPreMenu);
	mpConfig->SetBool("Game","ShowMenu",mbShowMenu);
	mpConfig->SetBool("Game","ShowIntro",mbShowIntro);

	mpConfig->SetInt("Physics","PhysicsAccuracy",mPhysicsAccuracy);
	mpConfig->SetFloat("Physics","UpdatesPerSec",mfPhysicsUpdatesPerSec);


	mpConfig->SetBool("Debug", "LogResources", mbLogResources);

	mpConfig->Save();
	delete  mpConfig ;

	delete  mpGameConfig ;
}

//-----------------------------------------------------------------------

void cInit::RunScriptCommand(const tString& asCommand)
{
	if(asCommand[0]=='@')
	{
		if(mpGlobalScript && mpGlobalScript->Run(cString::Sub(asCommand,1))==false)
		{
			Warning("Couldn't run command '%s' from global script\n",asCommand.c_str());
		}
	}
	else
	{
		cWorld3D *pWorld = mpGame->GetScene()->GetWorld3D();
		if(pWorld->GetScript()->Run(asCommand)==false)
		{
			Warning("Couldn't run command '%s' in map script file\n",asCommand.c_str());
		}
	}
}

//-----------------------------------------------------------------------
void cInit::PreloadSoundEntityData(const tString &asFile)
{
	if(asFile=="") return;
	
	mpGame->GetResources()->GetSoundEntityManager()->Preload(asFile);
		
	/*iSoundData *pSound = mpGame->GetResources()->GetSoundManager()->CreateSoundData(asFile,false);
	if(pSound){
		Warning("Couldn't preload sound '%s'\n",asFile.c_str());
	}*/
	
}

//-----------------------------------------------------------------------

void cInit::PreloadParticleSystem(const tString &asFile)
{
	if(asFile=="") return;
	cParticleSystem3D *pPS  = mpGame->GetResources()->GetParticleManager()->CreatePS3D(
																"",asFile,1,cMatrixf::Identity);
	delete  pPS ;
}

//-----------------------------------------------------------------------
