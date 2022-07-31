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
#include "StdAfx.h"
#include "aswrapcall.h"
#include "GameScripts.h"

#include "Init.h"
#include "MapHandler.h"
#include "Player.h"
#include "PlayerHelper.h"
#include "GameEntity.h"
#include "GameSwingDoor.h"
#include "GameItem.h"
#include "GameObject.h"
#include "GameArea.h"
#include "GameStickArea.h"
#include "GameEnemy.h"
#include "GameLink.h"
#include "GameLamp.h"
#include "GameLadder.h"
#include "GameDamageArea.h"
#include "GameForceArea.h"
#include "GameLiquidArea.h"
#include "GameSaveArea.h"
#include "GameMessageHandler.h"
#include "RadioHandler.h"
#include "Inventory.h"
#include "Notebook.h"
#include "NumericalPanel.h"
#include "FadeHandler.h"
#include "EffectHandler.h"
#include "AttackHandler.h"
#include "SaveHandler.h"
#include "GraphicsHelper.h"
#include "MainMenu.h"
#include "GameMusicHandler.h"
#include "Credits.h"

#include "GlobalInit.h"

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// SCRIPT FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///////// HELPERS ////////////////////////////////////

//-----------------------------------------------------------------------

#define GAME_ENTITY_BEGIN(asName) iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName); \
	if(pEntity==NULL){ \
	Warning("Couldn't find game entity '%s'\n",asName.c_str()); \
	return; } \

//-----------------------------------------------------------------------


///////// GENERAL GAME ////////////////////////////////////

static void ResetGame()
{
	gpInit->mpGame->ResetLogicTimer();

	gpInit->mpMapHandler->SetCurrentMapName("");
	gpInit->mpMainMenu->SetActive(true);
}

//-----------------------------------------------------------------------

static void StartCredits()
{
	gpInit->mpCredits->SetActive(true);
}

//-----------------------------------------------------------------------

static void ClearSavedMaps()
{
	gpInit->mpSaveHandler->ClearSavedMaps();
}

//-----------------------------------------------------------------------

static std::string GetActionKeyString(const std::string& asAction)
{
	iAction *pAction = gpInit->mpGame->GetInput()->GetAction(asAction);
	if(pAction)
	{
		return pAction->GetInputName();
	}
	return "ActionNotFound";
}

//-----------------------------------------------------------------------

static void AddMessageTrans(const std::string& asTransCat,
									  const std::string& asTransName)
{
	cInit *mpInit = gpInit;
	gpInit->mpGameMessageHandler->Add(kTranslate(asTransCat,asTransName));
}

//-----------------------------------------------------------------------

static void AddMessage(const std::string& asMessage)
{
	gpInit->mpGameMessageHandler->Add(cString::To16Char(asMessage));
}

//-----------------------------------------------------------------------

static void AddSubTitleTrans(const std::string& asTransCat, const std::string& asTransName, float afTime)
{
	cInit *mpInit = gpInit;
	gpInit->mpEffectHandler->GetSubTitle()->Add(kTranslate(asTransCat,asTransName),afTime,false);
}

//-----------------------------------------------------------------------

static void AddSubTitle(const std::string& asMessage, float afTime)
{
	gpInit->mpEffectHandler->GetSubTitle()->Add(cString::To16Char(asMessage),afTime,false);
}

//-----------------------------------------------------------------------

static void AddRadioMessage(const std::string& asTransCat,const std::string& asTransName, const std::string& asSound)
{
	cInit *mpInit = gpInit;
	gpInit->mpRadioHandler->Add(kTranslate(asTransCat,asTransName),asSound);
}

//-----------------------------------------------------------------------

static void SetRadioOnEndCallback(const std::string& asFunc)
{
	gpInit->mpRadioHandler->SetOnEndCallback(asFunc);
}

//-----------------------------------------------------------------------

static void SetInventoryMessage(const std::string& asMessage)
{
	gpInit->mpInventory->SetMessage(cString::To16Char(asMessage));
}

static void SetInventoryMessageTrans(const std::string& asTransCat,
											   const std::string& asTransName)
{
	cInit *mpInit = gpInit;
	gpInit->mpInventory->SetMessage(kTranslate(asTransCat,asTransName));
}

//-----------------------------------------------------------------------

static void SetMessagesOverCallback(const std::string& asFunction)
{
	gpInit->mpGameMessageHandler->SetOnMessagesOverCallback(asFunction);
}

//-----------------------------------------------------------------------

static void ChangeMap(const std::string& asMapFile, const std::string& asMapPos,
								const std::string& asStartSound, const std::string& asStopSound,
								float afFadeOutTime, float afFadeInTime,
								const std::string& asLoadTextCat, const std::string& asLoadTextEntry)
{
	//if(gpInit->mpRadioHandler->IsActive()) return;

	gpInit->mpMapHandler->ChangeMap(asMapFile,asMapPos, asStartSound,asStopSound,
									afFadeOutTime,afFadeInTime,
									asLoadTextCat,asLoadTextEntry);
}

//-----------------------------------------------------------------------

static void SetMapGameName(const std::string& asName)
{
	gpInit->mpMapHandler->SetMapGameName(cString::To16Char(asName));
}

static void SetMapGameNameTrans(const std::string& asTransCat,const std::string& asTransEntry)
{
	cInit *mpInit = gpInit;
	gpInit->mpMapHandler->SetMapGameName(kTranslate(asTransCat,asTransEntry));
}

//-----------------------------------------------------------------------

static void AddNotebookTaskText(const std::string& asName,const std::string& asText)
{
	gpInit->mpNotebook->AddTask(asName, cString::To16Char(asText));
}

//-----------------------------------------------------------------------

static void AddNotebookTask(const std::string& asName, const std::string& asTransCat,
									  const std::string& asTransEntry)
{
	cInit *mpInit = gpInit;
	gpInit->mpNotebook->AddTask(asName, kTranslate(asTransCat,asTransEntry));
}	

//-----------------------------------------------------------------------

static void RemoveNotebookTask(const std::string& asName)
{
	gpInit->mpNotebook->RemoveTask(asName);
}

//-----------------------------------------------------------------------

static void AddNotebookNote(const std::string& asNameCat, const std::string& asNameEntry,
											 const std::string& asTextCat,	const std::string& asTextEntry)
{
	cInit *mpInit = gpInit;
	gpInit->mpNotebook->AddNote(kTranslate(asNameCat,asNameEntry),asTextCat,asTextEntry);
}

//-----------------------------------------------------------------------

static void StartNumericalPanel(const std::string& asName,int alCode1,int alCode2,int alCode3,int alCode4,
										  float afDifficulty, const std::string& asCallback)
{
	gpInit->mpNumericalPanel->SetActive(true);
	tIntVec vCode; vCode.resize(4);
	vCode[0] = alCode1; vCode[1] = alCode2; vCode[2] = alCode3; vCode[3] = alCode4;
	
	gpInit->mpNumericalPanel->SetUp(asName,asCallback);
	gpInit->mpNumericalPanel->SetCode(vCode);
}

//-----------------------------------------------------------------------

static void SetInventoryActive(bool abX)
{
	gpInit->mpInventory->SetActive(abX);
}

//-----------------------------------------------------------------------

static void FadeIn(float afTime)
{
	gpInit->mpFadeHandler->FadeIn(afTime);
}

static void FadeOut(float afTime)
{
	gpInit->mpFadeHandler->FadeOut(afTime);
}

