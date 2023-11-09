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
#include "scene/SubMeshEntity.h"
#include "scene/MeshEntity.h"

#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"

#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"

#include "scene/AnimationState.h"
#include "scene/NodeState.h"

#include "physics/PhysicsBody.h"

#include "math/Math.h"
#include "system/Log.h"

#include <string.h>

namespace hpl {
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	cSubMeshEntity::cSubMeshEntity(const tString &asName, cMeshEntity *apMeshEntity, cSubMesh * apSubMesh,
								cMaterialManager* apMaterialManager) : iRenderable(asName)
	{
		mpMeshEntity = apMeshEntity;
		mpSubMesh = apSubMesh;

		mpMaterialManager = apMaterialManager;

		mbCastShadows = false;

		mbGraphicsUpdated = false;

		mpBody = NULL;

		if(mpMeshEntity->GetMesh()->GetSkeleton())
		{
			mpDynVtxBuffer = mpSubMesh->GetVertexBuffer()->CreateCopy(VertexBufferUsageType::Dynamic);
		}
		else
		{
			mpDynVtxBuffer = NULL;
		}

		mpLocalNode = NULL;

		mpEntityCallback = new cSubMeshEntityBodyUpdate();
		mbUpdateBody = false;

		mpMaterial = NULL;
	}

