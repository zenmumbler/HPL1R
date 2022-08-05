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
#include "aswrapcall.h"
#include "script/ScriptFuncs.h"

#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "game/Game.h"
#include "graphics/Graphics.h"
#include "sound/Sound.h"
#include "resources/Resources.h"
#include "math/Math.h"
#include "scene/Scene.h"
#include "scene/Light3D.h"
#include "scene/World3D.h"
#include "scene/SoundEntity.h"
#include "scene/PortalContainer.h"
#include "input/Input.h"
#include "system/Log.h"
#include "sound/MusicHandler.h"
#include "resources/SoundManager.h"
#include "resources/SoundEntityManager.h"
#include "resources/TextureManager.h"
#include "sound/SoundData.h"
#include "sound/SoundChannel.h"
#include "sound/SoundHandler.h"
#include "sound/SoundEntityData.h"
#include "system/String.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsJointHinge.h"
#include "physics/PhysicsJointScrew.h"
#include "physics/PhysicsJointSlider.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsController.h"
#include "graphics/ParticleSystem3D.h"
#include "scene/MeshEntity.h"
#include "graphics/BillBoard.h"
#include "graphics/Beam.h"
#include "graphics/Renderer3D.h"
#include "script/Script.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// JOINT CALLBACK
	//////////////////////////////////////////////////////////////////////////

	cScriptJointCallback::cScriptJointCallback(cScene *apScene)
	{
		mpScene = apScene;

		msMaxFunc = "";
		msMinFunc = "";
	}

	void cScriptJointCallback::OnMinLimit(iPhysicsJoint *apJoint)
	{
		if(msMinFunc!="")
		{
			cScriptModule *pScript = mpScene->GetWorld3D()->GetScript();

			tString sCommand = msMinFunc + "(\"" + apJoint->GetName() + "\")";
			if(pScript->Run(sCommand)==false){
				Warning("Couldn't run script command '%s'\n",sCommand.c_str());
			}

		}
	}

	void cScriptJointCallback::OnMaxLimit(iPhysicsJoint *apJoint)
	{
		if(msMaxFunc!="")
		{
			cScriptModule *pScript = mpScene->GetWorld3D()->GetScript();

			tString sCommand = msMaxFunc + "(\"" + apJoint->GetName() + "\")";
			if(pScript->Run(sCommand)==false){
				Warning("Couldn't run script command '%s'\n",sCommand.c_str());
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	static cResources *gpResources=NULL;
	static cInput *gpInput=NULL;
	static cGraphics *gpGraphics=NULL;
	static cScene *gpScene=NULL;
	static cSound *gpSound=NULL;
	static cGame *gpGame=NULL;

	//-----------------------------------------------------------------------

	static void Print(const std::string& asText)
	{
		Log("%s", asText.c_str());
	}

	static std::string FloatToString(float afX)
	{
		char sTemp[30];
		snprintf(sTemp,sizeof(sTemp),"%f",afX);
		return sTemp;
	}

	static std::string IntToString(int alX)
	{
		char sTemp[30];
		snprintf(sTemp,sizeof(sTemp),"%d",alX);
		return sTemp;
	}

	static float RandFloat(float afMin, float afMax)
	{
		return cMath::RandRectf(afMin,afMax);
	}

	static int RandInt(int alMin, int alMax)
	{
		return cMath::RandRectl(alMin,alMax);
	}

	static bool StringContains(const std::string& asString, const std::string& asSubString)
	{
		return cString::GetLastStringPos(asString,asSubString)>=0;
	}

	static void ResetLogicTimer()
	{
		gpGame->ResetLogicTimer();
	}

	/////////////////////////////////////////////////////////////////////////
	/////// RENDERER //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	* Sets ambient light color-
	**/
	static void SetAmbientColor(float afR, float afG, float afB)
	{
		gpGraphics->GetRenderer3D()->SetAmbientColor(cColor(afR,afG,afB,1.0f));
	}

	//-----------------------------------------------------------------------

	/**
	* Sets if the skybox should be active
	**/
	static void SetSkyboxActive(bool abX)
	{
		gpGraphics->GetRenderer3D()->SetSkyBoxActive(abX);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets the skybox color.
	**/
	static void SetSkyboxColor(float afR, float afG, float afB, float afA)
	{
		gpGraphics->GetRenderer3D()->SetSkyBoxColor(cColor(afR,afG,afB,afA));
	}

	//-----------------------------------------------------------------------

	/**
	* Sets the skybox
	* \param asTexture Name of the cube map texture to use
	**/
	static void SetSkybox(const std::string& asTexture)
	{
		if(asTexture!="")
		{
			iTexture *pTex = gpResources->GetTextureManager()->CreateCubeMap(asTexture,false);
			gpGraphics->GetRenderer3D()->SetSkyBox(pTex,true);
		}
		else
		{
			gpGraphics->GetRenderer3D()->SetSkyBox(NULL,false);
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Creates a particle system and attaches it to the camera.
	* \param asName Name of particle system
	* \param asType The type of particle system (file)
	**/
	static void CreateParticleSystemOnCamera(const std::string& asName,const std::string& asType)
	{
		cParticleSystem3D *pPS = gpScene->GetWorld3D()->CreateParticleSystem(asName,asType,
																			1,cMatrixf::Identity);
		if(pPS)
		{
			auto pCam = gpScene->GetCamera();
			pCam->AttachEntity(pPS);
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Sets if fog should be active
	* \param abX If the fog is active or not.
	**/
	static void SetFogActive(bool abX)
	{
		gpGraphics->GetRenderer3D()->SetFogActive(abX);
	}

	/**
	* Sets if the fog should be used to cull non-visible objects
	* \param abX If the culling is active or not.
	**/
	static void SetFogCulling(bool abX)
	{
		gpGraphics->GetRenderer3D()->SetFogCulling(abX);
	}

	/**
	* Creates a particle system and attaches it to the camera.
	* \param afStart Start of fog color
	* \param afStart End of fog fade. After this limit all geometry is full fog color.
	* \param afR, afG, afB Color of Fog.
	**/
	static void SetFogProperties(float afStart, float afEnd, float afR,float afG, float afB)
	{
		gpGraphics->GetRenderer3D()->SetFogStart(afStart);
		gpGraphics->GetRenderer3D()->SetFogEnd(afEnd);
		gpGraphics->GetRenderer3D()->SetFogColor(cColor(afR,afG,afB,1.0f));
	}

	//-----------------------------------------------------------------------

	static void SetSectorProperties(const std::string& asSector, float afAmbR,float afAmbG, float afAmbB)
	{
		cPortalContainer *pContainer = gpScene->GetWorld3D()->GetPortalContainer();

		cSector *pSector = pContainer->GetSector(asSector);
		if(pSector == NULL){
			Warning("Could not find sector '%s'\n", asSector.c_str());
			return;
		}

		pSector->SetAmbientColor(cColor(afAmbR, afAmbG, afAmbB,1));
	}

	//-----------------------------------------------------------------------

	static void SetSectorPortalActive(const std::string& asSector, int alPortal, bool abActive)
	{
		cPortalContainer *pContainer = gpScene->GetWorld3D()->GetPortalContainer();

		cSector *pSector = pContainer->GetSector(asSector);
		if(pSector == NULL){
			Warning("Could not find sector '%s'\n", asSector.c_str());
			return;
		}

		cPortal *pPortal = pSector->GetPortal(alPortal);
		if(pPortal==NULL)
		{
			Warning("Could not find portal %d in sector '%s'\n",alPortal,asSector.c_str());
			return;
		}

		pPortal->SetActive(abActive);
	}

	/////////////////////////////////////////////////////////////////////////
	/////// RESOURCES //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	/**
	 * Gets a string in the current language.
	 * \param asCat The translation category
	 * \param asName The name of the category entry.
	 **/
	static std::string Translate(const std::string& asCat, const std::string& asName)
	{
		tWString wsText = gpResources->Translate(asCat,asName);
		return cString::To8Char(wsText);
	}

	//-----------------------------------------------------------------------

	/**
	* Preloads the data for a sound.
	* \param asFile This can be a wav, ogg, mp3 or snt file.
	**/
	static void PreloadSound(const std::string& asFile)
	{
		tString sExt = cString::ToLowerCase(cString::GetFileExt(asFile));
		if(sExt=="snt")
		{
			cSoundEntityData *pData = gpResources->GetSoundEntityManager()->CreateSoundEntity(asFile);
			if(pData == NULL) {
				Warning("Couldn't preload sound '%s'\n",asFile.c_str());
				return;
			}

			if(pData->GetMainSoundName() != ""){
				iSoundChannel *pChannel = gpSound->GetSoundHandler()->CreateChannel(pData->GetMainSoundName(),0);
				delete pChannel;
			}
			if(pData->GetStartSoundName() != ""){
				iSoundChannel *pChannel = gpSound->GetSoundHandler()->CreateChannel(pData->GetStartSoundName(),0);
				delete pChannel;
			}
			if(pData->GetStopSoundName() != ""){
				iSoundChannel *pChannel = gpSound->GetSoundHandler()->CreateChannel(pData->GetStopSoundName(),0);
				delete pChannel;
			}
		}
		else
		{
			iSoundData *pSound = gpResources->GetSoundManager()->CreateSoundData(asFile,false);
			if(pSound){
				Warning("Couldn't preload sound '%s'\n",asFile.c_str());
			}
		}
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// MESH ENTITY //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static void SetMeshActive(const std::string& asName, bool abActive)
	{
		cMeshEntity *pEnt = gpScene->GetWorld3D()->GetMeshEntity(asName);
		if(pEnt==NULL){
			Warning("Didn't find mesh entity '%s'\n",asName.c_str());
			return;
		}

		pEnt->SetActive(abActive);
		pEnt->SetVisible(abActive);
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// PARTICLE SYSTEM //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	* Create a particle system at the position of an area
	* \param asName The name of the particle system.
	* \param abActive If it should be active or not.
	**/
	static void SetParticleSystemActive(const std::string& asName, bool abActive)
	{
		cParticleSystem3D *pPS = gpScene->GetWorld3D()->GetParticleSystem(asName);
		if(pPS==NULL){
			Warning("Didn't find particle system '%s'\n",asName.c_str());
			return;
		}

		pPS->SetActive(abActive);
		pPS->SetVisible(abActive);
	}

	//-----------------------------------------------------------------------

	/**
	* Create a particle system at the position of an area
	* \param asName The name of the particle system.
	* \param asType The type of aprticle system
	* \param asArea The name of the area
	* \param X Y and Z the variables of the particle system.
	**/
	static void CreateParticleSystem(const std::string& asName, const std::string& asType, const std::string& asArea,
												float afX, float afY, float afZ)
	{
		cAreaEntity* pArea = gpScene->GetWorld3D()->GetAreaEntity(asArea);
		if(pArea==NULL){
			Warning("Couldn't find area '%s'\n",asArea.c_str());
			return;
		}

		cParticleSystem3D *pPS = gpScene->GetWorld3D()->CreateParticleSystem(asName,asType,
														cVector3f(afX,afY,afZ),pArea->m_mtxTransform);
		if(pPS==NULL){
			Warning("No particle system of type '%s'\n",asType.c_str());
			return;
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Kill a particle system
	* \param asName The name of the particle system.
	**/
	static void KillParticleSystem(const std::string& asName)
	{
		/*cParticleSystem3D *pPS = gpScene->GetWorld3D()->GetParticleSystem(asName);
		if(pPS==NULL){
			Warning("Didn't find particle system '%s'\n",asName.c_str());
			return;
		}
		pPS->Kill();*/

		bool bFound = false;
		cParticleSystem3DIterator it = gpScene->GetWorld3D()->GetParticleSystemIterator();
		while(it.HasNext())
		{
			cParticleSystem3D *pPS = it.Next();

			if(pPS->GetName() == asName)
			{
				pPS->Kill();
				bFound = true;
			}
		}

		if(!bFound) Warning("Didn't find particle system '%s'\n",asName.c_str());
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// BEAM //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	 * Creates an a beam between two areas
	 * \param asName
	 * \param asFile
	 * \param asStartArea
	 * \param asEndArea
	 */
	static void CreateBeam(const std::string& asName, const std::string& asFile,
									const std::string& asStartArea, const std::string& asEndArea)
	{
		cAreaEntity* pStartArea = gpScene->GetWorld3D()->GetAreaEntity(asStartArea);
		if(pStartArea==NULL){
			Warning("Couldn't find area '%s'\n",asStartArea.c_str());
			return;
		}

		cAreaEntity* pEndArea = gpScene->GetWorld3D()->GetAreaEntity(asEndArea);
		if(pEndArea==NULL){
			Warning("Couldn't find area '%s'\n",asEndArea.c_str());
			return;
		}

		cBeam *pBeam = gpScene->GetWorld3D()->CreateBeam(asName);

		if(pBeam->LoadXMLProperties(asFile)==false)
		{
			Error("Couldn't create beam from file '%s'\n",asFile.c_str());
			gpScene->GetWorld3D()->DestroyBeam(pBeam);
			return;
		}

		pBeam->SetPosition(pStartArea->m_mtxTransform.GetTranslation());
		pBeam->GetEnd()->SetPosition(pEndArea->m_mtxTransform.GetTranslation());
	}

	//-----------------------------------------------------------------------

	/**
	 * Destroys a beam
	 * \param asName
	 */
	static void DestroyBeam(const std::string& asName)
	{
		cBeam* pBeam = gpScene->GetWorld3D()->GetBeam(asName);
		if(pBeam==NULL)
		{
			Warning("Couldn't find beam '%s'\n",asName.c_str());
			return;
		}

		gpScene->GetWorld3D()->DestroyBeam(pBeam);
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// LIGHT //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	* Fades a sound to a color and a radius
	* \param asName The name of the light
	* \param afR The red channel to fade to.
	* \param afG The green channel to fade to.
	* \param afB The blue channel to fade to.
	* \param afA The alpha channel to fade to.
	* \param afRadius The radius to fade to.
	* \param afTime The amount of seconds the fade should last.
	**/
	static void FadeLight3D(const std::string& asName, float afR,float afG, float afB, float afA,
										float afRadius, float afTime)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->FadeTo(cColor(afR,afG,afB,afA),afRadius, afTime);
		pLight->SetVisible(true);
		pLight->UpdateLight(2.0f/60.0f);
	}

	//-----------------------------------------------------------------------

	/**
	* Attaches a billboard to a light
	* \param asBillboardName The billbaord name
	* \param asLightName The light name
	* \param abX True if it should be attached, false if you want to remove.
	**/
	static void AttachBillboardToLight3D(const std::string& asBillboardName, const std::string& asLightName,bool abX)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asLightName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asLightName.c_str());
			return;
		}

		cBillboard *pBillboard = gpScene->GetWorld3D()->GetBillboard(asBillboardName);
		if(pBillboard==NULL)
		{
			Warning("Couldn't find billboard '%s'\n",asBillboardName.c_str());
			return;
		}

		if(abX)
			pLight->AttachBillboard(pBillboard);
		else
			pLight->RemoveBillboard(pBillboard);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets on/off a light
	* \param asName The light name
	* \param abX if the light should be on or off.
	**/
	static void SetLight3DVisible(const std::string& asName, bool abX)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->SetVisible(abX);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets on/off for affect only in sector where centre is.
	* \param asName The light name
	* \param abX if the light should only affects objects in same sector or not.
	**/
	static void SetLight3DOnlyAffectInSector(const std::string& asName, bool abX)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->SetOnlyAffectInSector(abX);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets flickering on/off a light
	* \param asName The light name
	* \param abX if the light flicker should be on or off.
	**/
	static void SetLight3DFlickerActive(const std::string& asName, bool abX)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->SetFlickerActive(abX);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets flickering parameters
	* \param asName The light name
	* \param abR, afG, afB, afA The color of the light when off
	* \param afRadius The radius of the light when off.
	* \param afOnMinLength Minimum time before going from off to on.
	* \param afOnMaxLength Maximum time before going from off to on.
	* \param asOnSound Name of the sound played when going from off to on. "" means no sound.
	* \param asOnPS Name of the particle system played when going from off to on. "" means none.
	* \param afOffMinLength Minimum time before going from on to off.
	* \param afOffMaxLength Maximum time before going from on to off.
	* \param asOffSound Name of the sound played when going from on to off. "" means no sound.
	* \param asOffPS Name of the particle system played when going from on to off. "" means none.
	* \param abFade If there should be a fade between off and on.
	* \param afOnFadeLength Fade length from off to on.
	* \param afOffFadeLength Fade length from on to off.
	**/
	static void SetLight3DFlicker(const std::string& asName,
											float afR,float afG, float afB, float afA,
											float afRadius,

											float afOnMinLength, float afOnMaxLength,
											const std::string& asOnSound,const std::string& asOnPS,

											float afOffMinLength, float afOffMaxLength,
											const std::string& asOffSound,const std::string& asOffPS,

											bool abFade,
											float afOnFadeLength, float afOffFadeLength)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->SetFlicker(cColor(afR,afG,afB,afA),afRadius,
							afOnMinLength, afOnMaxLength,asOnSound,asOnPS,
							afOffMinLength, afOffMaxLength,asOffSound,asOffPS,
							abFade,afOnFadeLength,afOffFadeLength);
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// SOUND //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	 * Creates a sound entity at the postion of an area.
	 * \param asName Name of the sound area
	 * \param asFile The snt file to load.
	 * \param asArea The area to create at.
	 */
	static void CreateSoundEntity(const std::string& asName, const std::string& asFile,
												const std::string& asArea)
	{
		cAreaEntity* pArea = gpScene->GetWorld3D()->GetAreaEntity(asArea);
		if(pArea==NULL){
			Warning("Couldn't find area '%s'\n",asArea.c_str());
			return;
		}

		cSoundEntity *pSound = gpScene->GetWorld3D()->CreateSoundEntity(asName,asFile,true);
		if(pSound==NULL)
		{
			Warning("Couldn't create sound entity '%s'\n",asFile.c_str());
			return;
		}

		pSound->SetPosition(pArea->m_mtxTransform.GetTranslation());
	}

	//-----------------------------------------------------------------------

	/**
	* Play a sound entity
	* \param asName The entity name
	* \param abPlayStart If the start sound should be played.
	**/
	static void PlaySoundEntity(const std::string& asName, bool abPlayStart)
	{
		cSoundEntity *pSound = gpScene->GetWorld3D()->GetSoundEntity(asName);
		if(pSound==NULL)
		{
			Warning("Couldn't find sound entity '%s'\n",asName.c_str());
			return;
		}

		pSound->Play(abPlayStart);
	}

	/**
	* Stop a sound entity
	* \param asName The entity name
	* \param abPlayEnd If the end sound should be played.
	**/
	static void StopSoundEntity(const std::string& asName, bool abPlayEnd)
	{
		cSoundEntity *pSound = gpScene->GetWorld3D()->GetSoundEntity(asName);
		if(pSound==NULL)
		{
			Warning("Couldn't find sound entity '%s'\n",asName.c_str());
			return;
		}

		pSound->Stop(abPlayEnd);
	}

	//-----------------------------------------------------------------------

	/**
	* Play a sound entity fading it
	* \param asName The entity name
	* \param afSpeed Volume increase per second.
	**/
	static void FadeInSoundEntity(const std::string& asName, float afSpeed)
	{
		cSoundEntity *pSound = gpScene->GetWorld3D()->GetSoundEntity(asName);
		if(pSound==NULL)
		{
			Warning("Couldn't find sound entity '%s'\n",asName.c_str());
			return;
		}

		pSound->FadeIn(afSpeed);
	}

	/**
	* Stop a sound entity fading it
	* \param asName The entity name
	* \param afSpeed Volume decrease per second.
	**/
	static void FadeOutSoundEntity(const std::string& asName, float afSpeed)
	{
		cSoundEntity *pSound = gpScene->GetWorld3D()->GetSoundEntity(asName);
		if(pSound==NULL)
		{
			Warning("Couldn't find sound entity '%s'\n",asName.c_str());
			return;
		}

		pSound->FadeOut(afSpeed);
	}

	//-----------------------------------------------------------------------

	static void PlayMusic(const std::string& asName, float afVol, float afStepSize, bool abLoop)
	{
		gpSound->GetMusicHandler()->Play(asName,afVol,afStepSize,abLoop);
	}

	//-----------------------------------------------------------------------

	static void StopMusic(float afStepSize)
	{
		gpSound->GetMusicHandler()->Stop(afStepSize);
	}

	//-----------------------------------------------------------------------

	/**
	* Play a sound gui sound, with out any position.
	* \param asName The sound name
	* \param afVol Volume of the sound
	**/
	static void PlayGuiSound(const std::string& asName, float afVol)
	{
		gpSound->GetSoundHandler()->PlayGui(asName,false,afVol);
	}

	/////////////////////////////////////////////////////////////////////////
	/////// PHYSICS //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	* Sets a callback for a joint.
	* The syntax for the function is: void MyFunction(string asJointName)
	* \param asJointName The joint name
	* \param asType The type, can be: "OnMax" or "OnMin".
	* \param asFunc The script function to be called. Must be in the current script file. "" = disabled.
	**/
	static void SetJointCallback(const std::string& asJointName, const std::string& asType,
											const std::string& asFunc)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		int lType = 0;
		tString sLowName = cString::ToLowerCase(asType);
		if(sLowName == "onmax") lType = 1;
		if(sLowName == "onmin") lType = 2;

		if(lType ==0){
			Warning("Joint callback type '%s' does not exist\n",asType.c_str()); return;
		}

		cScriptJointCallback *pCallback = static_cast<cScriptJointCallback*>(pJoint->GetCallback());
		if(pCallback==NULL)
		{
			pCallback = new cScriptJointCallback(gpScene);
			pJoint->SetCallback(pCallback,true);
		}

		if(lType==1) pCallback->msMaxFunc = asFunc;
		if(lType==2) pCallback->msMinFunc = asFunc;
	}

	//-----------------------------------------------------------------------

	/**
	* Breaks a joint.
	* \param asJointName The joint name
	**/
	static void BreakJoint(const std::string& asJointName)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		pJoint->Break();
	}

	//-----------------------------------------------------------------------

	/**
	* Sets if a joint controller is active or not.
	* \param asJointName The joint name
	* \param asCtrlName The controller name
	* \param abActive If the controller is to be active or not.
	**/
	static void SetJointControllerActive(const std::string& asJointName,const std::string& asCtrlName, bool abActive)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		iPhysicsController *pCtrl = pJoint->GetController(asCtrlName);
		if(pCtrl==NULL){
			Warning("Couldn't find controller %s in joint '%s'\n",asCtrlName.c_str(),asJointName.c_str());return;
		}

		pCtrl->SetActive(abActive);
	}

	//-----------------------------------------------------------------------

	/**
	* Change the active controller. All other controllers are set to false.
	* \param asJointName The joint name
	* \param asCtrlName The controller name
	**/
	static void ChangeJointController(const std::string& asJointName,const std::string& asCtrlName)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		if(pJoint->ChangeController(asCtrlName)==false)
		{
			Warning("Couldn't find controller %s in joint '%s'\n",asCtrlName.c_str(),asJointName.c_str());
			return;
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Sets if a joint controller is active or not.
	* \param asJointName The joint name
	* \param asCtrlName The controller name
	* \param asProperty Property to change, can be "DestValue"
	* \param afValue Value to set it to.
	**/
	static void SetJointControllerPropertyFloat(const std::string& asJointName,const std::string& asCtrlName,
													const std::string& asProperty, float afValue)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		iPhysicsController *pCtrl = pJoint->GetController(asCtrlName);
		if(pCtrl==NULL){
			Warning("Couldn't find controller %s in joint '%s'\n",asCtrlName.c_str(),asJointName.c_str());return;
		}

		if(asProperty == "DestValue")
		{
			pCtrl->SetDestValue(afValue);
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Gets a property from the joint.
	* Valid properties are:
	* "Angle" The angle between the bodies (in degrees) (Not working for ball joint)
	* "Distance" The distance between the bodies (in meter)
	* "LinearSpeed" The relative linear speed between the bodies (in m/s)
	* "AngularSpeed" The relative angular speed between the bodies (in m/s)
	* "Force" The size of the force (in newton, kg*m/s^2).
	* "MaxLimit" The max limit (meters or degrees)
	* "MinLimit" The in limit (meters or degrees)
	* \param asJointName The joint name
	* \param asProp The property to get
	**/
	static float GetJointProperty(const std::string& asJointName, const std::string& asProp)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return 0;
		}

		tString sLowProp = cString::ToLowerCase(asProp);

		if(sLowProp == "angle")
		{
			return cMath::ToDeg(pJoint->GetAngle());
		}
		else if(sLowProp == "distance")
		{
			return pJoint->GetDistance();
		}
		else if(sLowProp == "linearspeed")
		{
			return pJoint->GetVelocity().Length();
		}
		else if(sLowProp == "angularspeed")
		{
			return pJoint->GetAngularVelocity().Length();
		}
		else if(sLowProp == "force")
		{
			return pJoint->GetForceSize();
		}
		/////////////////////////////
		// Min Limit
		else if(sLowProp == "minlimit")
		{
			switch(pJoint->GetType())
			{
			case ePhysicsJointType_Hinge:
				{
					iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
					return cMath::ToDeg(pHingeJoint->GetMinAngle());
				}
			case ePhysicsJointType_Screw:
				{
					iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);
					return pScrewJoint->GetMinDistance();
				}
			case ePhysicsJointType_Slider:
				{
					iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);
					return pSliderJoint->GetMinDistance();
				}
			case ePhysicsJointType_Ball:
			case ePhysicsJointType_LastEnum:
				break;
			}
		}
		/////////////////////////////
		// Max Limit
		else if(sLowProp == "maxlimit")
		{
			switch(pJoint->GetType())
			{
			case ePhysicsJointType_Hinge:
				{
					iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
					return cMath::ToDeg(pHingeJoint->GetMaxAngle());
				}
			case ePhysicsJointType_Screw:
				{
					iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);
					return pScrewJoint->GetMaxDistance();
				}
			case ePhysicsJointType_Slider:
				{
					iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);
					return pSliderJoint->GetMaxDistance();
				}
			case ePhysicsJointType_Ball:
			case ePhysicsJointType_LastEnum:
				break;
			}

		}

		Warning("Joint property '%s' does not exist!\n",asProp.c_str());
		return 0;
	}

	//-----------------------------------------------------------------------

	/**
	* Gets a property from the body.
	* Valid properties are:
	* "Mass" The mass of the body (in kg)
	* "LinearSpeed" The linear speed the body has (in m/s)
	* "AngularSpeed" The angular speed the body has (in m/s)
	* \param asBodyName The body name
	* \param asProp The property to get
	**/
	static float GetBodyProperty(const std::string& asBodyName, const std::string& asProp)
	{
		iPhysicsBody *pBody = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asBodyName);
		if(pBody==NULL){
			Warning("Couldn't find Body '%s'\n",asBodyName.c_str());return 0;
		}

		tString sLowProp = cString::ToLowerCase(asProp);

		if(sLowProp == "mass")
		{
			return pBody->GetMass();
		}
		else if(sLowProp == "linearspeed")
		{
			return pBody->GetLinearVelocity().Length();
		}
		else if(sLowProp == "angularspeed")
		{
			return pBody->GetAngularVelocity().Length();
		}

		Warning("Body property '%s' does not exist!\n",asProp.c_str());
		return 0;
	}

	//-----------------------------------------------------------------------

	/**
	* Sets a property to the body.
	* Valid properties are:
	* "Mass" The mass of the body (in kg)
	* "CollideCharacter"	0 = false 1=true
	* \param asBodyName The body name
	* \param asProp The property to get
	* \param afVal The new value of the property
	**/
	static void SetBodyProperty(const std::string& asBodyName, const std::string& asProp, float afVal)
	{
		iPhysicsBody *pBody = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asBodyName);
		if(pBody==NULL){
			Warning("Couldn't find Body '%s'\n",asBodyName.c_str());return;
		}

		tString sLowProp = cString::ToLowerCase(asProp);

		if(sLowProp == "mass")
		{
			pBody->SetMass(afVal);
			pBody->SetEnabled(true);

			if(afVal == 0)
			{
				pBody->SetLinearVelocity(0);
				pBody->SetAngularVelocity(0);
			}

			return;
		}
		else if(sLowProp == "collidecharacter")
		{
			pBody->SetCollideCharacter(afVal <0.05 ? false : true);
			return;
		}
		else if(sLowProp == "hasgravity")
		{
			pBody->SetCollideCharacter(afVal <0.05 ? false : true);
			return;
		}


		Warning("Body property '%s' does not exist!\n",asProp.c_str());
	}

	//-----------------------------------------------------------------------

	static void AttachBodiesWithJoint(const std::string& asParentName, const std::string& asChildName, const std::string& asJointName)
	{
		iPhysicsBody *pParent = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asParentName);
		if(pParent==NULL){
			Warning("Couldn't find Body '%s'\n",asParentName.c_str());return;
		}

		iPhysicsBody *pChild = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asChildName);
		if(pChild==NULL){
			Warning("Couldn't find Body '%s'\n",asChildName.c_str());return;
		}

		iPhysicsWorld *pPhysicsWorld = gpScene->GetWorld3D()->GetPhysicsWorld();

		cVector3f vPivot = (pParent->GetLocalPosition() + pChild->GetLocalPosition()) * 0.5f;
		cVector3f vDir = cMath::Vector3Normalize(pChild->GetLocalPosition() - pParent->GetLocalPosition());

		iPhysicsJointSlider *pJoint = pPhysicsWorld->CreateJointSlider(asJointName,vPivot,vDir,pParent,pChild);

		pJoint->SetMinDistance(-0.01f);
		pJoint->SetMaxDistance(0.01f);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets a property to the joint.
	* Valid properties are:
	* "MinLimit" The min limit (depends on joint, does not work on ball)
	* "MaxLimit" The max limit (depends on joint, does not work on ball)
	* \param asJointName The body name
	* \param asProp The property to get
	* \param afVal The new value of the property
	**/
	static void SetJointProperty(const std::string& asJointName, const std::string& asProp, float afVal)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		tString sLowProp = cString::ToLowerCase(asProp);

		if(pJoint->GetChildBody()) pJoint->GetChildBody()->SetEnabled(true);
		if(pJoint->GetParentBody()) pJoint->GetParentBody()->SetEnabled(true);

		/////////////////////////////
		// Min Limit
		if(sLowProp == "minlimit")
		{
			switch(pJoint->GetType())
			{
			case ePhysicsJointType_Hinge:
				{
					iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
					pHingeJoint->SetMinAngle(cMath::ToRad(afVal));
					break;
				}
			case ePhysicsJointType_Screw:
				{
					iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);
					pScrewJoint->SetMinDistance(afVal);
					break;
				}
			case ePhysicsJointType_Slider:
				{
					iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);
					pSliderJoint->SetMinDistance(afVal);
					break;
				}
			case ePhysicsJointType_Ball:
			case ePhysicsJointType_LastEnum:
				break;
			}
		}
		/////////////////////////////
		// Max Limit
		else if(sLowProp == "maxlimit")
		{
			switch(pJoint->GetType())
			{
			case ePhysicsJointType_Hinge:
				{
					iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
					pHingeJoint->SetMaxAngle(cMath::ToRad(afVal));
					break;
				}
			case ePhysicsJointType_Screw:
				{
					iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);
					pScrewJoint->SetMaxDistance(afVal);
					break;
				}
			case ePhysicsJointType_Slider:
				{
					iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);
					pSliderJoint->SetMaxDistance(afVal);
					break;
				}
			case ePhysicsJointType_Ball:
			case ePhysicsJointType_LastEnum:
				break;
			}
		}
		else
		{
			Warning("Joint property '%s' does not exist!\n",asProp.c_str());
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Adds a force to the body. This can either be in the bodies local coord system or the world's.
	* \param asBodyName The body name
	* \param asCoordType The coordinate system type. "World" or "Local".
	* \param afX force in the x direction. (in newton, kg*m/s^2)
	* \param afY force in the y direction. (in newton, kg*m/s^2)
	* \param afZ force in the z direction. (in newton, kg*m/s^2)
	**/
	static void AddBodyForce(const std::string& asBodyName, const std::string& asCoordType,
										float afX, float afY, float afZ)
	{
		iPhysicsBody *pBody = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asBodyName);
		if(pBody==NULL){
			Warning("Couldn't find Body '%s'\n",asBodyName.c_str());return;
		}

		int lType =0;
		tString sLowType = cString::ToLowerCase(asCoordType);

		if(sLowType == "world") lType = 1;
		else if(sLowType == "local") lType =2;

		if(lType==0){
			Warning("Coord system type '%s' is not valid.\n",asCoordType.c_str());
			return;
		}

		if(lType==1)
		{
			pBody->AddForce(cVector3f(afX,afY,afZ));
		}
		else if(lType==2)
		{
			cVector3f vWorldForce = cMath::MatrixMul(pBody->GetLocalMatrix().GetRotation(),
														cVector3f(afX,afY,afZ));
			pBody->AddForce(vWorldForce);
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Adds an impule (a change in velocity) to the body. This can either be in the bodies local coord system or the world's.
	* \param asBodyName The body name
	* \param asCoordType The coordinate system type. "World" or "Local".
	* \param afX velocity in the x direction. (in m/s)
	* \param afY velocity in the y direction. (in m/s)
	* \param afZ velocity in the z direction. (in m/s)
	**/
	static void AddBodyImpulse(const std::string& asBodyName, const std::string& asCoordType,
		float afX, float afY, float afZ)
	{
		iPhysicsBody *pBody = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asBodyName);
		if(pBody==NULL){
			Warning("Couldn't find Body '%s'\n",asBodyName.c_str());return;
		}

		int lType =0;
		tString sLowType = cString::ToLowerCase(asCoordType);

		if(sLowType == "world") lType = 1;
		else if(sLowType == "local") lType =2;

		if(lType==0){
			Warning("Coord system type '%s' is not valid.\n",asCoordType.c_str());
			return;
		}

		if(lType==1)
		{
			pBody->AddImpulse(cVector3f(afX,afY,afZ));
		}
		else if(lType==2)
		{
			cVector3f vWorldForce = cMath::MatrixMul(pBody->GetLocalMatrix().GetRotation(),
				cVector3f(afX,afY,afZ));
			pBody->AddImpulse(vWorldForce);
		}
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// LOCAL VARS //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	static void CreateLocalVar(const std::string& asName, int alVal)
	{
		if(gpScene->GetLocalVar(asName)==NULL)
		{
			cScriptVar* pVar = gpScene->CreateLocalVar(asName);
			pVar->mlVal = alVal;
		}
	}

	static void SetLocalVar(const std::string& asName, int alVal)
	{
		cScriptVar* pVar = gpScene->CreateLocalVar(asName);
		pVar->mlVal = alVal;
	}

	static void AddLocalVar(const std::string& asName, int alVal)
	{
		cScriptVar* pVar = gpScene->CreateLocalVar(asName);
		pVar->mlVal += alVal;
	}

	static int GetLocalVar(const std::string& asName)
	{
		cScriptVar* pVar = gpScene->GetLocalVar(asName);
		if(pVar==NULL)
		{
			Error("Couldn't find local var '%s'\n",asName.c_str());
			return 0;
		}
		return pVar->mlVal;
	}

	//-----------------------------------------------------------------------
	/////////////////////////////////////////////////////////////////////////
	/////// GLOBAL VARS //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	static void CreateGlobalVar(const std::string& asName, int alVal)
	{
		if(gpScene->GetGlobalVar(asName)==NULL)
		{
			cScriptVar* pVar = gpScene->CreateGlobalVar(asName);
			pVar->mlVal = alVal;
		}
	}

	static void SetGlobalVar(const std::string& asName, int alVal)
	{
		cScriptVar* pVar = gpScene->CreateGlobalVar(asName);
		pVar->mlVal = alVal;
	}


	static void AddGlobalVar(const std::string& asName, int alVal)
	{
		cScriptVar* pVar = gpScene->CreateGlobalVar(asName);
		pVar->mlVal += alVal;
	}


	static int GetGlobalVar(const std::string& asName)
	{
		cScriptVar* pVar = gpScene->GetGlobalVar(asName);
		if(pVar==NULL)
		{
			Error("Couldn't find global var '%s'\n",asName.c_str());
			return 0;
		}
		return pVar->mlVal;
	}

	//-----------------------------------------------------------------------

	void RegisterCoreFunctions(
		cScript* apScript,
		cGraphics* apGraphics,
		cResources *apResources,
		cInput *apInput,
		cScene *apScene,
		cSound *apSound,
		cGame *apGame
	)
	{
		gpGraphics = apGraphics;
		gpResources = apResources;
		gpInput = apInput;
		gpScene = apScene;
		gpSound = apSound;
		gpGame = apGame;
		
#define AddFunc(nm, fn) apScript->GetEngine()->RegisterGlobalFunction(nm, WRAP_FN(fn), asCALL_GENERIC);

		//General
		AddFunc("void Print(const string &in asText)", Print);
		AddFunc("string FloatToString(float afX)", FloatToString);
		AddFunc("string IntToString(int aiX)", IntToString);
		AddFunc("float RandFloat(float afMin, float afMax)", RandFloat);
		AddFunc("int RandInt(int aiMin, int aiMax)", RandInt);
		AddFunc("bool StringContains(const string &in asHaystack, const string &in asNeedle)", StringContains);
		AddFunc("void ResetLogicTimer()", ResetLogicTimer);

		// Renderer
		AddFunc("void SetAmbientColor(float afR, float afG, float afB)", SetAmbientColor);
		AddFunc("void SetSkyboxActive(bool abX)", SetSkyboxActive);
		AddFunc("void SetSkyboxColor(float afR, float afG, float afB, float afA)", SetSkyboxColor);
		AddFunc("void SetSkybox(const string &in asTexture)", SetSkybox);
		AddFunc("void CreateParticleSystemOnCamera(const string &in asName,const string &in asType)", CreateParticleSystemOnCamera);
		AddFunc("void SetFogActive(bool abX)", SetFogActive);
		AddFunc("void SetFogCulling(bool abX)", SetFogCulling);
		AddFunc("void SetFogProperties(float afStart, float afEnd, float afR,float afG, float afB)", SetFogProperties);
		AddFunc("void SetSectorProperties(const string &in asSector, float afAmbR, float afAmbG, float afAmbB)", SetSectorProperties);
		AddFunc("void SetSectorPortalActive(const string &in asSector, int alPortal, bool abActive)", SetSectorPortalActive);

		// Resources
		AddFunc("void PreloadSound(const string &in asFile)", PreloadSound);
		AddFunc("string Translate(const string &in asCat, const string &in asName)", Translate);

		// Mesh Entity
		AddFunc("void SetMeshActive(const string &in asName, bool abActive)", SetMeshActive);

		// Particle System
		AddFunc("void SetParticleSystemActive(const string &in asName, bool abActive)", SetParticleSystemActive);
		AddFunc("void CreateParticleSystem(const string &in asName, const string &in asType, const string &in asArea, float afX, float afY, float afZ)", CreateParticleSystem);
		AddFunc("void KillParticleSystem(const string &in asName)", KillParticleSystem);

		// Beam
		AddFunc("void CreateBeam(const string &in asName, const string &in asFile, const string &in asStartArea, const string &in asEndArea)", CreateBeam);
		AddFunc("void DestroyBeam(const string &in asName)", DestroyBeam);

		// Light
		AddFunc("void FadeLight3D(const string &in asName, float afR,float afG, float afB, float afA, float afRadius, float afTime)", FadeLight3D);
		AddFunc("void AttachBillboardToLight3D(const string &in asBillboardName, const string &in asLightName,bool abX)", AttachBillboardToLight3D);
		AddFunc("void SetLight3DVisible(const string &in asName, bool abX)", SetLight3DVisible);
		AddFunc("void SetLight3DOnlyAffectInSector(const string &in asName, bool abX)", SetLight3DOnlyAffectInSector);
		AddFunc("void SetLight3DFlickerActive(const string &in asName, bool abX)", SetLight3DFlickerActive);
		AddFunc("void SetLight3DFlicker(const string &in asName, float afR,float afG, float afB, float afA, float afRadius, float afOnMinLength, \
				float afOnMaxLength, const string &in asOnSound, const string &in asOnPS, float afOffMinLength, float afOffMaxLength, const string &in asOffSound, \
				const string &in asOffPS, bool abFade, float afOnFadeLength, float afOffFadeLength)", SetLight3DFlicker);

		// Sound
		AddFunc("void CreateSoundEntity(const string &in asName, const string &in asFile, const string &in asArea)", CreateSoundEntity);
		AddFunc("void PlaySoundEntity(const string &in asName, bool abPlayStart)", PlaySoundEntity);
		AddFunc("void StopSoundEntity(const string &in asName, bool abPlayEnd)", StopSoundEntity);
		AddFunc("void FadeInSoundEntity(const string &in asName, float afSpeed)", FadeInSoundEntity);
		AddFunc("void FadeOutSoundEntity(const string &in asName, float afSpeed)", FadeOutSoundEntity);
		AddFunc("void PlayMusic(const string &in asName, float afVol, float afStepSize, bool abLoop)", PlayMusic);
		AddFunc("void StopMusic(float afStepSize)", StopMusic);
		AddFunc("void PlayGuiSound(const string &in asName, float afVol)", PlayGuiSound);

		// Physics
		AddFunc("void SetJointCallback(const string &in asJointName, const string &in asType, const string &in asFunc)", SetJointCallback);
		AddFunc("float GetJointProperty(const string &in asJointName, const string &in asProp)", GetJointProperty);
		AddFunc("float GetBodyProperty(const string &in asBodyName, const string &in asProp)", GetBodyProperty);
		AddFunc("void BreakJoint(const string &in asJointName)", BreakJoint);
		AddFunc("void SetJointControllerActive(const string &in asJointName,const string &in asCtrlName, bool abActive)", SetJointControllerActive);
		AddFunc("void ChangeJointController(const string &in asJointName,const string &in asCtrlName)", ChangeJointController);
		AddFunc("void SetJointControllerPropertyFloat(const string &in asJointName,const string &in asCtrlName, const string &in asProperty, float afValue)", SetJointControllerPropertyFloat);
		AddFunc("void SetBodyProperty(const string &in asBodyName, const string &in asProp, float afVal)", SetBodyProperty);
		AddFunc("void AttachBodiesWithJoint(const string &in asParentName, const string &in asChildName, const string &in asJointName)", AttachBodiesWithJoint);
		AddFunc("void SetJointProperty(const string &in asJointName, const string &in asProp, float afVal)", SetJointProperty);
		AddFunc("void AddBodyForce(const string &in asBodyName, const string &in asCoordType, float afX, float afY, float afZ)", AddBodyForce);
		AddFunc("void AddBodyImpulse(const string &in asBodyName, const string &in asCoordType, float afX, float afY, float afZ)", AddBodyImpulse);

		// Local Vars
		AddFunc("void CreateLocalVar(const string &in asName, int alVal)", CreateLocalVar);
		AddFunc("void SetLocalVar(const string &in asName, int alVal)", SetLocalVar);
		AddFunc("void AddLocalVar(const string &in asName, int alVal)", AddLocalVar);
		AddFunc("int GetLocalVar(const string &in asName)", GetLocalVar);

		// Global Vars
		AddFunc("void CreateGlobalVar(const string &in asName, int alVal)", CreateGlobalVar);
		AddFunc("void SetGlobalVar(const string &in asName, int alVal)", SetGlobalVar);
		AddFunc("void AddGlobalVar(const string &in asName, int alVal)", AddGlobalVar);
		AddFunc("int GetGlobalVar(const string &in asName)", GetGlobalVar);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------



	//-----------------------------------------------------------------------

}