static bool IsFading()
{
	return gpInit->mpFadeHandler->IsActive();
}

//-----------------------------------------------------------------------

static void SetWideScreenActive(bool abActive)
{
	gpInit->mpFadeHandler->SetWideScreenActive(abActive);
}

//-----------------------------------------------------------------------

static void AutoSave()
{
	if(gpInit->mpPlayer->GetHealth()<=0) return;

	//TODO: Some other effect here.
	//gpInit->mpGraphicsHelper->DrawLoadingScreen("other_saving.jpg");
	//gpInit->mpSaveHandler->SaveGameToFile("save/auto.sav");
	//gpInit->mpSaveHandler->AutoSave("Auto",5);
	gpInit->mpEffectHandler->GetSaveEffect()->AutoSave();
}

//-----------------------------------------------------------------------

static void StartFlash(float afFadeIn, float afWhite, float afFadeOut)
{
	gpInit->mpEffectHandler->GetFlash()->Start(afFadeIn,afWhite,afFadeOut);
}

//-----------------------------------------------------------------------

static void SetWaveGravityActive(bool abX)
{
	gpInit->mpEffectHandler->GetWaveGravity()->SetActive(abX);
}

static void SetupWaveGravity(float afMaxAngle, float afSwingLength, 
									   float afGravitySize, const std::string& asAxis)
{
	int lDir = cString::ToLowerCase(asAxis) == "x" ? 0 : 1;

	gpInit->mpEffectHandler->GetWaveGravity()->Setup(cMath::ToRad(afMaxAngle),afSwingLength,afGravitySize,lDir);
}

//-----------------------------------------------------------------------

static void SetDepthOfFieldActive(bool abX, float afFadeTime)
{
	gpInit->mpEffectHandler->GetDepthOfField()->SetActive(abX,afFadeTime);
}

//-----------------------------------------------------------------------

static void SetupDepthOfField(float afNearPlane, float afFocalPlane, float afFarPlane)
{
	gpInit->mpEffectHandler->GetDepthOfField()->SetUp(afNearPlane,afFocalPlane,afFarPlane);
	gpInit->mpEffectHandler->GetDepthOfField()->SetFocusBody(NULL);
}

//-----------------------------------------------------------------------

static void FocusOnEntity(const std::string& asEntity)
{
	GAME_ENTITY_BEGIN(asEntity);
	
	if(pEntity->GetBodyNum()==0) {
		Error("Entity %s had no bodies and can not be focus on.\n",pEntity->GetName().c_str());
		return;
	}

	gpInit->mpEffectHandler->GetDepthOfField()->FocusOnBody(pEntity->GetBody(0));
	gpInit->mpEffectHandler->GetDepthOfField()->SetFocusBody(NULL);
}

//-----------------------------------------------------------------------

static void SetConstantFocusOnEntity(const std::string& asEntity)
{
	if(asEntity=="")
	{
		gpInit->mpEffectHandler->GetDepthOfField()->SetFocusBody(NULL);
		return;
	}

	GAME_ENTITY_BEGIN(asEntity);

	if(pEntity->GetBodyNum()==0) {
		Error("Entity %s had no bodies and can not be focus on.\n",pEntity->GetName().c_str());
			return;
	}

	gpInit->mpEffectHandler->GetDepthOfField()->SetFocusBody(pEntity->GetBody(0));
}

//-----------------------------------------------------------------------

static void PlayGameMusic(	const std::string& asFile, float afVolume,float afFadeStep,
											bool abLoop, int alPrio)
{
	gpInit->mpMusicHandler->Play(asFile,abLoop,afVolume,afFadeStep,alPrio);
}

//-----------------------------------------------------------------------

static void StopGameMusic(float afFadeStep, int alPrio)
{
	gpInit->mpMusicHandler->Stop(afFadeStep,alPrio);
}

//-----------------------------------------------------------------------

static void StartScreenShake(float afAmount, float afTime,float afFadeInTime,float afFadeOutTime)
{
	gpInit->mpEffectHandler->GetShakeScreen()->Start(afAmount,afTime,afFadeInTime,afFadeOutTime);
}

//-----------------------------------------------------------------------

static void CreateLightFlashAtArea(const std::string& asArea, float afRadius,
											 float afR,float afG, float afB, float afA,
											 float afAddTime, float afNegTime)
{
	cAreaEntity *pArea = gpInit->mpGame->GetScene()->GetWorld3D()->GetAreaEntity(asArea);
	if(pArea==NULL)
	{
		Error("Could not find area '%s'\n",asArea.c_str());
		return;
	}

    gpInit->mpMapHandler->AddLightFlash(pArea->m_mtxTransform.GetTranslation(),afRadius,
										cColor(afR,afG,afB,afA),afAddTime,afNegTime);
}

//-----------------------------------------------------------------------


///////// ATTACK ////////////////////////////////////

//-----------------------------------------------------------------------

static void CreateSplashDamage(const std::string& asAreaName, float afRadius,
										 float afMinDamage, float afMaxDamge,
										 float afMinForce, float afMaxForce,
										 float afMaxImpulse, int alStrength)
{	
	///////////////////
	//Get area
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asAreaName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Area)
	{
		Warning("Couldn't find area entity '%s'\n",asAreaName.c_str());
		return;
	}
	cGameArea *pArea = static_cast<cGameArea*>(pEntity);

    gpInit->mpAttackHandler->CreateSplashDamage(pArea->GetBody(0)->GetWorldPosition(),
												afRadius,afMinDamage,afMaxDamge,
												afMinForce, afMaxForce, afMaxImpulse,
												eAttackTargetFlag_Bodies | 
												eAttackTargetFlag_Player |
												eAttackTargetFlag_Enemy,
												1, alStrength);
}
//-----------------------------------------------------------------------

///////// GAME TIMER ////////////////////////////////////

//-----------------------------------------------------------------------

static void CreateTimer(const std::string& asName, float afTime, const std::string& asCallback, bool abGlobal)
{
	gpInit->mpMapHandler->CreateTimer(asName,afTime,asCallback,abGlobal);
}

static void DestroyTimer(const std::string& asName)
{
	cGameTimer *pTimer = gpInit->mpMapHandler->GetTimer(asName);
	if(pTimer==NULL){
		Warning("Couldn't find timer '%s'\n",asName.c_str());
		return;
	}
	
	pTimer->mbDeleteMe = true;
}

static int siFuncCounter = 0;

static std::string NextAnonTimerName() {
	int counter = siFuncCounter;
	siFuncCounter = (siFuncCounter + 1) & 1023;
	return "runafter_" + std::to_string(counter);
}

static void RunAfter(float afSecs, const std::string& asCallback) {
	CreateTimer(NextAnonTimerName(), afSecs, asCallback, false);
}

static void RunAfterGlobal(float afSecs, const std::string& asCallback) {
	CreateTimer(NextAnonTimerName(), afSecs, asCallback, true);
}

//-----------------------------------------------------------------------

///////// PLAYER ////////////////////////////////////

//-----------------------------------------------------------------------


static void GivePlayerDamage(float afAmount, const std::string& asType)
{
	ePlayerDamageType type = ePlayerDamageType_BloodSplash;
	tString sLowType = cString::ToLowerCase(asType);

	if(sLowType == "bloodsplash") type = ePlayerDamageType_BloodSplash;
	else if(sLowType == "ice") type = ePlayerDamageType_Ice;
	else 
		Warning("Damage type %s does not exist!\n",asType.c_str());



	gpInit->mpPlayer->Damage(afAmount,type);
}

