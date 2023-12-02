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
#include "graphics/Renderer3D.h"

#include "math/Math.h"
#include "graphics/Texture.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/TextureManager.h"
#include "resources/GpuProgramManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/MeshCreator.h"
#include "scene/Camera.h"
#include "scene/Entity3D.h"
#include "graphics/RenderList.h"
#include "graphics/Renderable.h"
#include "scene/World3D.h"
#include "scene/RenderableContainer.h"
#include "scene/Light3D.h"
#include "math/BoundingVolume.h"
#include "graphics/GPUProgram.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// RENDER SETTINGS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRenderSettings::cRenderSettings()
	{
		mbFogActive = false;
		mfFogStart = 5.0f;
		mfFogEnd = 5.0f;
		mFogColor = cColor(1,1);
		mbFogCulling = false;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRenderer3D::cRenderer3D(iLowLevelGraphics *apLowLevelGraphics, cTextureManager* textureManager, cGpuProgramManager* programManager)
	{
		Log("  Creating Renderer3D\n");

		_llGfx = apLowLevelGraphics;
		_programManager = programManager;
		_textureManager = textureManager;

		mpSkyBoxTexture = NULL;
		mbAutoDestroySkybox = false;
		mbSkyBoxActive = false;
		mSkyBoxColor = cColor(1,1);

		mRenderSettings.mAmbientColor = cColor(0,1);

		mpRenderList = new cRenderList{};

		mDebugFlags = 0;

		mbLog = false;

		mfRenderTime =0;

		//Set up render settings.
		mRenderSettings.mpLowLevel = _llGfx;
		mRenderSettings.mbLog = false;
		mRenderSettings.mShowShadows = eRendererShowShadows_All;

		Log("   Load Renderer3D gpu programs:\n");

		///////////////////////////////////
		//Load diffuse program, for stuff like query rendering
		Log("    Diffuse\n");
		mpDiffuseProgram = _programManager->CreateProgram("Universal.vert", "Universal.frag");
		if(mpDiffuseProgram==NULL)
		{
			Error("Couldn't load Diffuse shader\n");
		}

		/////////////////////////////////////////////
		//Create sky box graphics.

		Log("   init sky box\n");
		InitSkyBox();

		Log("  Renderer3D created\n");
	}

	//-----------------------------------------------------------------------

	cRenderer3D::~cRenderer3D()
	{
		delete mpRenderList;

		if(mpDiffuseProgram) _programManager->Destroy(mpDiffuseProgram);

		if(mpSkyBox) delete mpSkyBox;
		if(mpSkyBoxTexture && mbAutoDestroySkybox)
		{
			_textureManager->Destroy(mpSkyBoxTexture);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cRenderSettings::Clear()
	{
		mbDepthTest = true;

		mAlphaMode = eMaterialAlphaMode_Solid;
		mBlendMode = eMaterialBlendMode_None;
		mChannelMode = eMaterialChannelMode_RGBA;

		mpProgram = NULL;

		mpSector = NULL;

		for(int i=0;i<MAX_TEXTUREUNITS;i++)
		{
			mpTexture[i] = NULL;
		}

		mpVtxBuffer = NULL;
	}

	void cRenderSettings::Reset(iLowLevelGraphics *apLowLevel)
	{
		if(mpProgram) mpProgram->UnBind();
		if(mpVtxBuffer) mpVtxBuffer->UnBind();

		for(int i=0;i<MAX_TEXTUREUNITS;i++)
		{
			if(mpTexture[i])
			{
				apLowLevel->SetTexture(i,NULL);
			}
		}

		Clear();
	}

	//-----------------------------------------------------------------------

	eRendererShowShadows cRenderer3D::GetShowShadows()
	{
		return mRenderSettings.mShowShadows;
	}
	void cRenderer3D::SetShowShadows(eRendererShowShadows aState)
	{
		mRenderSettings.mShowShadows = aState;
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::UpdateRenderList(cWorld3D* apWorld, cCamera *apCamera, float afFrameTime)
	{
		//Clear all objects to be rendereded
		mpRenderList->Clear();

		//Set some variables
		mpRenderList->SetFrameTime(afFrameTime);
		mpRenderList->SetCamera(apCamera);

		//Set the frustum
		mRenderSettings.mpFrustum = apCamera->GetFrustum();

		//Setup fog BV
		if(mRenderSettings.mbFogActive && mRenderSettings.mbFogCulling)
		{
			//This is becuase the fog line is a stright line infront of the camera.
			float fCornerDist = (mRenderSettings.mfFogEnd *2.0f) /
								cos(apCamera->GetFOV()*apCamera->GetAspect()*0.5f);

			mFogBV.SetSize(fCornerDist);
			mFogBV.SetPosition(apCamera->GetPosition());
		}

		//Add all objects to be rendered
		apWorld->GetRenderContainer()->GetVisible(mRenderSettings.mpFrustum, mpRenderList);

		//Compile an optimized render list.
		mpRenderList->Compile();
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderWorld(cWorld3D* apWorld, cCamera *apCamera, float afFrameTime)
	{
		mfRenderTime += afFrameTime;

		//////////////////////////////
		//Setup render settings and logging
		if(mDebugFlags & eRendererDebugFlag_LogRendering) {
			mbLog = true;
			mRenderSettings.mbLog = true;
		}
		else if(mbLog)
		{
			mbLog = false;
			mRenderSettings.mbLog = false;
		}
		mRenderSettings.mDebugFlags = mDebugFlags;

		/////////////////////////////////
		//Set up rendering
		_llGfx->SetCullActive(true);
		_llGfx->SetDepthTestActive(true);
		_llGfx->SetDepthTestFunc(eDepthTestFunc_LessOrEqual);

		mRenderSettings.mpCamera = apCamera;

		for (int i=0; i < MAX_TEXTUREUNITS; ++i)
			_llGfx->SetTexture(i, NULL);

		mRenderSettings.Clear();

		////////////////////////////
		// Render Z
//		RenderZ(apCamera);

		////////////////////////////
		//Render Diffuse
		RenderDiffuse(apCamera);

		////////////////////////////
		//Render Occlusion Queries
		RenderOcclusionQueries(apCamera);

		////////////////////////////
		//Render lighting
//		RenderLight(apCamera);


		////////////////////////////
		//Render sky box
		RenderSkyBox(apCamera);

		//Render transparent
		RenderTrans(apCamera);

		mRenderSettings.Reset(_llGfx);

		////////////////////////////
		//Render debug
//		RenderDebug(apCamera);
//		RenderPhysicsDebug(apWorld, apCamera);

		_llGfx->SetColorWriteActive(true, true, true, true);
		_llGfx->SetDepthWriteActive(true);
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::SetSkyBox(iTexture *apTexture, bool abAutoDestroy)
	{
		if(mpSkyBoxTexture && mbAutoDestroySkybox)
		{
			_textureManager->Destroy(mpSkyBoxTexture);
		}

		mbAutoDestroySkybox = abAutoDestroy;
		mpSkyBoxTexture = apTexture;
		if(mpSkyBoxTexture)
		{
			mpSkyBoxTexture->SetWrapS(eTextureWrap_ClampToEdge);
			mpSkyBoxTexture->SetWrapT(eTextureWrap_ClampToEdge);
		}
	}

	void cRenderer3D::SetSkyBoxActive(bool abX)
	{
		mbSkyBoxActive = abX;
	}

	void cRenderer3D::SetSkyBoxColor(const cColor& aColor)
	{
		if(mSkyBoxColor == aColor) return;
		mSkyBoxColor = aColor;

		float *pColors = mpSkyBox->GetArray(VertexAttr_Color0);
		int colorStride = mpSkyBox->GetArrayStride(VertexAttr_Color0);

		int lNum = mpSkyBox->GetVertexCount();
		for(int i=0; i<lNum;++i)
		{
			pColors[0] = mSkyBoxColor.r;
			pColors[1] = mSkyBoxColor.g;
			pColors[2] = mSkyBoxColor.b;
			pColors[3] = mSkyBoxColor.a;
			pColors += colorStride;
		}

		mpSkyBox->UpdateData(VertexMask_Color0, false);
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::SetFogActive(bool abX)
	{
		mRenderSettings.mbFogActive = abX;
	}
	void cRenderer3D::SetFogStart(float afX)
	{
		mRenderSettings.mfFogStart = afX;
	}
	void cRenderer3D::SetFogEnd(float afX)
	{
		mRenderSettings.mfFogEnd = afX;
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::FetchOcclusionQueries()
	{
		if(mbLog) Log("Fetching Occlusion Queries Result:\n");

		//With depth test
		cOcclusionQueryObjectIterator it = mpRenderList->GetQueryIterator();
		while(it.HasNext())
		{
			cOcclusionQueryObject *pObject = it.Next();
			//LogUpdate("Query: %d!\n",pObject->mpQuery);

			while(pObject->mpQuery->FetchResults()==false);

			if(mbLog) Log(" Query: %d SampleCount: %d\n",	pObject->mpQuery,
															pObject->mpQuery->GetSampleCount());
		}

		if(mbLog) Log("Done fetching queries\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cRenderer3D::InitSkyBox()
	{
		mpSkyBox = CreateSkyBoxVertexBuffer(_llGfx, 1);
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderSkyBox(cCamera *apCamera)
	{
	/*
		if(mbSkyBoxActive==false) return;

		if(mbLog) Log("Rendering Skybox:\n");
		_llGfx->SetDepthTestFunc(eDepthTestFunc_LessOrEqual);

		if(mRenderSettings.mpProgram)
		{
			mRenderSettings.mpProgram->UnBind();
			mRenderSettings.mpProgram = NULL;
			if(mbLog) Log(" Setting program: NULL\n");
		}
		if(mRenderSettings.mpVtxBuffer)
		{
			mRenderSettings.mpVtxBuffer->UnBind();
			mRenderSettings.mpVtxBuffer = NULL;
			if(mbLog) Log(" Setting Vertex Buffer: NULL\n");
		}

		for(int i=1; i<MAX_TEXTUREUNITS; ++i)
		{
			if(mRenderSettings.mpTexture[i])
			{
				_llGfx->SetTexture(i,NULL);
				mRenderSettings.mpTexture[i] = NULL;
				if(mbLog) Log(" Setting Texture %d : NULL\n",i);
			}
		}

		cMatrixf mtxSky = cMatrixf::Identity;

		//Calculate the size of the sky box need to just touch the far clip plane.
		float fFarClip = apCamera->GetFarClipPlane();
		float fSide = sqrt((fFarClip*fFarClip) / 3) *0.95f;
		mtxSky.m[0][0] = fSide;
		mtxSky.m[1][1] = fSide;
		mtxSky.m[2][2] = fSide;

		mtxSky = cMath::MatrixMul(mtxSky,apCamera->GetViewMatrix());

		mtxSky.SetTranslation(0);

		_llGfx->SetMatrix(eMatrix_ModelView,mtxSky);

		mpLowLevelGraphics->SetTexture(0,mpSkyBoxTexture);
		mRenderSettings.mpTexture[0] = mpSkyBoxTexture;

		mpLowLevelGraphics->SetBlendActive(false);
		mRenderSettings.mBlendMode = eMaterialBlendMode_Replace;

		mpSkyBox->Bind();
		mpSkyBox->Draw();
		mpSkyBox->UnBind();
	*/
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderZ(cCamera *apCamera)
	{
		if(mbLog) Log("Rendering ZBuffer:\n");

		//_llGfx->SetDepthTestFunc(eDepthTestFunc_Equal);
		_llGfx->SetColorWriteActive(false, false, false,false);
		mRenderSettings.mChannelMode = eMaterialChannelMode_Z;

		cRenderNode* pNode = mpRenderList->GetRootNode();
		pNode->Render(&mRenderSettings);
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderOcclusionQueries(cCamera *apCamera)
	{
		if(mbLog) Log("Rendering Occlusion Queries:\n");
		_llGfx->SetColorWriteActive(false, false, false, false);
		mRenderSettings.mChannelMode = eMaterialChannelMode_Z;
		_llGfx->SetDepthWriteActive(false);

		////////////////////////////
		// Program
		if(mRenderSettings.mpProgram != mpDiffuseProgram)
		{
			if(mRenderSettings.mpProgram) mRenderSettings.mpProgram->UnBind();
			mRenderSettings.mpProgram = mpDiffuseProgram;

			mpDiffuseProgram->Bind();
			if(mbLog) Log(" Binding program %d\n",mpDiffuseProgram);
		}

		////////////////////////
		// Reset texture unit 0
		_llGfx->SetTexture(0,NULL);
		mRenderSettings.mpTexture[0] = NULL;

		////////////////////////
		// Keep track of what has been set
		iVertexBuffer *pPrevBuffer = mRenderSettings.mpVtxBuffer;
		cMatrixf viewProjMatrix = cMath::MatrixMul(apCamera->GetProjectionMatrix(), apCamera->GetViewMatrix());
		bool bPrevDepthTest = true;

		//////////////////////////////////
		//Iterate the query objects
		cOcclusionQueryObjectIterator it = mpRenderList->GetQueryIterator();
		while(it.HasNext())
		{
			cOcclusionQueryObject *pObject = it.Next();

			/*if(pObject->mbDepthTest) {
				mpLowLevelGraphics->SetColorWriteActive(true, true,true,true);
				mRenderSettings.mChannelMode = eMaterialChannelMode_RGBA;
			}
			else {
				mpLowLevelGraphics->SetColorWriteActive(false, false, false, false);
				mRenderSettings.mChannelMode = eMaterialChannelMode_Z;
			}*/


			/////////////////////
			//Set depth test
			if(bPrevDepthTest != pObject->mbDepthTest)
			{
				//mpLowLevelGraphics->SetDepthTestActive(pObject->mbDepthTest);
				if(pObject->mbDepthTest)
					_llGfx->SetDepthTestFunc(eDepthTestFunc_LessOrEqual);
				else
					_llGfx->SetDepthTestFunc(eDepthTestFunc_Always);

				bPrevDepthTest = pObject->mbDepthTest;
				if(mbLog) Log(" Setting depth test %d\n",pObject->mbDepthTest?1:0);
			}

			/////////////////////
			//Set matrix
			cMatrixf mvpMat = cMath::MatrixMul(viewProjMatrix, pObject->_matrix);
			mpDiffuseProgram->SetMatrixf("worldViewProj", mvpMat);
			//if(mbLog) Log(" Setting matrix %d\n",pObject->_matrix);

			/////////////////////
			//Set Vertex buffer and draw
			if(pPrevBuffer != pObject->mpVtxBuffer)
			{
				if(pPrevBuffer) pPrevBuffer->UnBind();
				pObject->mpVtxBuffer->Bind();
				pPrevBuffer = pObject->mpVtxBuffer;

				if(mbLog) Log(" Setting vtx buffer %d\n",pObject->mpVtxBuffer);
			}

			pObject->mpQuery->Begin();
			pObject->mpVtxBuffer->Draw();
			pObject->mpQuery->End();

			if(mbLog) Log(" Render with query: %d\n",pObject->mpQuery);
		}

		mRenderSettings.mpVtxBuffer = pPrevBuffer;

		//if(bPrevDepthTest==false)
		//	mpLowLevelGraphics->SetDepthTestActive(true);
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderLight(cCamera *apCamera)
	{
		if(mDebugFlags & eRendererDebugFlag_DisableLighting) return;
		if(mbLog) Log("Rendering Lighting:\n");

		mRenderSettings.mChannelMode = eMaterialChannelMode_RGBA;
		_llGfx->SetColorWriteActive(true, true, true, true);
		_llGfx->SetDepthTestFunc(eDepthTestFunc_Equal);

		cLight3DIterator lightIt = mpRenderList->GetLightIt();

		int lLightCount=0;
		while(lightIt.HasNext())
		{
			if(lLightCount >= MAX_NUM_OF_LIGHTS) break;

			iLight3D* pLight = lightIt.Next();

			/*
			if(mpRenderList->GetLightObjects(lLightCount)==0)
			{
				lLightCount++;
				continue;
			}

			if(mbLog) Log("-----Light %s/%d ------\n",pLight->GetName().c_str(), (size_t)pLight);

			cRenderNode* pNode = mpRenderList->GetRootNode(eRenderListDrawType_Normal);

			if(pLight->BeginDraw(&mRenderSettings, _llGfx))
			{
				pNode->Render(&mRenderSettings);
			}
			pLight->EndDraw(&mRenderSettings, _llGfx);
			*/

			lLightCount++;
		}
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderDiffuse(cCamera *apCamera)
	{
		if(mbLog) Log("Rendering Diffuse:\n");

		_llGfx->SetColorWriteActive(true, true, true, true);
		mRenderSettings.mChannelMode = eMaterialChannelMode_RGBA;

		cRenderNode* pNode = mpRenderList->GetRootNode();
		pNode->Render(&mRenderSettings);
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderTrans(cCamera *apCamera)
	{
		if(mbLog) Log("Rendering Transparent:\n");
		cVector3f vForward = apCamera->GetForward();

		cTransperantObjectIterator it = mpRenderList->GetTransperantIterator();
		while(it.HasNext())
		{
			// render object
		}
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderDebug(cCamera *apCamera)
	{
	/*
		if(mDebugFlags)
		{
			mpLowLevelGraphics->SetDepthWriteActive(false);

			//Render Debug for objects
			cRenderableIterator objectIt = mpRenderList->GetObjectIt();
			while(objectIt.HasNext())
			{
				iRenderable* pObject = objectIt.Next();
				RenderDebugObject(apCamera, pObject);
			}

			//Render debug for lights.
			if(mDebugFlags & eRendererDebugFlag_DrawLightBoundingBox)
			{
				mpLowLevelGraphics->SetDepthTestActive(false);
				mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,apCamera->GetViewMatrix());

				cLight3DIterator lightIt = mpRenderList->GetLightIt();
				while(lightIt.HasNext())
				{
					iLight3D* pLight = lightIt.Next();
					cBoundingVolume *pBV = pLight->GetBoundingVolume();

					cColor Col = pLight->GetDiffuseColor();

					mpLowLevelGraphics->DrawSphere(pLight->GetWorldPosition(),0.1f,Col);
					mpLowLevelGraphics->DrawBoxMaxMin(pBV->GetMax(), pBV->GetMin(),Col);
				}

				mpLowLevelGraphics->SetDepthTestActive(true);
			}

			mpLowLevelGraphics->SetDepthWriteActive(true);
		}
	*/
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderDebugObject(cCamera *apCamera,iRenderable* &apObject)
	{
	/*
		iVertexBuffer* pVtxBuffer = apObject->GetVertexBuffer();

		if(mDebugFlags & eRendererDebugFlag_DrawBoundingBox)
		{
			mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,apCamera->GetViewMatrix());

			cBoundingVolume *pBV = apObject->GetBoundingVolume();

			mpLowLevelGraphics->DrawBoxMaxMin(pBV->GetMax(), pBV->GetMin(),cColor(1,1.0f,1.0f,1));
		}

		if(mDebugFlags & eRendererDebugFlag_DrawBoundingSphere)
		{
			mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,apCamera->GetViewMatrix());

			cBoundingVolume *pBV = apObject->GetBoundingVolume();

			mpLowLevelGraphics->DrawSphere(pBV->GetWorldCenter(), pBV->GetRadius(),cColor(1,1.0f,1.0f,1));
		}

		cMatrixf mtxModel;
		cMatrixf *pModelMtx = apObject->GetModelMatrix(apCamera);

		if(pModelMtx)
			mtxModel = cMath::MatrixMul(apCamera->GetViewMatrix(),*pModelMtx);
		else
			mtxModel = cMath::MatrixMul(apCamera->GetViewMatrix(),cMatrixf::Identity);

		mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,mtxModel);

		//Draw the debug graphics for the object.
		for(int i=0; i< pVtxBuffer->GetVertexCount(); i++)
		{
			cVector3f vPos = pVtxBuffer->GetVector3(VertexAttr_Position, i);

			if(mDebugFlags & eRendererDebugFlag_DrawNormals)
			{
				cVector3f vNormal = pVtxBuffer->GetVector3(VertexAttr_Normal, i);

				mpLowLevelGraphics->DrawLine(vPos,vPos+(vNormal*0.1f),cColor(0.5f,0.5f,1,1));
			}
			if(mDebugFlags & eRendererDebugFlag_DrawTangents)
			{
				cVector3f vTan = pVtxBuffer->GetVector3(VertexAttr_Tangent, i);

				mpLowLevelGraphics->DrawLine(vPos,vPos+(vTan*0.2f),cColor(1,0.0f,0.0f,1));
			}
		}
	*/
	}

	//-----------------------------------------------------------------------

	void cRenderer3D::RenderPhysicsDebug(cWorld3D *apWorld, cCamera *apCamera)
	{
	/*
		if ((mDebugFlags & eRendererDebugFlag_DrawPhysicsBox) == 0)
		{
			return;
		}

		mpLowLevelGraphics->SetDepthWriteActive(false);
		mpLowLevelGraphics->SetDepthTestActive(false);

		mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,apCamera->GetViewMatrix());

		apWorld->GetPhysicsWorld()->RenderDebugGeometry(mpLowLevelGraphics, cColor(1, 1, 0, 1));

		mpLowLevelGraphics->SetDepthTestActive(true);
		mpLowLevelGraphics->SetDepthWriteActive(true);
	*/
	}

	//-----------------------------------------------------------------------
}