	cSubMeshEntity::~cSubMeshEntity()
	{
		delete mpEntityCallback;

		if(mpDynVtxBuffer) delete mpDynVtxBuffer;

		/* Clear any custom textures here*/
		if(mpMaterial) mpMaterialManager->Destroy(mpMaterial);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// BODY CALLBACK
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSubMeshEntityBodyUpdate::OnTransformUpdate(iEntity3D* apEntity)
	{
		/*cSubMeshEntity *pSubEntity = static_cast<cSubMeshEntity*>(apEntity);

		if(pSubEntity->GetBody())
		{
			if(apEntity->GetWorldMatrix() != pSubEntity->GetBody()->GetLocalMatrix())
			{
				Log("Setting matrix on %s from\n",pSubEntity->GetBody()->GetName().c_str());
				Log("  %s\n",apEntity->GetWorldMatrix().ToString().c_str());
				Log("  %s\n",pSubEntity->GetBody()->GetLocalMatrix().ToString().c_str());

				pSubEntity->GetBody()->SetMatrix(apEntity->GetWorldMatrix());
			}
		}*/
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSubMeshEntity::UpdateLogic(float afTimeStep)
	{
		if(mbUpdateBody && mpBody)
		{
			mpBody->SetMatrix(GetWorldMatrix());
		}
	}

	//-----------------------------------------------------------------------


	iMaterial* cSubMeshEntity::GetMaterial()
	{
		if(mpMaterial==NULL && mpSubMesh->GetMaterial()==NULL)
		{
			Error("Materials for sub entity %s are NULL!\n",GetName().c_str());
		}

		if(mpMaterial)
			return mpMaterial;
		else
			return mpSubMesh->GetMaterial();
	}

	//-----------------------------------------------------------------------

	// Set Src as private variable to give this a little boost! Or?
	static inline void MatrixFloatTransformSet(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
	{
		pDest[0] = ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] + a_mtxA.m[0][3] ) * fWeight;
		pDest[1] = ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] + a_mtxA.m[1][3] ) * fWeight;
		pDest[2] = ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] + a_mtxA.m[2][3] ) * fWeight;
	}

	static inline void MatrixFloatRotateSet(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
	{
		pDest[0] = ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] ) * fWeight;
		pDest[1] = ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] ) * fWeight;
		pDest[2] = ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] ) * fWeight;
	}

	////////////////////////////////////////////////////////////

	// Set Src as private variable to give this a little boost!Or?
	static inline void MatrixFloatTransformAdd(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
	{
		pDest[0] += ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] + a_mtxA.m[0][3] ) * fWeight;
		pDest[1] += ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] + a_mtxA.m[1][3] ) * fWeight;
		pDest[2] += ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] + a_mtxA.m[2][3] ) * fWeight;
	}

	static inline void MatrixFloatRotateAdd(float *pDest, const cMatrixf &a_mtxA, const float* pSrc, const float fWeight)
	{
		pDest[0] += ( a_mtxA.m[0][0] * pSrc[0] + a_mtxA.m[0][1] * pSrc[1] + a_mtxA.m[0][2] * pSrc[2] ) * fWeight;
		pDest[1] += ( a_mtxA.m[1][0] * pSrc[0] + a_mtxA.m[1][1] * pSrc[1] + a_mtxA.m[1][2] * pSrc[2] ) * fWeight;
		pDest[2] += ( a_mtxA.m[2][0] * pSrc[0] + a_mtxA.m[2][1] * pSrc[1] + a_mtxA.m[2][2] * pSrc[2] ) * fWeight;
	}

	////////////////////////////////////////////////////////////

	void cSubMeshEntity::UpdateGraphics(cCamera *apCamera, float afFrameTime, cRenderList *apRenderList)
	{
		if(mpDynVtxBuffer)
		{
			if(mpMeshEntity->mbSkeletonPhysicsSleeping && mbGraphicsUpdated)
			{
				return;
			}

			mbGraphicsUpdated = true;

			const float *pBindPos = mpSubMesh->GetVertexBuffer()->GetArray(VertexAttr_Position);
			const float *pBindNormal = mpSubMesh->GetVertexBuffer()->GetArray(VertexAttr_Normal);
			const float *pBindTangent = mpSubMesh->GetVertexBuffer()->GetArray(VertexAttr_Tangent);

			float *pSkinPos = mpDynVtxBuffer->GetArray(VertexAttr_Position);
			float *pSkinNormal = mpDynVtxBuffer->GetArray(VertexAttr_Normal);
			float *pSkinTangent = mpDynVtxBuffer->GetArray(VertexAttr_Tangent);

			const int posStride = mpDynVtxBuffer->GetArrayStride(VertexAttr_Position);
			const int normalStride = mpDynVtxBuffer->GetArrayStride(VertexAttr_Normal);
			const int tangentStride = mpDynVtxBuffer->GetArrayStride(VertexAttr_Tangent);

			const int lVtxNum = mpDynVtxBuffer->GetVertexNum();

			for (int vtx=0; vtx < lVtxNum; vtx++)
			{
				//To count the bone bindings
				int lCount = 0;
				//Get pointer to weights and bone index.
				const float *pWeight = &mpSubMesh->mpVertexWeights[vtx*4];
				if(*pWeight==0) continue;

				const unsigned char *pBoneIdx = &mpSubMesh->mpVertexBones[vtx*4];

				const cMatrixf &mtxTransform = mpMeshEntity->mvBoneMatrices[*pBoneIdx];

				//Log("Vtx: %d\n",vtx);
				//Log("Boneidx: %d Count %d Weight: %f vtx: %d\n",(int)*pBoneIdx,lCount, *pWeight,vtx);

				//ATTENTION: Some optimizing might be done by accumulating the matrix instead.
								//THIS is really unsure since it seems like it will result in more math, matrix mul = 8*4 calc
								//Vertex mul with matrix is 3 * 3 calculations
								//this means: vertex= 9*3 = 27, matrix = 32

				MatrixFloatTransformSet(pSkinPos,mtxTransform, pBindPos, *pWeight);

				MatrixFloatRotateSet(pSkinNormal,mtxTransform, pBindNormal, *pWeight);

				MatrixFloatRotateSet(pSkinTangent,mtxTransform, pBindTangent, *pWeight);

				++pWeight; ++pBoneIdx; ++lCount;

				//Iterate weights until 0 is found or count < 4
				while(*pWeight != 0 && lCount < 4)
				{
					//Log("Boneidx: %d Count %d Weight: %f\n",(int)*pBoneIdx,lCount, *pWeight);
					const cMatrixf &mtxTransform = mpMeshEntity->mvBoneMatrices[*pBoneIdx];

					//Transform with the local movement of the bone.
					MatrixFloatTransformAdd(pSkinPos,mtxTransform, pBindPos, *pWeight);

					MatrixFloatRotateAdd(pSkinNormal,mtxTransform, pBindNormal, *pWeight);

					MatrixFloatRotateAdd(pSkinTangent,mtxTransform, pBindTangent, *pWeight);

					++pWeight; ++pBoneIdx; ++lCount;
				}

				pBindPos += posStride;
				pSkinPos += posStride;

				pBindNormal += normalStride;
				pSkinNormal += normalStride;

				pBindTangent += tangentStride;
				pSkinTangent += tangentStride;
			}

			//Update buffer
			mpDynVtxBuffer->UpdateData(VertexMask_Position | VertexMask_Normal | VertexMask_Tangent,false);
		}
	}

	iVertexBuffer* cSubMeshEntity::GetVertexBuffer()
	{
		if(mpDynVtxBuffer)
		{
			return mpDynVtxBuffer;
		}
		else
		{
			return mpSubMesh->GetVertexBuffer();
		}
	}

	cBoundingVolume* cSubMeshEntity::GetBoundingVolume()
	{
		return mpMeshEntity->GetBoundingVolume();
	}

	int cSubMeshEntity::GetMatrixUpdateCount()
	{
		if(mpMeshEntity->HasNodes())
		{
			return GetTransformUpdateCount();
		}
		else
		{
			return mpMeshEntity->GetMatrixUpdateCount();
		}
	}

	cMatrixf* cSubMeshEntity::GetModelMatrix(cCamera *apCamera)
	{
		if(mpMeshEntity->HasNodes())
		{
			//Log("%s Matrix from local node!\n",msName.c_str());
			return &GetWorldMatrix();
		}
		else
		{
			//Log("%s Matrix from mesh!\n",msName.c_str());

			if(mpMeshEntity->IsStatic()) return NULL;

			return mpMeshEntity->GetModelMatrix(NULL);
		}
	}

	//-----------------------------------------------------------------------

	void cSubMeshEntity::SetLocalNode(cNode3D *apNode)
	{
		mpLocalNode = apNode;

		mpLocalNode->AddEntity(this);
	}

	//-----------------------------------------------------------------------

	cNode3D* cSubMeshEntity::GetLocalNode()
	{
		return mpLocalNode;
	}

	//-----------------------------------------------------------------------

	tRenderContainerDataList* cSubMeshEntity::GetRenderContainerDataList()
	{
		//Log("Get from parent %s\n",mpMeshEntity->GetName().c_str());
		return mpMeshEntity->GetRenderContainerDataList();
	}

	//-----------------------------------------------------------------------

	void cSubMeshEntity::SetUpdateBody(bool abX)
	{
		mbUpdateBody = abX;

		/*if(mbUpdateBody)
		{
			AddCallback(mpEntityCallback);
		}
		else
		{
			RemoveCallback(mpEntityCallback);
		}*/
	}

	bool cSubMeshEntity::GetUpdateBody()
	{
		return mbUpdateBody;
	}

	//-----------------------------------------------------------------------

	void cSubMeshEntity::SetCustomMaterial(iMaterial *apMaterial, bool abDestroyOldCustom)
	{
		if(abDestroyOldCustom)
		{
			if(mpMaterial) mpMaterialManager->Destroy(mpMaterial);
		}

		mpMaterial = apMaterial;
	}

	//-----------------------------------------------------------------------

	cSector* cSubMeshEntity::GetCurrentSector() const
	{
		return mpMeshEntity->GetCurrentSector();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	kBeginSerialize(cSaveData_cSubMeshEntity,cSaveData_iRenderable)
	kSerializeVar(msMaterial, eSerializeType_String)
	kSerializeVar(mbCastShadows, eSerializeType_Bool)
	kSerializeVar(mlBodyId, eSerializeType_Int32)
	kSerializeVar(mbUpdateBody, eSerializeType_Bool)
	kEndSerialize()


	//-----------------------------------------------------------------------

	iSaveData* cSubMeshEntity::CreateSaveData()
	{
		return new cSaveData_cSubMeshEntity();
	}

	//-----------------------------------------------------------------------

	void cSubMeshEntity::SaveToSaveData(iSaveData *apSaveData)
	{
		kSaveData_SaveToBegin(cSubMeshEntity);

		kSaveData_SaveTo(mbCastShadows);
		kSaveData_SaveTo(mbUpdateBody);

		pData->msMaterial = mpMaterial==NULL ?"" : mpMaterial->GetName();

		kSaveData_SaveObject(mpBody, mlBodyId);
	}

	//-----------------------------------------------------------------------

	void cSubMeshEntity::LoadFromSaveData(iSaveData *apSaveData)
	{
		kSaveData_LoadFromBegin(cSubMeshEntity);

		kSaveData_LoadFrom(mbCastShadows);
		kSaveData_LoadFrom(mbUpdateBody);

		if(pData->msMaterial != "")
		{
			iMaterial *pMat = mpMaterialManager->CreateMaterial(pData->msMaterial);
			if(pMat) SetCustomMaterial(pMat);
		}
	}

	//-----------------------------------------------------------------------

	void cSubMeshEntity::SaveDataSetup(cSaveObjectHandler *apSaveObjectHandler, cGame *apGame)
	{
		kSaveData_SetupBegin(cSubMeshEntity);

		kSaveData_LoadObject(mpBody, mlBodyId,iPhysicsBody*);

		if(mpBody && mbUpdateBody == false)
		{
			mpBody->CreateNode()->AddEntity(this);
		}
	}

	//-----------------------------------------------------------------------
}