//-----------------------------------------------------------------------

static void SetPlayerHealth(float afHealth)
{
	gpInit->mpPlayer->SetHealth(afHealth);
}

static float GetPlayerHealth()
{
	return gpInit->mpPlayer->GetHealth();
}

//-----------------------------------------------------------------------

static void SetPlayerPose(const std::string& asPose,bool abChangeDirectly)
{
	tString sPose = cString::ToLowerCase(asPose);

	if(sPose == "stand")
	{
		gpInit->mpPlayer->ChangeMoveState(ePlayerMoveState_Walk,abChangeDirectly);
	}
	else if(sPose == "crouch")
	{
		gpInit->mpPlayer->ChangeMoveState(ePlayerMoveState_Crouch,abChangeDirectly);
	}
	else
	{
		Warning("Player pose mode '%s' does not exist\n",asPose.c_str());
	}
}

static void SetPlayerActive(bool abActive)
{
	gpInit->mpPlayer->SetActive(abActive);
}

//-----------------------------------------------------------------------

static void StartPlayerLookAt(const std::string& asEntityName, float afSpeedMul, float afMaxSpeed)
{
	///////////////////
	//Get entity
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asEntityName);
	if(pEntity==NULL)
	{
		Warning("Couldn't find entity '%s'\n",asEntityName.c_str());
		return;
	}
	if(pEntity->GetBody(0)==NULL)
	{
		Warning("Couldn't find a body in entity '%s'\n",asEntityName.c_str());
		return;
	}
	
	gpInit->mpPlayer->GetLookAt()->SetTarget(pEntity->GetBody(0)->GetWorldPosition(),afSpeedMul,afMaxSpeed);
	gpInit->mpPlayer->GetLookAt()->SetActive(true);
}

static void StopPlayerLookAt()
{
	gpInit->mpPlayer->GetLookAt()->SetActive(false);
}

//-----------------------------------------------------------------------

static void StartPlayerFearFilter(float afStrength)
{
	gpInit->mpPlayer->GetFearFilter()->SetActive(true);
}

static void StopPlayerFearFilter()
{
	gpInit->mpPlayer->GetFearFilter()->SetActive(false);
}

//-----------------------------------------------------------------------

static void SetFlashlightDisabled(bool abDisabled)
{
	gpInit->mpPlayer->GetFlashLight()->SetDisabled(abDisabled);
}

//-----------------------------------------------------------------------

///////// INVENTORY ////////////////////////////////////

//-----------------------------------------------------------------------


static void AddPickupCallback(const std::string& asItem, const std::string& asFunction)
{
	gpInit->mpInventory->AddPickupCallback(asItem,asFunction);
}

static void AddUseCallback(const std::string& asItem, const std::string& asEntity, const std::string& asFunction)
{
	gpInit->mpInventory->AddUseCallback(asItem,asEntity,asFunction);
}

static void AddCombineCallback(const std::string& asItem1,const std::string& asItem2, const std::string& asFunction)
{
	gpInit->mpInventory->AddCombineCallback(asItem1,asItem2,asFunction);
}

//-----------------------------------------------------------------------

static void RemovePickupCallback(const std::string& asFunction)
{
	gpInit->mpInventory->RemovePickupCallback(asFunction);
}

static void RemoveUseCallback(const std::string& asFunction)
{
	gpInit->mpInventory->RemoveUseCallback(asFunction);
}

static void RemoveCombineCallback(const std::string& asFunction)
{
	gpInit->mpInventory->RemoveCombineCallback(asFunction);
}

//-----------------------------------------------------------------------

static bool HasItem(const std::string& asName)
{
	//if(cString::ToLowerCase(asName)=="notebook") return gpInit->mpInventory->GetNoteBookActive();

	return gpInit->mpInventory->GetItem(asName) != NULL;
}

static void RemoveItem(const std::string& asName)
{
	cInventoryItem *pItem = gpInit->mpInventory->GetItem(asName);
	if(pItem)
	{
		gpInit->mpInventory->RemoveItem(pItem);
	}
	else
	{
		Warning("Cannot find item '%s' in inventory\n",asName.c_str());
	}
}

static void GiveItem(const std::string& asName,const std::string& asEntityFile,int alSlotIndex)
{
	gpInit->mpInventory->AddItemFromFile(asName,asEntityFile, alSlotIndex);
}

//-----------------------------------------------------------------------

///////// GAME ENTITY PROPERTIES //////////////////////////////////


//-----------------------------------------------------------------------

static void ReplaceEntity(const std::string& asName, const std::string& asBodyName,
											   const std::string& asNewName, const std::string& asNewFile)
{
	GAME_ENTITY_BEGIN(asName)

	if(pEntity->GetBodyNum()==0){
        Error("Entity '%s' contains no bodies!\n",pEntity->GetName().c_str());
		return;
	}

	iPhysicsBody *pBody = NULL;
	
	if(asBodyName != "" && pEntity->GetBodyNum()>1)
	{
		for(int i= 0; i < pEntity->GetBodyNum(); ++i)
		{
			tString sBodyName = cString::Sub(pEntity->GetBody(i)->GetName(), (int)asName.size() +1);
			if(sBodyName == asBodyName){
				pBody = pEntity->GetBody(i);
				break;
			}
		}

		if(pBody==NULL)
		{
			Error("Body '%s' could not be found in entity '%s'!\n",asBodyName.c_str(),asName.c_str());
			return;
		}
	}
	else
	{
		iPhysicsBody *pBody = pEntity->GetBody(0);
	}

	cMatrixf mtxTransform = pBody->GetWorldMatrix();

	gpInit->mpMapHandler->RemoveGameEntity(pEntity);

	cWorld3D *pWorld = gpInit->mpGame->GetScene()->GetWorld3D();
	pWorld->CreateEntity(asNewName,mtxTransform,asNewFile, true);
}

//-----------------------------------------------------------------------

static void SetGameEntityActive(const std::string& asName, bool abX)
{
	GAME_ENTITY_BEGIN(asName)

	pEntity->SetActive(abX);
}

static bool GetGameEntityActive(const std::string& asName)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName); 
	if(pEntity==NULL){ 
		Warning("Couldn't find game entity '%s'\n",asName.c_str()); 
		return false; 
	}

	return pEntity->IsActive();
}

//-----------------------------------------------------------------------

static void CreateGameEntityVar(const std::string& asEntName, const std::string& asVarName, int alVal)
{
	GAME_ENTITY_BEGIN(asEntName);

	pEntity->CreateVar(asVarName,alVal);
}

static void SetGameEntityVar(const std::string& asEntName, const std::string& asVarName, int alVal)
{
	GAME_ENTITY_BEGIN(asEntName);

	pEntity->SetVar(asVarName,alVal);
}

static void AddGameEntityVar(const std::string& asEntName, const std::string& asVarName, int alVal)
{
	GAME_ENTITY_BEGIN(asEntName);

	pEntity->AddVar(asVarName,alVal);
}

static int GetGameEntityVar(const std::string& asEntName, const std::string& asVarName)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asEntName); 
	if(pEntity==NULL){ 
		Warning("Couldn't find game entity '%s'\n",asEntName.c_str()); 
		return 0; 
	}

	return pEntity->GetVar(asVarName);
}

//-----------------------------------------------------------------------

