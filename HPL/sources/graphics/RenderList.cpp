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
#include "graphics/RenderList.h"
#include "graphics/Renderable.h"
#include "scene/MeshEntity.h"
#include "graphics/Material.h"
#include "scene/Camera.h"
#include "math/Math.h"
#include "graphics/RenderState.h"
#include "graphics/Renderer3D.h"
#include "graphics/Graphics.h"


namespace hpl {


	int cRenderList::mlGlobalRenderCount = 0;


	//////////////////////////////////////////////////////////////////////////
	// MOTION BLUR OBJECT COMPARE
	//////////////////////////////////////////////////////////////////////////

	bool cMotionBlurObject_Compare::operator()(	iRenderable* pObjectA,
												iRenderable* pObjectB) const
	{
		//TODO: perhaps Z here...

		return pObjectA->GetVertexBuffer() < pObjectB->GetVertexBuffer();
	}

	//////////////////////////////////////////////////////////////////////////
	// TRANSPERANT OBJECT COMPARE
	//////////////////////////////////////////////////////////////////////////

	bool cTransperantObject_Compare::operator()(iRenderable* pObjectA,iRenderable* pObjectB) const
	{
		return pObjectA->GetZ() < pObjectB->GetZ();
	}


	//////////////////////////////////////////////////////////////////////////
	// QUERY COMPARE
	//////////////////////////////////////////////////////////////////////////

	bool cOcclusionQueryObject_Compare::operator()(	const cOcclusionQueryObject* pObjectA,
													const cOcclusionQueryObject* pObjectB) const
	{
		if(pObjectA->mpVtxBuffer != pObjectB->mpVtxBuffer)
			return pObjectA->mpVtxBuffer < pObjectB->mpVtxBuffer;

		if(pObjectA->mpMatrix != pObjectB->mpMatrix)
			return pObjectA->mpMatrix < pObjectB->mpMatrix;

		return (int)pObjectA->mbDepthTest < (int)pObjectB->mbDepthTest;
	}


	//////////////////////////////////////////////////////////////////////////
	// RENDER NODE
	//////////////////////////////////////////////////////////////////////////

	cMemoryPool<iRenderState>* g_poolRenderState = NULL;
	cMemoryPool<cRenderNode>* g_poolRenderNode = NULL;

	//-----------------------------------------------------------------------

	bool cRenderNodeCompare::operator()(cRenderNode* apNodeA,cRenderNode* apNodeB) const
	{
		int  val = apNodeA->mpState->Compare(apNodeB->mpState);
		bool ret =  val>0 ? true : false;
		return ret;
	}

	//-----------------------------------------------------------------------

	void cRenderNode::DeleteChildren()
	{
		tRenderNodeSetIt it = m_setNodes.begin();
		for(;it != m_setNodes.end(); ++it)
		{
			cRenderNode* pNode = *it;

			pNode->DeleteChildren();
			g_poolRenderNode->Release(pNode);
		}

		if(mpState) g_poolRenderState->Release(mpState);
		mpState = NULL;
		m_setNodes.clear();
	}

	//-----------------------------------------------------------------------

