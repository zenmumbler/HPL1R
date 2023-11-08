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
#include "scene/Light3D.h"

#include "tinyXML/tinyxml.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/Renderer3D.h"
#include "graphics/Mesh.h"
#include "graphics/RenderList.h"
#include "scene/Camera.h"
#include "math/Math.h"
#include "scene/MeshEntity.h"
#include "graphics/SubMesh.h"
#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/FileSearcher.h"
#include "graphics/ParticleSystem3D.h"
#include "scene/World3D.h"
#include "graphics/BillBoard.h"
#include "scene/SectorVisibility.h"
#include "scene/PortalContainer.h"
#include "system/Log.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iLight3D::iLight3D(tString asName, cResources *apResources) : iLight(), iRenderable(asName)
	{
		mbStaticCasterAdded = false;

		mbOnlyAffectInInSector = false;

		mbApplyTransformToBV = false;

		mpTextureManager = apResources->GetTextureManager();
		mpFileSearcher = apResources->GetFileSearcher();

		mpFalloffMap = mpTextureManager->Create1D("core_falloff_linear",false);
		mpFalloffMap->SetWrapS(eTextureWrap_ClampToEdge);
		mpFalloffMap->SetWrapT(eTextureWrap_ClampToEdge);

		mpVisSectorCont = NULL;
		mlSectorVisibilityCount =-1;

		for(int i=0; i<3;++i)mvTempTextures[i]=NULL;
	}

	//-----------------------------------------------------------------------

	iLight3D::~iLight3D()
	{
		mpTextureManager->Destroy(mpFalloffMap);

		if(mpVisSectorCont) delete mpVisSectorCont;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iLight3D::SetVisible(bool abVisible)
	{
		SetRendered(abVisible);

		for(size_t i =0; i<mvBillboards.size(); ++i)
		{
			mvBillboards[i]->SetVisible(abVisible);
		}
	}

	//-----------------------------------------------------------------------


	void iLight3D::AddShadowCaster(iRenderable* apObject, cFrustum* apFrustum, bool abStatic,cRenderList *apRenderList)
	{
		//Log("Testing: %s\n",apObject->GetName().c_str());
		/////////////////////////////////////////////////////
		// Check if the object should be added
		eRenderableType renderType = apObject->GetRenderType();

		//Is it affected by the light.
		if(GetOnlyAffectInSector() && apObject->IsInSector(GetCurrentSector())==false) return;

		//Is it visible
		if(apObject->GetForceShadow()==false)
		{
			if(apObject->IsVisible()==false) return;
			//Does the object cast shadows?
			if(apObject->IsShadowCaster()==false) return;

			//Can be material cast shadows?
			iMaterial *pMaterial = apObject->GetMaterial();
			if(pMaterial){
				if(pMaterial->IsTransperant() || pMaterial->HasAlpha()) return;
			}

			//Check so that the object is the right type
			if(renderType != eRenderableType_Mesh && renderType != eRenderableType_Normal) {
				return;
			}
		}

		//Log("Right type!\n");

		//Check if the object has all ready been added
		if(abStatic)
		{
			if(m_setStaticCasters.find(apObject) != m_setStaticCasters.end()) return;
		}
		else
		{
			if(m_setDynamicCasters.find(apObject) != m_setDynamicCasters.end()) return;
		}

		//Log("Not in list!\n");

		//Check if the object touches the light.
		//if(CollidesWithBV(apObject->GetBoundingVolume())==false){
		//	return;
		//}

		if(CheckObjectIntersection(apObject)==false) return;

		//Log("Collides!\n");

		//Log("Shadow is in frustum!\n");

		////////////////////////////////////////////////////
		// All checks passed, add the object!
		if(renderType == eRenderableType_Mesh)
		{
			cMeshEntity* pMesh = static_cast<cMeshEntity*>(apObject);

			///Need to add shadow casted objects else shadows might get choppy.
			if(abStatic == false){
				pMesh->SetGlobalRenderCount(cRenderList::GetGlobalRenderCount());
			}

			for(int i=0;i<pMesh->GetSubMeshEntityNum();i++)
			{
				cSubMeshEntity *pSub = pMesh->GetSubMeshEntity(i);

				if(pSub->IsVisible()==false) continue;
				if(apObject->GetForceShadow()==false)
				{
					iMaterial *pMaterial = pSub->GetMaterial();
					if(pMaterial){
						if(pMaterial->IsTransperant() || pMaterial->HasAlpha()) continue;
					}
				}

				if(abStatic)
				{
					m_setStaticCasters.insert(pSub);
				}
				else
				{
					m_setDynamicCasters.insert(pSub);
					///Need to add shadow casted objects else shadows might get choppy.
					pSub->SetGlobalRenderCount(cRenderList::GetGlobalRenderCount());
				}
			}
		}
		else
		{
			if(abStatic)
			{
				m_setStaticCasters.insert(apObject);
			}
			else
			{
				m_setDynamicCasters.insert(apObject);
				apObject->SetGlobalRenderCount(cRenderList::GetGlobalRenderCount());

			}
		}

	}

	bool iLight3D::HasStaticCasters()
	{
		return m_setStaticCasters.empty() ? false: true;
	}

	void iLight3D::ClearCasters(bool abClearStatic)
	{
		if(abClearStatic) m_setStaticCasters.clear();
		m_setDynamicCasters.clear();
	}

	//-----------------------------------------------------------------------

	bool iLight3D::CheckObjectIntersection(iRenderable *apObject)
	{
		//Log("------ Checking %s with light %s -----\n",apObject->GetName().c_str(), GetName().c_str());
		//Log(" BV: min: %s max: %s\n",	apObject->GetBoundingVolume()->GetMin().ToString().c_str(),
		//								apObject->GetBoundingVolume()->GetMax().ToString().c_str());

		//////////////////////////////////////////////////////////////
		// If the lights cast shadows, cull objects that are in shadow
		if(mbCastShadows)
		{
			/////////////////////////////////////
			//Check if the visibility needs update
			if(mlSectorVisibilityCount != GetMatrixUpdateCount())
			{
				mlSectorVisibilityCount = GetMatrixUpdateCount();
				if(mpVisSectorCont) delete mpVisSectorCont;

				mpVisSectorCont = CreateSectorVisibility();
				//Log("Creating Visibility container!\n");
			}


			// Get the data list containing the sectors the object is connected to
			tRenderContainerDataList *pDataList = apObject->GetRenderContainerDataList();

			//It is not attached to any room. Just use BV test.
			if(pDataList->empty())
			{
				//Log("Empty data list using BV\n");
				return CollidesWithBV(apObject->GetBoundingVolume());
			}
			//The object is in one or more sectors
			else
			{
				//Iterate the sectors and remove the object from them.
				tRenderContainerDataListIt it = pDataList->begin();
				for(; it != pDataList->end(); ++it)
				{
					cSector* pSector = static_cast<cSector*>(*it);

					//Log("Checking intersection in sector %s\n",pSector->GetId().c_str());

					cSectorVisibility *pVisSector = mpVisSectorCont->GetSectorVisibilty(pSector);
					if(pVisSector)
					{
						if(pVisSector->IntersectionBV(apObject->GetBoundingVolume()))
						{
							//Log("Intersected!\n");
							//Log("-----------------------");
							return true;
						}
					}
				}

				//Log("-----------------------");
				return false;
			}


		}
		/////////////////////////////////////////////////
		//Light is not in shadow, do not do any culling
		else
		{
			//Log("No shadow, using BV\n");
			return CollidesWithBV(apObject->GetBoundingVolume());
		}


	}

	//-----------------------------------------------------------------------

	bool iLight3D::BeginDraw(cRenderSettings *apRenderSettings,iLowLevelGraphics *apLowLevelGraphics)
	{
		cRect2l ClipRect;
		bool bVisible = CreateClipRect(ClipRect, apRenderSettings,apLowLevelGraphics);

		if(bVisible)
		{
			apLowLevelGraphics->SetScissorActive(true);
			apLowLevelGraphics->SetScissorRect(ClipRect);

			if(apRenderSettings->mbLog)
				Log("Cliprect pos: (%d, %d) size: (%d, %d)\n",ClipRect.x, ClipRect.y, ClipRect.w, ClipRect.h);
		}
		else
		{
			if(apRenderSettings->mbLog)
				Log("Cliprect entire screen\n");
		}

		//////////////////////////////////////////////////////////
		// Cast shadows
		if(mbCastShadows && apRenderSettings->mShowShadows != eRendererShowShadows_None
			&& apRenderSettings->mpExtrudeProgram != NULL)
		{
			//Get temp index array. (Remove this when the index pool
			// is implemented.).
			mpIndexArray = apRenderSettings->mpTempIndexArray;

			//Setup for shadow drawing
			apLowLevelGraphics->SetStencilActive(true);
			//Do no set this when debugging.
			apLowLevelGraphics->SetColorWriteActive(false, false, false,false);

			///////////////////////////////////////////////////////////////////////////
			//Clear stencil, since scissor is set this should be fast.
			apLowLevelGraphics->SetClearStencilActive(true);
			apLowLevelGraphics->SetClearDepthActive(false);
			apLowLevelGraphics->SetClearColorActive(false);

			apLowLevelGraphics->SetClearStencil(0);

			apLowLevelGraphics->ClearScreen();

			apLowLevelGraphics->SetClearStencilActive(false);
			apLowLevelGraphics->SetClearDepthActive(true);
			apLowLevelGraphics->SetClearColorActive(true);

			apLowLevelGraphics->SetCullActive(false);

			apLowLevelGraphics->SetDepthWriteActive(false);

			//Setup the depth test so that shadow volume is not rendered in front
			//off the normal graphics.
			apLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc_Less);

			//Resert the algo (zfail or zpass) used.
			apRenderSettings->mlLastShadowAlgo=0;

			//Reset this variable so it can be used when rendering shadows.
			apRenderSettings->mbMatrixWasNULL = false;

			//Set the program.
			if(apRenderSettings->mbLog)Log("Setting program: '%s'\n",
											apRenderSettings->mpExtrudeProgram->GetName().c_str());
			apRenderSettings->mpExtrudeProgram->Bind();
			apRenderSettings->mpProgram = apRenderSettings->mpExtrudeProgram;

			//Render shadows
			tCasterCacheSetIt it = m_setDynamicCasters.begin();

			if(apRenderSettings->mShowShadows == eRendererShowShadows_All)
			{
				it = m_setDynamicCasters.begin();
				for(; it!= m_setDynamicCasters.end(); ++it)
				{
					RenderShadow(*it,apRenderSettings,apLowLevelGraphics);
				}
			}

			it = m_setStaticCasters.begin();
			for(; it!= m_setStaticCasters.end(); ++it)
			{
				RenderShadow(*it,apRenderSettings,apLowLevelGraphics);
			}

			//Make rendering ready for the objects.
			apLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc_Equal);

			apLowLevelGraphics->SetColorWriteActive(true, true, true,true);
			apLowLevelGraphics->SetCullActive(true);

			apLowLevelGraphics->SetStencil(eStencilFunc_Equal,0,0xFF,
											eStencilOp_Keep,eStencilOp_Keep, eStencilOp_Keep);
		}

		//Reset this var so that the new light properties are set.
		apRenderSettings->mbMatrixWasNULL = false;

		return true;
	}

	//-----------------------------------------------------------------------

	void iLight3D::EndDraw(cRenderSettings *apRenderSettings,iLowLevelGraphics *apLowLevelGraphics)
	{
		apLowLevelGraphics->SetScissorActive(false);
		apLowLevelGraphics->SetStencilActive(false);
	}

	//-----------------------------------------------------------------------

	void iLight3D::SetFarAttenuation(float afX)
	{
		mfFarAttenuation = afX;

		mbUpdateBoundingVolume = true;

		//This is so that the render container is updated.
		SetTransformUpdated();
	}
	//-----------------------------------------------------------------------

	void iLight3D::SetNearAttenuation(float afX)
	{
		mfNearAttenuation = afX;
		if(mfNearAttenuation>mfFarAttenuation)
			SetFarAttenuation(mfNearAttenuation);
	}
	//-----------------------------------------------------------------------

	cVector3f iLight3D::GetLightPosition()
	{
		return GetWorldPosition();
	}

	//-----------------------------------------------------------------------

	void iLight3D::UpdateLogic(float afTimeStep)
	{
		UpdateLight(afTimeStep);
		if(mfFadeTime>0 || mbFlickering)
		{
			mbUpdateBoundingVolume = true;

			//This is so that the render container is updated.
			SetTransformUpdated();
		}
	}

	//-----------------------------------------------------------------------

	cBoundingVolume* iLight3D::GetBoundingVolume()
	{
		if(mbUpdateBoundingVolume)
		{
			UpdateBoundingVolume();
			mbUpdateBoundingVolume = false;
		}

		return &mBoundingVolume;
	}

	//-----------------------------------------------------------------------

	cMatrixf* iLight3D::GetModelMatrix(cCamera *apCamera)
	{
		mtxTemp = GetWorldMatrix();
		return &mtxTemp;
	}

	//-----------------------------------------------------------------------

	bool iLight3D::IsVisible()
	{
		if(mDiffuseColor.r <=0 && mDiffuseColor.g <=0 && mDiffuseColor.b <=0 && mDiffuseColor.a <=0)
			return false;
		if(mfFarAttenuation <= 0) return false;

		return IsRendered();
	}

	//-----------------------------------------------------------------------

	iTexture *iLight3D::GetFalloffMap()
	{
		return mpFalloffMap;
	}

	void iLight3D::SetFalloffMap(iTexture* apTexture)
	{
		if(mpFalloffMap) mpTextureManager->Destroy(mpFalloffMap);

		mpFalloffMap = apTexture;
		mpFalloffMap->SetWrapS(eTextureWrap_ClampToEdge);
		mpFalloffMap->SetWrapT(eTextureWrap_ClampToEdge);

		//reset temp textures.
		for(int i=0; i<3;++i)mvTempTextures[i]=NULL;
	}

	//-----------------------------------------------------------------------

	void iLight3D::LoadXMLProperties(const tString asFile)
	{
		tString sPath = mpFileSearcher->GetFilePath(asFile);
		if(sPath != "")
		{
			TiXmlDocument *pDoc = new TiXmlDocument(sPath.c_str());
			if(pDoc->LoadFile())
			{
				TiXmlElement *pRootElem = pDoc->RootElement();

				TiXmlElement *pMainElem = pRootElem->FirstChildElement("MAIN");
				if(pMainElem!=NULL)
				{
					mbCastShadows = cString::ToBool(pMainElem->Attribute("CastsShadows"),mbCastShadows);

					mDiffuseColor.a = cString::ToFloat(pMainElem->Attribute("Specular"),mDiffuseColor.a);

					tString sFalloffImage = cString::ToString(pMainElem->Attribute("FalloffImage"),"");
					iTexture *pTexture = mpTextureManager->Create1D(sFalloffImage,false);
					if(pTexture) SetFalloffMap(pTexture);

					ExtraXMLProperties(pMainElem);
				}
				else
				{
					Error("Cannot find main element in %s\n",asFile.c_str());
				}
			}
			else
			{
				Error("Couldn't load file '%s'\n",asFile.c_str());
			}
			delete pDoc;
		}
		else
		{
			Error("Couldn't find file '%s'\n",asFile.c_str());
		}

	}

	//-----------------------------------------------------------------------


	void iLight3D::AttachBillboard(cBillboard *apBillboard)
	{
		mvBillboards.push_back(apBillboard);
		apBillboard->SetColor(cColor(mDiffuseColor.r,mDiffuseColor.g,mDiffuseColor.b,1));
		apBillboard->SetVisible(IsVisible());
	}

	void iLight3D::RemoveBillboard(cBillboard *apBillboard)
	{
		std::vector<cBillboard*>::iterator it = mvBillboards.begin();
		for(; it != mvBillboards.end(); ++it)
		{
			if(*it == apBillboard)
			{
				mvBillboards.erase(it);
			}
		}
	}

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iLight3D::RenderShadow(iRenderable *apObject,cRenderSettings *apRenderSettings,
				iLowLevelGraphics *apLowLevelGraphics)
	{
#pragma unused(apObject)
#pragma unused(apRenderSettings)
#pragma unused(apLowLevelGraphics)
	}


	//-----------------------------------------------------------------------

	void iLight3D::OnFlickerOff()
	{
		//Particle system
		if(msFlickerOffPS!=""){
			cParticleSystem3D *pPS = mpWorld3D->CreateParticleSystem(GetName() + "_PS",
									msFlickerOffPS, cVector3f(1,1,1),GetWorldMatrix());
		}
	}

	//-----------------------------------------------------------------------

	void iLight3D::OnFlickerOn()
	{
		//Particle system
		if(msFlickerOnPS!=""){
			cParticleSystem3D *pPS = mpWorld3D->CreateParticleSystem(GetName() + "_PS",
										msFlickerOnPS, cVector3f(1,1,1),GetWorldMatrix());
		}

	}

	//-----------------------------------------------------------------------

	void iLight3D::OnSetDiffuse()
	{
		for(size_t i =0; i<mvBillboards.size(); ++i)
		{
			cBillboard *pBill = mvBillboards[i];
			pBill->SetColor(cColor(mDiffuseColor.r,mDiffuseColor.g,mDiffuseColor.b,1));
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	kBeginSerializeVirtual(cSaveData_iLight3D,cSaveData_iRenderable)
	kSerializeVar(msFalloffMap, eSerializeType_String)
	kSerializeVarContainer(mlstBillboardIds, eSerializeType_Int32)

	kSerializeVar(mDiffuseColor, eSerializeType_Color)
	kSerializeVar(mSpecularColor, eSerializeType_Color)
	kSerializeVar(mfIntensity, eSerializeType_Float32)
	kSerializeVar(mfFarAttenuation, eSerializeType_Float32)
	kSerializeVar(mfNearAttenuation, eSerializeType_Float32)
	kSerializeVar(mfSourceRadius, eSerializeType_Float32)

	kSerializeVar(mbCastShadows, eSerializeType_Bool)
	kSerializeVar(mbAffectMaterial, eSerializeType_Bool)

	kSerializeVar(mColAdd, eSerializeType_Color)
	kSerializeVar(mfRadiusAdd, eSerializeType_Float32)
	kSerializeVar(mDestCol, eSerializeType_Color)
	kSerializeVar(mfDestRadius, eSerializeType_Float32)
	kSerializeVar(mfFadeTime, eSerializeType_Float32)

	kSerializeVar(mbFlickering, eSerializeType_Bool)
	kSerializeVar(msFlickerOffSound, eSerializeType_String)
	kSerializeVar(msFlickerOnSound, eSerializeType_String)
	kSerializeVar(msFlickerOffPS, eSerializeType_String)
	kSerializeVar(msFlickerOnPS, eSerializeType_String)
	kSerializeVar(mfFlickerOnMinLength, eSerializeType_Float32)
	kSerializeVar(mfFlickerOffMinLength, eSerializeType_Float32)
	kSerializeVar(mfFlickerOnMaxLength, eSerializeType_Float32)
	kSerializeVar(mfFlickerOffMaxLength, eSerializeType_Float32)
	kSerializeVar(mFlickerOffColor, eSerializeType_Color)
	kSerializeVar(mfFlickerOffRadius, eSerializeType_Float32)
	kSerializeVar(mbFlickerFade, eSerializeType_Bool)
	kSerializeVar(mfFlickerOnFadeLength, eSerializeType_Float32)
	kSerializeVar(mfFlickerOffFadeLength, eSerializeType_Float32)

	kSerializeVar(mFlickerOnColor, eSerializeType_Color)
	kSerializeVar(mfFlickerOnRadius, eSerializeType_Float32)

	kSerializeVar(mbFlickerOn, eSerializeType_Bool)
	kSerializeVar(mfFlickerTime, eSerializeType_Float32)
	kSerializeVar(mfFlickerStateLength, eSerializeType_Float32)
	kEndSerialize()

	//-----------------------------------------------------------------------

	iSaveData* iLight3D::CreateSaveData()
	{
		return NULL;
	}

	//-----------------------------------------------------------------------

	void iLight3D::SaveToSaveData(iSaveData *apSaveData)
	{
		kSaveData_SaveToBegin(iLight3D);

		//////////////////////////
		// Data
		pData->msFalloffMap = mpFalloffMap==NULL ? "" : mpFalloffMap->GetName();

		pData->mlstBillboardIds.Clear();
		for(size_t i=0; i< mvBillboards.size(); ++i)
		{
			pData->mlstBillboardIds.Add(mvBillboards[i]->GetSaveObjectId());
		}

		//////////////////////////
		// Variables
		kSaveData_SaveTo(mDiffuseColor);
		kSaveData_SaveTo(mSpecularColor);
		kSaveData_SaveTo(mfIntensity);
		kSaveData_SaveTo(mfFarAttenuation);
		kSaveData_SaveTo(mfNearAttenuation);
		kSaveData_SaveTo(mfSourceRadius);

		kSaveData_SaveTo(mbCastShadows);
		kSaveData_SaveTo(mbAffectMaterial);

		kSaveData_SaveTo(mColAdd);
		kSaveData_SaveTo(mfRadiusAdd);
		kSaveData_SaveTo(mDestCol);
		kSaveData_SaveTo(mfDestRadius);
		kSaveData_SaveTo(mfFadeTime);

		kSaveData_SaveTo(mbFlickering);
		kSaveData_SaveTo(msFlickerOffSound);
		kSaveData_SaveTo(msFlickerOnSound);
		kSaveData_SaveTo(msFlickerOffPS);
		kSaveData_SaveTo(msFlickerOnPS);
		kSaveData_SaveTo(mfFlickerOnMinLength);
		kSaveData_SaveTo(mfFlickerOffMinLength);
		kSaveData_SaveTo(mfFlickerOnMaxLength);
		kSaveData_SaveTo(mfFlickerOffMaxLength);
		kSaveData_SaveTo(mFlickerOffColor);
		kSaveData_SaveTo(mfFlickerOffRadius);
		kSaveData_SaveTo(mbFlickerFade);
		kSaveData_SaveTo(mfFlickerOnFadeLength);
		kSaveData_SaveTo(mfFlickerOffFadeLength);

		kSaveData_SaveTo(mFlickerOnColor);
		kSaveData_SaveTo(mfFlickerOnRadius);

		kSaveData_SaveTo(mbFlickerOn);
		kSaveData_SaveTo(mfFlickerTime);
		kSaveData_SaveTo(mfFlickerStateLength);
	}

	//-----------------------------------------------------------------------

	void iLight3D::LoadFromSaveData(iSaveData *apSaveData)
	{
		kSaveData_LoadFromBegin(iLight3D);

		//////////////////////////
		// Data
		if(pData->msFalloffMap != ""){
			iTexture *pTex = mpTextureManager->Create1D(pData->msFalloffMap,false);
			if(pTex) SetFalloffMap(pTex);
		}

		//////////////////////////
		// Variables
		kSaveData_LoadFrom(mDiffuseColor);
		kSaveData_LoadFrom(mSpecularColor);
		kSaveData_LoadFrom(mfIntensity);
		kSaveData_LoadFrom(mfFarAttenuation);
		kSaveData_LoadFrom(mfNearAttenuation);
		kSaveData_LoadFrom(mfSourceRadius);

		kSaveData_LoadFrom(mbCastShadows);
		kSaveData_LoadFrom(mbAffectMaterial);

		kSaveData_LoadFrom(mColAdd);
		kSaveData_LoadFrom(mfRadiusAdd);
		kSaveData_LoadFrom(mDestCol);
		kSaveData_LoadFrom(mfDestRadius);
		kSaveData_LoadFrom(mfFadeTime);

		kSaveData_LoadFrom(mbFlickering);
		kSaveData_LoadFrom(msFlickerOffSound);
		kSaveData_LoadFrom(msFlickerOnSound);
		kSaveData_LoadFrom(msFlickerOffPS);
		kSaveData_LoadFrom(msFlickerOnPS);
		kSaveData_LoadFrom(mfFlickerOnMinLength);
		kSaveData_LoadFrom(mfFlickerOffMinLength);
		kSaveData_LoadFrom(mfFlickerOnMaxLength);
		kSaveData_LoadFrom(mfFlickerOffMaxLength);
		kSaveData_LoadFrom(mFlickerOffColor);
		kSaveData_LoadFrom(mfFlickerOffRadius);
		kSaveData_LoadFrom(mbFlickerFade);
		kSaveData_LoadFrom(mfFlickerOnFadeLength);
		kSaveData_LoadFrom(mfFlickerOffFadeLength);

		kSaveData_LoadFrom(mFlickerOnColor);
		kSaveData_LoadFrom(mfFlickerOnRadius);

		kSaveData_LoadFrom(mbFlickerOn);
		kSaveData_LoadFrom(mfFlickerTime);
		kSaveData_LoadFrom(mfFlickerStateLength);
	}

	//-----------------------------------------------------------------------

	void iLight3D::SaveDataSetup(cSaveObjectHandler *apSaveObjectHandler, cGame *apGame)
	{
		kSaveData_SetupBegin(iLight3D);

		//Get attached billboards.
		cContainerListIterator<int> it = pData->mlstBillboardIds.GetIterator();
		while(it.HasNext())
		{
			int lId = it.Next();
			iSaveObject *pObject = apSaveObjectHandler->Get(lId);
			cBillboard *pBill = static_cast<cBillboard*>(pObject);

			if(pBill==NULL){
				Error("Couldn't find billboard id %s\n",lId);
				continue;
			}

			AttachBillboard(pBill);
		}
	}

	//-----------------------------------------------------------------------

}