static void SetGameEntityMaxExamineDist(const std::string& asName,float afDist)
{
	GAME_ENTITY_BEGIN(asName)
	
	pEntity->SetMaxExamineDist(afDist);
}

static void SetGameEntityMaxInteractDist(const std::string& asName,float afDist)
{
	GAME_ENTITY_BEGIN(asName)

	pEntity->SetMaxInteractDist(afDist);
}

//-----------------------------------------------------------------------

static void SetGameEntityDescriptionTrans(const std::string& asName,
													const std::string& asTransCat,
													const std::string& asTransName)
{
	cInit *mpInit = gpInit;
	GAME_ENTITY_BEGIN(asName)

	pEntity->SetDescription(kTranslate(asTransCat,asTransName));
	pEntity->SetShowDescritionOnce(false);
}

//-----------------------------------------------------------------------

static void SetGameEntityDescriptionOnceTrans(const std::string& asName,
														   const std::string& asTransCat,
														   const std::string& asTransName)
{
	cInit *mpInit = gpInit;
	GAME_ENTITY_BEGIN(asName)

	if (asTransCat.length() > 0) {
		pEntity->SetDescription(kTranslate(asTransCat,asTransName));
		pEntity->SetShowDescritionOnce(true);
	}
	else {
		pEntity->SetDescription(tWString());
	}
}

//-----------------------------------------------------------------------

static void SetGameEntityDescription(const std::string& asName, const std::string& asMessage)
{
	GAME_ENTITY_BEGIN(asName)

	pEntity->SetDescription(cString::To16Char(asMessage));
	pEntity->SetShowDescritionOnce(false);
}

static void SetGameEntityGameNameTrans(const std::string& asName,
												 const std::string& asTransCat,
												 const std::string& asTransName)
{
	cInit *mpInit = gpInit;
	GAME_ENTITY_BEGIN(asName)

	pEntity->SetGameName(kTranslate(asTransCat,asTransName));
}

//-----------------------------------------------------------------------

static void ChangeEntityAnimation(const std::string& asName,
											const std::string& asAnimation,
											bool abLoop)
{
	GAME_ENTITY_BEGIN(asName)

 	pEntity->GetMeshEntity()->PlayName(asAnimation,abLoop, true);
}

//-----------------------------------------------------------------------

static void SetEntityHealth(const std::string& asName, float afHealth)
{
	GAME_ENTITY_BEGIN(asName)

	pEntity->SetHealth(afHealth);
}

static void DamageEntity(const std::string& asName, float afDamage, int alStrength)
{
	GAME_ENTITY_BEGIN(asName)

	pEntity->Damage(afDamage,alStrength);
}

//-----------------------------------------------------------------------

static void SetDoorState(const std::string& asName,
											const std::string& asState)
{
	/*iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Door)
	{
		Warning("Couldn't find door entity '%s'\n",asName.c_str());
		return;
	}

	cGameDoor *pDoor = static_cast<cGameDoor*>(pEntity);

	asState = cString::ToLowerCase(asState);
	eGameDoorState DoorState;

    if(asState == "open") DoorState = eGameDoorState_Open;
	else if(asState == "closed") DoorState = eGameDoorState_Closed;
	else if(asState == "opening") DoorState = eGameDoorState_Opening;
	else if(asState == "closing") DoorState = eGameDoorState_Closing;
	else{
		Warning("Invalid door state '%s'\n",asState.c_str());
		return;
	}

	pDoor->ChangeDoorState(DoorState);*/
}

//-----------------------------------------------------------------------

static void SetObjectInteractMode(const std::string& asName,const std::string& asMode)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Object)
	{
		Warning("Couldn't find object entity '%s'\n",asName.c_str());
		return;
	}

	cGameObject *pObject = static_cast<cGameObject*>(pEntity);

	pObject->SetInteractMode(cEntityLoader_GameObject::ToInteractMode(asMode.c_str()));
}

//-----------------------------------------------------------------------

static void SetupLink(const std::string& asName,
								const std::string& asMapFile, const std::string& asMapPos,
								const std::string& asStartSound, const std::string& asStopSound,
								float afFadeOutTime, float afFadeInTime)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Link)
	{
		Warning("Couldn't find object entity '%s'\n",asName.c_str());
		return;
	}

	cGameLink *pLink = static_cast<cGameLink*>(pEntity);

	pLink->msMapFile = asMapFile;
	pLink->msMapPos = asMapPos;
	pLink->msStartSound = asStartSound;
	pLink->msStopSound = asStopSound;
	pLink->mfFadeInTime = afFadeInTime;
	pLink->mfFadeOutTime = afFadeOutTime;
	pLink->msLoadTextCat = "";
	pLink->msLoadTextEntry = "";
}

//-----------------------------------------------------------------------

static void SetupLinkLoadText(const std::string& asName,
										const std::string& asMapFile, const std::string& asMapPos,
										const std::string& asStartSound, const std::string& asStopSound,
										float afFadeOutTime, float afFadeInTime,
										const std::string& asTextCat, const std::string& asTextEntry)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Link)
	{
		Warning("Couldn't find object entity '%s'\n",asName.c_str());
		return;
	}

	cGameLink *pLink = static_cast<cGameLink*>(pEntity);

	pLink->msMapFile = asMapFile;
	pLink->msMapPos = asMapPos;
	pLink->msStartSound = asStartSound;
	pLink->msStopSound = asStopSound;
	pLink->mfFadeInTime = afFadeInTime;
	pLink->mfFadeOutTime = afFadeOutTime;
	pLink->msLoadTextCat = asTextCat;
	pLink->msLoadTextEntry = asTextEntry;
}

//-----------------------------------------------------------------------

static void SetAreaCustomIcon(const std::string& asName, const std::string& asIcon)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Area)
	{
		Warning("Couldn't find area entity '%s'\n",asName.c_str());
		return;
	}
	cGameArea *pArea = static_cast<cGameArea*>(pEntity);

	eCrossHairState aCrosshair = eCrossHairState_None;
	
	tString sIconName = cString::ToLowerCase(asIcon);

	if(sIconName == "active") aCrosshair = eCrossHairState_Active;
	else if(sIconName == "inactive") aCrosshair = eCrossHairState_Inactive;
	else if(sIconName == "invalid") aCrosshair = eCrossHairState_Invalid;
	else if(sIconName == "grab") aCrosshair = eCrossHairState_Grab;
	else if(sIconName == "examine") aCrosshair = eCrossHairState_Examine;
	else if(sIconName == "pointer") aCrosshair = eCrossHairState_Pointer;
	else if(sIconName == "item") aCrosshair = eCrossHairState_Item;
	else if(sIconName == "doorlink") aCrosshair = eCrossHairState_DoorLink;
	else if(sIconName == "pickup") aCrosshair = eCrossHairState_PickUp;
	else if(sIconName == "none") aCrosshair = eCrossHairState_None;
	else Warning("Icon type %s not found!\n",asIcon.c_str());

	pArea->SetCustomIcon(aCrosshair);

}

//-----------------------------------------------------------------------

static void AddEnemyPatrolNode(const std::string& asEnemy,const std::string& asNode,float afTime,
														const std::string& asAnimation)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asEnemy);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Enemy)
	{
		Warning("Couldn't find enemy entity '%s'\n",asEnemy.c_str());
		return;
	}
	iGameEnemy *pEnemy = static_cast<iGameEnemy*>(pEntity);
    
    pEnemy->AddPatrolNode(asNode,afTime,asAnimation);
}