	void cRenderNode::Render(cRenderSettings* apSettings)
	{
		tRenderNodeSetIt it = m_setNodes.begin();
		for(;it != m_setNodes.end(); ++it)
		{
			cRenderNode* pNode = *it;

			pNode->mpState->SetMode(apSettings);
			pNode->Render(apSettings);
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRenderList::cRenderList()
	{
		mfFrameTime = 0;
		mTempNode.mpState = new iRenderState();

		mlRenderCount=0;
		mlLastRenderCount =0;

		m_poolRenderState = new cMemoryPool<iRenderState>(3000, NULL);
		m_poolRenderNode = new cMemoryPool<cRenderNode>(3000, NULL);

		g_poolRenderState = m_poolRenderState;
		g_poolRenderNode = m_poolRenderNode;
	}

	//-----------------------------------------------------------------------

	cRenderList::~cRenderList()
	{
		Clear();
		delete mTempNode.mpState;

		delete m_poolRenderState;
		delete m_poolRenderNode;

		g_poolRenderState = NULL;
		g_poolRenderNode = NULL;
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	void cRenderList::Clear()
	{
		++mlGlobalRenderCount;

		//Clear containers.
		m_setLights.clear();
		m_setObjects.clear();
		m_setQueries.clear();
		m_setMotionBlurObjects.clear();
		m_setTransperantObjects.clear();

		mRootNodeDiffuse.DeleteChildren();

		mlLastRenderCount = mlRenderCount;
		mlRenderCount++;
	}

	//-----------------------------------------------------------------------

	void cRenderList::AddOcclusionQuery(cOcclusionQueryObject *apObject)
	{
		m_setQueries.insert(apObject);
	}

	//-----------------------------------------------------------------------

	cOcclusionQueryObjectIterator cRenderList::GetQueryIterator()
	{
		return cOcclusionQueryObjectIterator(&m_setQueries);
	}

	//-----------------------------------------------------------------------

	cMotionBlurObjectIterator cRenderList::GetMotionBlurIterator()
	{
		return cMotionBlurObjectIterator(&m_setMotionBlurObjects);
	}

	//-----------------------------------------------------------------------

	cTransperantObjectIterator cRenderList::GetTransperantIterator()
	{
		return cTransperantObjectIterator(&m_setTransperantObjects);
	}

	//-----------------------------------------------------------------------

	cLight3DIterator cRenderList::GetLightIt()
	{
		return cLight3DIterator(&m_setLights);
	}

	//-----------------------------------------------------------------------

	cRenderableIterator cRenderList::GetObjectIt()
	{
		return cRenderableIterator(&m_setObjects);
	}

	//-----------------------------------------------------------------------

	int cRenderList::GetLightNum()
	{
		return (int)m_setLights.size();
	}

	int cRenderList::GetObjectNum()
	{
		return (int)m_setObjects.size();
	}

	//-----------------------------------------------------------------------

	cRenderNode* cRenderList::GetRootNode()
	{
		return &mRootNodeDiffuse;
	}

	//-----------------------------------------------------------------------

	void cRenderList::Compile()
	{
		int lLightNum = (int)m_setLights.size();
		if(lLightNum > MAX_NUM_OF_LIGHTS) lLightNum = MAX_NUM_OF_LIGHTS;

		for (int i=0; i<lLightNum; ++i) mvObjectsPerLight[i] = 0;

		// Iterate the objects to be rendered and build trees with render states.
		for (auto pObject : m_setObjects)
		{
			iMaterial* pMat = pObject->GetMaterial();

			//Skip meshes...
			if(pObject->GetRenderType() == eRenderableType_Mesh) continue;

			if (pMat->IsTransperant())
			{
				//Use the set cotainer instead for now:
				m_setTransperantObjects.insert(pObject);
			}
			else
			{
				//If the object uses z pass add to z tree.
				AddToTree(pObject);

				//Light trees that the object will belong to.
				/*
				int lLightCount =0;

				for (auto pLight : m_setLights)
				{
					if(	(pLight->GetOnlyAffectInSector()==false || pObject->IsInSector(pLight->GetCurrentSector())) &&
						pLight->CheckObjectIntersection(pObject))
					{
						if(lLightCount >= MAX_NUM_OF_LIGHTS) break;

						++mvObjectsPerLight[lLightCount];

						AddToTree(pObject);
					}
					++lLightCount;
				}
				*/
			}
		}
	}

	//-----------------------------------------------------------------------

	bool cRenderList::Add(iRenderable* apObject)
	{
		if(apObject->IsVisible()==false) return false;

		//Check if the object is culled by fog.
		/*
		cRenderer3D *pRenderer = mpGraphics->GetRenderer3D();
		if(pRenderer->GetFogActive() && pRenderer->GetFogCulling())
		{
			if(cMath::CheckCollisionBV(*pRenderer->GetFogBV(), *apObject->GetBoundingVolume()))
			{

			}
			else {
				return false;
			}
		}
		*/

		//Check if the object as already been added.
		if(mlRenderCount == apObject->GetRenderCount()) return false;
		apObject->SetRenderCount(mlRenderCount);

		//Update the graphics of object.
		apObject->UpdateGraphics(cRenderList::GetCamera(),mfFrameTime,this);

		//Set that the object is to be rendered.
		apObject->SetGlobalRenderCount(mlGlobalRenderCount);

		switch(apObject->GetRenderType())
		{
			//Add a normal renderable
			case eRenderableType_Normal:
			case eRenderableType_ParticleSystem:
				if(apObject->GetMaterial()->IsTransperant())
				{
					//Calculate the Z for the trans object
					cVector3f vCameraPos = cMath::MatrixMul(cRenderList::GetCamera()->GetViewMatrix(),
						apObject->GetBoundingVolume()->GetWorldCenter());
					apObject->SetZ(vCameraPos.z);

					m_setObjects.insert(apObject);
				}
				else
				{
					m_setObjects.insert(apObject);

					//MotionBlur
					/*
					if(false // mpGraphics->GetRendererPostEffects()->GetMotionBlurActive()
						|| false // mpGraphics->GetRenderer3D()->GetRenderSettings()->mbFogActive
						|| false // mpGraphics->GetRendererPostEffects()->GetDepthOfFieldActive()
					)
					{
						m_setMotionBlurObjects.insert(apObject);

						if(apObject->GetPrevRenderCount() != GetLastRenderCount())
						{
							cMatrixf *pMtx = apObject->GetModelMatrix(mpCamera);
							if(pMtx)
							{
								apObject->SetPrevMatrix(*pMtx);
							}
						}
						apObject->SetPrevRenderCount(mlRenderCount);
					}
					*/
				}
				break;

			//Add all sub meshes of the mesh
			case eRenderableType_Mesh:
				{
					cMeshEntity* pMesh = static_cast<cMeshEntity*>(apObject);
					for(int i=0;i<pMesh->GetSubMeshEntityNum();i++)
					{
						Add(pMesh->GetSubMeshEntity(i));
					}
					break;
				}

			//Add a light to a special container
			case eRenderableType_Light:
				{
					iLight3D *pLight = static_cast<iLight3D*>(apObject);
					m_setLights.insert(pLight);
				}
				break;

			default:
				// no action
				break;
		}

		return true;

	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRenderNode* cRenderList::InsertNode(cRenderNode* apListNode, cRenderNode* apTempNode)
	{
		//Log("Searching for node... ");
		//Create a node with the state.

		tRenderNodeSetIt it = apListNode->m_setNodes.find(apTempNode);

		if(it != apListNode->m_setNodes.end())
		{
			//Log("Node found!\n");
			//node found, return it.
			return *it;
		}
		else
		{
			//Log("Node NOT found, creating new.\n");
			//no node found, create one and return it.
			cRenderNode *pNode = m_poolRenderNode->Create();
			pNode->mpState = m_poolRenderState->Create();

			//Copy the relevant values to the state.
			pNode->mpState->Set(apTempNode->mpState);

			apListNode->m_setNodes.insert(pNode);

			return pNode;
		}
	}

	//-----------------------------------------------------------------------

	void cRenderList::AddToTree(iRenderable* apObject)
	{
		//Log("------ OBJECT ------------\n");

		cRenderNode *pNode = GetRootNode();
		iMaterial *pMaterial = apObject->GetMaterial();
		iRenderState *pTempState = mTempNode.mpState;
		cRenderNode *pTempNode = &mTempNode;

		//-------------- EXPLAINATION ----------------------------------
		// Go through each render state type and set the appropriate
		// variables for each type. The most important states are set first.
		// The state is then inserted to a tree structure, where each state type is a level.
		//                       Example:
		//                 |----------root--------|
		//           |---pass              |-----pass----|
		//       depth-test            depth-test     depth-test
		//              ............etc......................
		// After each insertion a pointer to the inserted or existing (if variables already existed)
		// node is returned
		// This is then passed on the next state, the state inserted (or exist found) there and so on.
		// At each insertion, it is first checked if a state with variables exist, and if so, use that.
		// if not, a new node is created and inserted.
		// The temp node state is used as storage for the data to skip the overhead of deletion
		// and creation when testing if state exist. Instead new data is only created if the state
		// is new.
		// Each Render state has the same class, but uses different var depending on type, this to
		// skip the overhead of using inheritance.
		//-------------------------------------------------------------

		/////// SECTOR //////////////
		{
			pTempState->mType = eRenderStateType_Sector;
			pTempState->mpSector = apObject->GetCurrentSector();

			pNode = InsertNode(pNode, pTempNode);
		}

		/////// ALPHA MODE //////////////
		{
			//Log("\nAlpha level %d\n", pMaterial->GetAlphaMode(mType));
			pTempState->mType = eRenderStateType_AlphaMode;
			pTempState->mAlphaMode = pMaterial->GetAlphaMode();

			pNode = InsertNode(pNode, pTempNode);
		}

		/////// BLEND MODE //////////////
		{
			//Log("\nBlend level %d : %d\n",pMaterial->GetBlendMode(mType),pMaterial->GetChannelMode(mType));
			pTempState->mType = eRenderStateType_BlendMode;

			pTempState->mBlendMode = pMaterial->GetBlendMode();
			pTempState->mChannelMode = pMaterial->GetChannelMode();

			pNode = InsertNode(pNode, pTempNode);
		}

		/////// GPU PROGRAM //////////////
		{
			//Log("\nVertex program level\n");
			pTempState->mType = eRenderStateType_GPUProgram;

			pTempState->mpProgram = pMaterial->GetProgramEx();
			pTempState->mpProgramSetup = pMaterial->GetProgramSetup();

			pNode = InsertNode(pNode, pTempNode);
		}

		/////// TEXTURE //////////////
		{
			//Log("\nTexture level\n");
			pTempState->mType = eRenderStateType_Texture;
			for(int i=0; i<MAX_TEXTUREUNITS;i++)
			{
				pTempState->mpTexture[i] = pMaterial->GetTexture(i);
			}
			pNode = InsertNode(pNode, pTempNode);
		}

		/////// VERTEX BUFFER //////////////
		{
			//Log("\nVertex buffer level\n");
			pTempState->mType = eRenderStateType_VertexBuffer;
			pTempState->mpVtxBuffer = apObject->GetVertexBuffer();
			pNode = InsertNode(pNode, pTempNode);
		}

		/////// MATRIX //////////////
		{
			//Log("\nMatrix level\n");
			pTempState->mType = eRenderStateType_Matrix;

			pTempState->mpModelMatrix = apObject->GetModelMatrix(mpCamera);
			pTempState->mpInvModelMatrix = apObject->GetInvModelMatrix();
			pTempState->mvScale = apObject->GetCalcScale();

			pNode = InsertNode(pNode, pTempNode);
		}

		/////// RENDER //////////////
		{
			//Log("\nRender leaf!\n");
			pTempState->mType = eRenderStateType_Render;
			pTempState->mpObject = apObject;
			InsertNode(pNode, pTempNode);
		}

		//Log("-------------------------\n");
	}

	//-----------------------------------------------------------------------

}