/**
* Clears all the patrol nodes for the enemy
* \param asEnemy The Name of the enemy.
*/
static void ClearEnemyPatrolNodes(const std::string& asEnemy)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asEnemy);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Enemy)
	{
		Warning("Couldn't find enemy entity '%s'\n",asEnemy.c_str());
		return;
	}
	iGameEnemy *pEnemy = static_cast<iGameEnemy*>(pEntity);
	
	pEnemy->ClearPatrolNodes();
}

//-----------------------------------------------------------------------

static void SetEnemyDeathCallback(const std::string& asEnemy,const std::string& asFunction)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asEnemy);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Enemy)
	{
		Warning("Couldn't find enemy entity '%s'\n",asEnemy.c_str());
		return;
	}
	iGameEnemy *pEnemy = static_cast<iGameEnemy*>(pEntity);

	pEnemy->SetOnDeathCallback(asFunction);
}

//-----------------------------------------------------------------------

static void SetEnemyAttackCallback(const std::string& asEnemy,const std::string& asFunction)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asEnemy);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Enemy)
	{
		Warning("Couldn't find enemy entity '%s'\n",asEnemy.c_str());
		return;
	}
	iGameEnemy *pEnemy = static_cast<iGameEnemy*>(pEntity);

	pEnemy->SetOnAttackCallback(asFunction);
}

//-----------------------------------------------------------------------

static float GetEnemyHealth(const std::string& asEnemy)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asEnemy);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Enemy)
	{
		Warning("Couldn't find enemy entity '%s'\n",asEnemy.c_str());
		return 0;
	}
	iGameEnemy *pEnemy = static_cast<iGameEnemy*>(pEntity);

	return pEnemy->GetHealth();
}

//-----------------------------------------------------------------------

static void SetEnemyUseTriggers(const std::string& asEnemy, bool abUseTriggers)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asEnemy);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Enemy)
	{
		Warning("Couldn't find enemy entity '%s'\n",asEnemy.c_str());
		return;
	}
	iGameEnemy *pEnemy = static_cast<iGameEnemy*>(pEntity);

	pEnemy->SetUsesTriggers(abUseTriggers);
}

//-----------------------------------------------------------------------

static void ShowEnemyPlayer(const std::string& asEnemy)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asEnemy);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Enemy)
	{
		Warning("Couldn't find enemy entity '%s'\n",asEnemy.c_str());
		return;
	}
	iGameEnemy *pEnemy = static_cast<iGameEnemy*>(pEntity);


	pEnemy->SetLastPlayerPos(	gpInit->mpPlayer->GetCharacterBody()->GetFeetPosition() + 
								cVector3f(0,0.1f,0));
	pEnemy->ChangeState(STATE_HUNT);
}

//-----------------------------------------------------------------------

static void SetDoorLocked(const std::string& asDoor, bool abLocked)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asDoor);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_SwingDoor)
	{
		Warning("Couldn't find swing door entity '%s'\n",asDoor.c_str());
		return;
	}
	cGameSwingDoor *pDoor = static_cast<cGameSwingDoor*>(pEntity);

	pDoor->SetLocked(abLocked);

}

//-----------------------------------------------------------------------

static void SetupStickArea(	const std::string& asArea, bool abCanDeatch,
											bool abMoveBody,bool abRotateBody,
											bool abCheckCenterInArea, float afPoseTime,
									 const std::string& asAttachSound, const std::string& asDetachSound,
									 const std::string& asAttachPS, const std::string& asDetachPS,
									 const std::string& asAttachFunc, const std::string& asDetachFunc)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asArea);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_StickArea)
	{
		Warning("Couldn't find stick area '%s'\n",asArea.c_str());
		return;
	}
	cGameStickArea *pArea = static_cast<cGameStickArea*>(pEntity);

    pArea->SetCanDeatch(abCanDeatch);

	pArea->SetRotateBody(abRotateBody);
	pArea->SetMoveBody(abMoveBody);

	pArea->SetCheckCenterInArea(abCheckCenterInArea);
	
	pArea->SetPoseTime(afPoseTime);
	
	pArea->SetAttachSound(asAttachSound);
	pArea->SetDetachSound(asDetachSound);

	pArea->SetAttachPS(asAttachPS);
	pArea->SetDetachPS(asDetachPS);

	pArea->SetAttachFunction(asAttachFunc);
	pArea->SetDetachFunction(asDetachFunc);
}

//-----------------------------------------------------------------------

static void AllowAttachment()
{
	cGameStickArea::mbAllowAttachment = true;
}

//-----------------------------------------------------------------------

static void SetLampLit(const std::string& asName,bool abLit, bool abFade)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Lamp)
	{
		Warning("Couldn't find lamp '%s'\n",asName.c_str());
		return;
	}
	cGameLamp *pLamp = static_cast<cGameLamp*>(pEntity);

    pLamp->SetLit(abLit,abFade);
}

//-----------------------------------------------------------------------

static void SetLampFlicker(const std::string& asName,bool abFlicker)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Lamp)
	{
		Warning("Couldn't find lamp '%s'\n",asName.c_str());
		return;
	}
	cGameLamp *pLamp = static_cast<cGameLamp*>(pEntity);

	pLamp->SetFlicker(abFlicker);
}

//-----------------------------------------------------------------------

static void SetLampLitChangeCallback(const std::string& asName,const std::string& asCallback)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Lamp)
	{
		Warning("Couldn't find lamp '%s'\n",asName.c_str());
		return;
	}
	cGameLamp *pLamp = static_cast<cGameLamp*>(pEntity);

	pLamp->SetLitChangeCallback(asCallback);
}

//-----------------------------------------------------------------------

static void SetupLadder(	const std::string& asName,
											const std::string& asAttachSound,
											const std::string& asClimbUpSound,
											const std::string& asClimbDownSound)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_Ladder)
	{
		Warning("Couldn't find ladder '%s'\n",asName.c_str());
		return;
	}
	cGameLadder *pLadder = static_cast<cGameLadder*>(pEntity);

	pLadder->SetAttachSound(asAttachSound);
	pLadder->SetClimbUpSound(asClimbUpSound);
	pLadder->SetClimbDownSound(asClimbDownSound);
}

//-----------------------------------------------------------------------

static void SetupDamageArea(const std::string& asName, float afDamage,
													 float afUpdatesPerSec, int alStrength,
													 bool abDisableObjects, bool abDisableEnemies)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_DamageArea)
	{
		Warning("Couldn't find damage area '%s'\n",asName.c_str());
		return;
	}
	cGameDamageArea *pDamage = static_cast<cGameDamageArea*>(pEntity);

	pDamage->SetDamage(afDamage);
	pDamage->SetUpdatesPerSec(afUpdatesPerSec);
	pDamage->SetStrength(alStrength);
	pDamage->SetDisableObjects(abDisableObjects);
	pDamage->SetDisableEnemies(abDisableEnemies);
	
}

//-----------------------------------------------------------------------

static void SetupForceArea(const std::string& asName,
									 float afMaxForce, float afConstant,
									 float afDestSpeed, float afMaxMass,
									 bool abMulWithMass, bool abForceAtPoint,
									 bool abAffectBodies, bool abAffectCharacters)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_ForceArea)
	{
		Warning("Couldn't find force area '%s'\n",asName.c_str());
		return;
	}
	cGameForceArea *pForce = static_cast<cGameForceArea*>(pEntity);

	pForce->SetMaxForce(afMaxForce);
	pForce->SetConstant(afConstant);
	pForce->SetDestSpeed(afDestSpeed);
	pForce->SetMaxMass(afMaxMass);
	pForce->SetMulWithMass(abMulWithMass);
	pForce->SetForceAtPoint(abForceAtPoint);
	pForce->SetAffectBodies(abAffectBodies);
	pForce->SetAffectCharacters(abAffectCharacters);
}

//-----------------------------------------------------------------------


static void SetupLiquidArea(const std::string& asName,
								  float afDensity, float afLinearViscosity,
								  float afAngularViscosity,
								  const std::string& asPhysicsMaterial,
								  float fR, float fG, float fB,
								  bool abHasWaves)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_LiquidArea)
	{
		Warning("Couldn't find liquid area '%s'\n",asName.c_str());
		return;
	}
	cGameLiquidArea *pLiquid = static_cast<cGameLiquidArea*>(pEntity);

	pLiquid->SetDensity(afDensity);
	pLiquid->SetLinearViscosity(afLinearViscosity);
	pLiquid->SetAngularViscosity(afAngularViscosity);

	pLiquid->SetPhysicsMaterial(asPhysicsMaterial);

	pLiquid->SetColor(cColor(fR,fG,fB,1));

	pLiquid->SetHasWaves(abHasWaves);
}

//-----------------------------------------------------------------------

static void SetupSaveArea(const std::string& asName,
											const std::string& asMessageCat,const std::string& asMessageEntry,
											const std::string& asSound)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asName);
	if(pEntity==NULL || pEntity->GetType() != eGameEntityType_SaveArea)
	{
		Warning("Couldn't find save area '%s'\n",asName.c_str());
		return;
	}
	cGameSaveArea *pSave = static_cast<cGameSaveArea*>(pEntity);

	pSave->SetMessageCat(asMessageCat);
	pSave->SetMessageEntry(asMessageEntry);
	pSave->SetSound(asSound);
}

//-----------------------------------------------------------------------


///////// GAME ENTITY CALLBACKS //////////////////////////////////

//-----------------------------------------------------------------------

static eGameCollideScriptType GetGameCollideScriptType(const tString &asType)
{
	tString sName = cString::ToLowerCase(asType);

    if(sName == "enter") return eGameCollideScriptType_Enter;
	if(sName == "leave") return eGameCollideScriptType_Leave;
	if(sName == "during") return eGameCollideScriptType_During;

	Warning("Collide Type %s doesn't exist!\n",asType.c_str());

	return eGameCollideScriptType_LastEnum;
}

//////////////////////////////

static void AddEntityCollideCallback(const std::string& asType,
											   const std::string& asDestName,
											   const std::string& asEntityName,
											   const std::string& asFuncName)
{
	if(cString::ToLowerCase(asDestName)=="player")
	{
		eGameCollideScriptType type = GetGameCollideScriptType(asType);
		
		if(type != eGameCollideScriptType_LastEnum)
			gpInit->mpPlayer->AddCollideScript(type,asFuncName,asEntityName);
	}
	else
	{
		iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asDestName);
		if(pEntity==NULL)
		{
			Warning("Couldn't find entity '%s'\n",asDestName.c_str());
			return;
		}

		eGameCollideScriptType type = GetGameCollideScriptType(asType);

		if(type != eGameCollideScriptType_LastEnum)
			pEntity->AddCollideScript(type,asFuncName,asEntityName);
	}
	
}

//////////////////////////////

static void RemoveEntityCollideCallback(const std::string& asType,
											   const std::string& asDestName,
											   const std::string& asEntityName)
{
	if(cString::ToLowerCase(asDestName)=="player")
	{
		eGameCollideScriptType type = GetGameCollideScriptType(asType);
		
		if(type != eGameCollideScriptType_LastEnum)
			gpInit->mpPlayer->RemoveCollideScript(type,asEntityName);
	}
	else
	{
		iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asDestName);
		if(pEntity==NULL)
		{
			Warning("Couldn't find entity '%s'\n",asDestName.c_str());
			return;
		}

		eGameCollideScriptType type = GetGameCollideScriptType(asType);

		if(type != eGameCollideScriptType_LastEnum)
		{
			pEntity->RemoveCollideScript(type,asEntityName);
		}
	}
}

//-----------------------------------------------------------------------

static eGameEntityScriptType GetGameScriptType(const tString &asType)
{
	tString sName = cString::ToLowerCase(asType);

	if(sName == "playerinteract") return eGameEntityScriptType_PlayerInteract;
	if(sName == "playerexamine") return eGameEntityScriptType_PlayerExamine;
	if(sName == "playerlook") return eGameEntityScriptType_PlayerPick;
	if(sName == "onupdate") return eGameEntityScriptType_OnUpdate;
	if(sName == "onbreak") return eGameEntityScriptType_OnBreak;

	Warning("Script type '%s' doesn't exist!\n",asType.c_str());

	return eGameEntityScriptType_LastEnum;
}

/////////////////////

static void AddEntityCallback(const std::string& asType,
										const std::string& asDestName,
										const std::string& asFuncName)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asDestName);
	if(pEntity==NULL)
	{
		Warning("Couldn't find entity '%s'\n",asDestName.c_str());
		return;
	}

	eGameEntityScriptType type = GetGameScriptType(asType);

	if(type!= eGameEntityScriptType_LastEnum)
	{
		pEntity->AddScript(type, asFuncName);
	}
}

/////////////////////

static void RemoveEntityCallback(const std::string& asType,
										   const std::string& asDestName)
{
	iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asDestName);
	if(pEntity==NULL)
	{
		Warning("Couldn't find entity '%s'\n",asDestName.c_str());
		return;
	}

	eGameEntityScriptType type = GetGameScriptType(asType);

    if(type!= eGameEntityScriptType_LastEnum)
	{
		pEntity->RemoveScript(type);
	}
}

//-----------------------------------------------------------------------

///////// GAME SOUND //////////////////////////////////

//-----------------------------------------------------------------------

static void CreateSoundEntityAt(const std::string& asType,const std::string& asDestName,
													 const std::string& asSoundName,  const std::string& asSoundFile)
{
	cWorld3D *pWorld = gpInit->mpGame->GetScene()->GetWorld3D();
	iPhysicsWorld *pPhysicsWorld = gpInit->mpGame->GetScene()->GetWorld3D()->GetPhysicsWorld();

	int lType=0;
	tString sTypeLow = cString::ToLowerCase(asType);

	if(sTypeLow=="body")lType = 1;
	else if(sTypeLow=="joint")lType = 2;
	else if(sTypeLow=="entity")lType = 3;

	if(lType == 0)
	{
		Warning("Cannot find type '%s' for sound entity position.\n",asType.c_str());
		return;
	}

	cVector3f vPos;
	
	////////////////////////
	// Body
	if(lType == 1)
	{
		iPhysicsBody *pBody = pPhysicsWorld->GetBody(asDestName);
		if(pBody==NULL)	{
			Warning("Body '%s' coudln't be found!\n",asDestName.c_str()); return;
		}

		vPos = pBody->GetLocalPosition();
	}
	////////////////////////
	// Joint
	else if(lType == 2)
	{
		iPhysicsJoint *pJoint = pPhysicsWorld->GetJoint(asDestName);
		if(pJoint==NULL)	{
			Warning("Body '%s' coudln't be found!\n",asDestName.c_str()); return;
		}

		vPos = pJoint->GetPivotPoint();
	}
	////////////////////////
	// Entity
	{
		iGameEntity *pEntity = gpInit->mpMapHandler->GetGameEntity(asDestName);
		if(pEntity==NULL)
		{
			Warning("Couldn't find entity '%s'\n",asDestName.c_str());
			return;
		}
		
		if(pEntity->GetMeshEntity())
			vPos = pEntity->GetMeshEntity()->GetWorldPosition();
		else
			vPos = pEntity->GetBody(0)->GetLocalPosition();
	}
	
	////////////////////////
	// Create sound.
	cSoundEntity *pSound = pWorld->CreateSoundEntity(asSoundName,asSoundFile,true);
	if(pSound)
	{
		pSound->SetPosition(vPos);
	}
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGameScripts::Init()
{
	cScript *pScript = gpInit->mpGame->GetScript();

#define AddFunc(nm, fn) pScript->GetEngine()->RegisterGlobalFunction(nm, WRAP_FN(fn), asCALL_GENERIC);

	// Game general
	AddFunc("void ResetGame()", ResetGame);
	AddFunc("void StartCredits()", StartCredits);
	AddFunc("void ClearSavedMaps()", ClearSavedMaps);

	AddFunc("string GetActionKeyString(const string &in asAction)", GetActionKeyString);
	AddFunc("void AddMessageTrans(const string &in asTransCat, const string &in asTransName)", AddMessageTrans);
	AddFunc("void AddMessage(const string &in asMessage)", AddMessage);
	AddFunc("void AddSubTitleTrans(const string &in asTransCat, const string &in asTransName, float afTime)", AddSubTitleTrans);
	AddFunc("void AddSubTitle(const string &in asMessage, float afTime)", AddSubTitle);

	AddFunc("void AddRadioMessage(const string &in asTransCat,const string &in asTransName, const string &in asSound)", AddRadioMessage);
	AddFunc("void SetRadioOnEndCallback(const string &in asFunc)", SetRadioOnEndCallback);

	AddFunc("void SetInventoryMessage(const string &in asMessage)", SetInventoryMessage);
	AddFunc("void SetInventoryMessageTrans(const string &in asTransCat, const string &in asTransName)", SetInventoryMessageTrans);
	AddFunc("void SetMessagesOverCallback(const string &in asFunction)", SetMessagesOverCallback);
	AddFunc("void ChangeMap(const string &in asMapFile, const string &in asMapPos, const string &in asStartSound, const string &in asStopSound, float afFadeOutTime, float afFadeInTime, const string &in asLoadTextCat, const string &in asLoadTextEntry)", ChangeMap);

	AddFunc("void SetMapGameName(const string &in asName)", SetMapGameName);
	AddFunc("void SetMapGameNameTrans(const string &in asTransCat,const string &in asTransEntry)", SetMapGameNameTrans);

	AddFunc("void AddNotebookTaskText(const string &in asName,const string &in asText)", AddNotebookTaskText);
	AddFunc("void AddNotebookTask(const string &in asName, const string &in asTransCat, const string &in asTransEntry)", AddNotebookTask);
	AddFunc("void RemoveNotebookTask(const string &in asName)", RemoveNotebookTask);
	AddFunc("void AddNotebookNote(const string &in asNameCat, const string &in asNameEntry, const string &in asTextCat, const string &in asTextEntry)", AddNotebookNote);

	AddFunc("void StartNumericalPanel(const string &in asName,int alCode1,int alCode2,int alCode3,int alCode4, float afDifficulty, const string &in asCallback)", StartNumericalPanel);
	AddFunc("void SetInventoryActive(bool abX)", SetInventoryActive);

	AddFunc("void FadeIn(float afTime)", FadeIn);
	AddFunc("void FadeOut(float afTime)", FadeOut);
	AddFunc("bool IsFading()", IsFading);
	AddFunc("void SetWideScreenActive(bool abActive)", SetWideScreenActive);
	AddFunc("void AutoSave()", AutoSave);
	AddFunc("void StartFlash(float afFadeIn, float afWhite, float afFadeOut)", StartFlash);
	AddFunc("void SetWaveGravityActive(bool abX)", SetWaveGravityActive);
	AddFunc("void SetupWaveGravity(float afMaxAngle, float afSwingLength, float afGravitySize, const string &in asAxis)", SetupWaveGravity);
	AddFunc("void SetDepthOfFieldActive(bool abX, float afFadeTime)", SetDepthOfFieldActive);
	AddFunc("void SetupDepthOfField(float afNearPlane, float afFocalPlane, float afFarPlane)", SetupDepthOfField);
	AddFunc("void FocusOnEntity(const string &in asEntity)", FocusOnEntity);
	AddFunc("void SetConstantFocusOnEntity(const string &in asEntity)", SetConstantFocusOnEntity);
	AddFunc("void PlayGameMusic(const string &in asFile, float afVolume,float afFadeStep, bool abLoop, int alPrio)", PlayGameMusic);
	AddFunc("void StopGameMusic(float afFadeStep, int alPrio)", StopGameMusic);
	AddFunc("void StartScreenShake(float afAmount, float afTime,float afFadeInTime,float afFadeOutTime)", StartScreenShake);
	AddFunc("void CreateLightFlashAtArea(const string &in asArea, float afRadius, float afR,float afG, float afB, float afA, float afAddTime, float afNegTime)", CreateLightFlashAtArea);
	
	// Attack
	AddFunc("void CreateSplashDamage(const string &in asAreaName, float afRadius, float afMinDamage, float afMaxDamge, float afMinForce, float afMaxForce, float afMaxImpulse, int alStrength)", CreateSplashDamage);

	// Timers
	AddFunc("void CreateTimer(const string &in asName, float afTime, const string &in asCallback, bool abGlobal = false)", CreateTimer);
	AddFunc("void DestroyTimer(const string &in asName)", DestroyTimer);
	AddFunc("void RunAfter(float afSecs, const string &in asCallback)", RunAfter);
	AddFunc("void RunAfterGlobal(float afSecs, const string &in asCallback)", RunAfterGlobal);

	// Player
	AddFunc("void GivePlayerDamage(float afAmount, const string &in asType)", GivePlayerDamage);
	AddFunc("void SetPlayerHealth(float afHealth)", SetPlayerHealth);
	AddFunc("float GetPlayerHealth()", GetPlayerHealth);
	AddFunc("void SetPlayerPose(const string &in asPose,bool abChangeDirectly)", SetPlayerPose);
	AddFunc("void SetPlayerActive(bool abActive)", SetPlayerActive);
	AddFunc("void StartPlayerLookAt(const string &in asEntityName, float afSpeedMul, float afMaxSpeed)", StartPlayerLookAt);
	AddFunc("void StopPlayerLookAt()", StopPlayerLookAt);
	AddFunc("void StartPlayerFearFilter(float afStrength)", StartPlayerFearFilter);
	AddFunc("void StopPlayerFearFilter()", StopPlayerFearFilter);

	// Inventory
	AddFunc("void SetFlashlightDisabled(bool abDisabled)", SetFlashlightDisabled);
	AddFunc("void AddPickupCallback(const string &in asItem, const string &in asFunction)", AddPickupCallback);
	AddFunc("void AddUseCallback(const string &in asItem, const string &in asEntity, const string &in asFunction)", AddUseCallback);
	AddFunc("void AddCombineCallback(const string &in asItem1,const string &in asItem2, const string &in asFunction)", AddCombineCallback);
	AddFunc("void RemovePickupCallback(const string &in asFunction)", RemovePickupCallback);
	AddFunc("void RemoveUseCallback(const string &in asFunction)", RemoveUseCallback);
	AddFunc("void RemoveCombineCallback(const string &in asFunction)", RemoveCombineCallback);
	AddFunc("bool HasItem(const string &in asName)", HasItem);
	AddFunc("void RemoveItem(const string &in asName)", RemoveItem);
	AddFunc("void GiveItem(const string &in asName,const string &in asEntityFile,int alSlotIndex)", GiveItem);

	// Game entity properties
	AddFunc("void ReplaceEntity(const string &in asName, const string &in asBodyName, const string &in asNewName, const string &in asNewFile)", ReplaceEntity);
	AddFunc("void SetGameEntityActive(const string &in asName, bool abX)", SetGameEntityActive);
	AddFunc("bool GetGameEntityActive(const string &in asName)", GetGameEntityActive);
	AddFunc("void CreateGameEntityVar(const string &in asEntName, const string &in asVarName, int alVal)", CreateGameEntityVar);
	AddFunc("void SetGameEntityVar(const string &in asEntName, const string &in asVarName, int alVal)", SetGameEntityVar);
	AddFunc("void AddGameEntityVar(const string &in asEntName, const string &in asVarName, int alVal)", AddGameEntityVar);
	AddFunc("int GetGameEntityVar(const string &in asEntName, const string &in asVarName)", GetGameEntityVar);
	AddFunc("void SetGameEntityMaxExamineDist(const string &in asName,float afDist)", SetGameEntityMaxExamineDist);
	AddFunc("void SetGameEntityMaxInteractDist(const string &in asName,float afDist)", SetGameEntityMaxInteractDist);
	AddFunc("void SetGameEntityDescriptionTrans(const string &in asName, const string &in asTransCat, const string &in asTransName)", SetGameEntityDescriptionTrans);
	AddFunc("void SetGameEntityDescriptionOnceTrans(const string &in asName, const string &in asTransCat, const string &in asTransName)", SetGameEntityDescriptionOnceTrans);
	AddFunc("void SetGameEntityDescription(const string &in asName, const string &in asMessage)", SetGameEntityDescription);
	AddFunc("void SetGameEntityGameNameTrans(const string &in asName, const string &in asTransCat, const string &in asTransName)", SetGameEntityGameNameTrans);
	AddFunc("void ChangeEntityAnimation(const string &in asName, const string &in asAnimation, bool abLoop)", ChangeEntityAnimation);
	AddFunc("void SetEntityHealth(const string &in asName, float afHealth)", SetEntityHealth);
	AddFunc("void DamageEntity(const string &in asName, float afDamage, int alStrength)", DamageEntity);
	AddFunc("void SetDoorState(const string &in asName, const string &in asState)", SetDoorState);
	AddFunc("void SetObjectInteractMode(const string &in asName,const string &in asMode)", SetObjectInteractMode);
	AddFunc("void SetupLink(const string &in asName, const string &in asMapFile, const string &in asMapPos, const string &in asStartSound, const string &in asStopSound, float afFadeOutTime, float afFadeInTime)", SetupLink);
	AddFunc("void SetupLinkLoadText(const string &in asName, const string &in asMapFile, const string &in asMapPos, const string &in asStartSound, const string &in asStopSound, float afFadeOutTime, float afFadeInTime, const string &in asTextCat, const string &in asTextEntry)", SetupLinkLoadText);
	AddFunc("void SetAreaCustomIcon(const string &in asName, const string &in asIcon)", SetAreaCustomIcon);
	AddFunc("void AddEnemyPatrolNode(const string &in asEnemy,const string &in asNode,float afTime, const string &in asAnimation)", AddEnemyPatrolNode);
	AddFunc("void ClearEnemyPatrolNodes(const string &in asEnemy)", ClearEnemyPatrolNodes);
	AddFunc("void SetEnemyDeathCallback(const string &in asEnemy,const string &in asFunction)", SetEnemyDeathCallback);
	AddFunc("void SetEnemyAttackCallback(const string &in asEnemy,const string &in asFunction)", SetEnemyAttackCallback);
	AddFunc("float GetEnemyHealth(const string &in asEnemy)", GetEnemyHealth);
	AddFunc("void SetEnemyUseTriggers(const string &in asEnemy, bool abUseTriggers)", SetEnemyUseTriggers);
	AddFunc("void ShowEnemyPlayer(const string &in asEnemy)", ShowEnemyPlayer);
	AddFunc("void SetDoorLocked(const string &in asDoor, bool abLocked)", SetDoorLocked);
	AddFunc("void SetupStickArea(const string &in asArea, bool abCanDeatch, bool abMoveBody,bool abRotateBody, bool abCheckCenterInArea, float afPoseTime, const string &in asAttachSound, const string &in asDetachSound, const string &in asAttachPS, const string &in asDetachPS, const string &in asAttachFunc, const string &in asDetachFunc)", SetupStickArea);
	AddFunc("void AllowAttachment()", AllowAttachment);
	AddFunc("void SetLampLit(const string &in asName,bool abLit, bool abFade)", SetLampLit);
	AddFunc("void SetLampFlicker(const string &in asName,bool abFlicker)", SetLampFlicker);
	AddFunc("void SetLampLitChangeCallback(const string &in asName,const string &in asCallback)", SetLampLitChangeCallback);
	AddFunc("void SetupLadder(const string &in asName, const string &in asAttachSound, const string &in asClimbUpSound, const string &in asClimbDownSound)", SetupLadder);
	AddFunc("void SetupDamageArea(const string &in asName, float afDamage, float afUpdatesPerSec, int alStrength, bool abDisableObjects, bool abDisableEnemies)", SetupDamageArea);
	AddFunc("void SetupForceArea(const string &in asName, float afMaxForce, float afConstant, float afDestSpeed, float afMaxMass, bool abMulWithMass, bool abForceAtPoint, bool abAffectBodies, bool abAffectCharacters)", SetupForceArea);
	AddFunc("void SetupLiquidArea(const string &in asName, float afDensity, float afLinearViscosity, float afAngularViscosity, const string &in asPhysicsMaterial, float fR, float fG, float fB, bool abHasWaves)", SetupLiquidArea);
	AddFunc("void SetupSaveArea(const string &in asName, const string &in asMessageCat,const string &in asMessageEntry, const string &in asSound)", SetupSaveArea);

	// Game entity callbacks
	AddFunc("void AddEntityCollideCallback(const string &in asType, const string &in asDestName, const string &in asEntityName, const string &in asFuncName)", AddEntityCollideCallback);
	AddFunc("void RemoveEntityCollideCallback(const string &in asType, const string &in asDestName, const string &in asEntityName)", RemoveEntityCollideCallback);
	AddFunc("void AddEntityCallback(const string &in asType, const string &in asDestName, const string &in asFuncName)", AddEntityCallback);
	AddFunc("void RemoveEntityCallback(const string &in asType, const string &in asDestName)", RemoveEntityCallback);

	//Game sound
	AddFunc("void CreateSoundEntityAt(const string &in asType,const string &in asDestName, const string &in asSoundName, const string &in asSoundFile)", CreateSoundEntityAt);
}

//-----------------------------------------------------------------------
