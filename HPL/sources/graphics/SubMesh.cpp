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
#include "graphics/SubMesh.h"

#include "graphics/Mesh.h"
#include "resources/MaterialManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"
#include "math/Math.h"

#include "system/Log.h"

#include <string.h>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSubMesh::cSubMesh(const tString &asName, cMaterialManager* apMaterialManager)
	{
		mpMaterialManager = apMaterialManager;

		msName = asName;
		msNodeName = "";

		mpMaterial = NULL;
		mpVtxBuffer = NULL;

		mpVertexWeights = NULL;
		mpVertexBones = NULL;

		m_mtxLocalTransform = cMatrixf::Identity;
	}

	//-----------------------------------------------------------------------

	cSubMesh::~cSubMesh()
	{
		if(mpMaterial)mpMaterialManager->Destroy(mpMaterial);
		if(mpVtxBuffer) delete mpVtxBuffer;

		if(mpVertexBones) delete[] mpVertexBones;
		if(mpVertexWeights) delete[] mpVertexWeights;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSubMesh::SetMaterial(iMaterial* apMaterial)
	{
		if(mpMaterial) mpMaterialManager->Destroy(mpMaterial);
		mpMaterial = apMaterial;
	}

	//-----------------------------------------------------------------------

	void cSubMesh::SetVertexBuffer(iVertexBuffer* apVtxBuffer)
	{
		if(mpVtxBuffer == apVtxBuffer) return;

		mpVtxBuffer = apVtxBuffer;
	}

	//-----------------------------------------------------------------------

	iMaterial *cSubMesh::GetMaterial()
	{
		return mpMaterial;
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cSubMesh::GetVertexBuffer()
	{
		return mpVtxBuffer;
	}

	//-----------------------------------------------------------------------

	void cSubMesh::ResizeVertexBonePairs(int alSize)
	{
		mvVtxBonePairs.reserve(alSize);
	}

	int cSubMesh::GetVertexBonePairNum()
	{
		return (int)mvVtxBonePairs.size();
	}
	cVertexBonePair& cSubMesh::GetVertexBonePair(int alNum)
	{
		return mvVtxBonePairs[alNum];
	}

	void cSubMesh::AddVertexBonePair(const cVertexBonePair &aPair)
	{
		mvVtxBonePairs.push_back(aPair);
	}

	void cSubMesh::ClearVertexBonePairs()
	{
		mvVtxBonePairs.clear();
	}


	//-----------------------------------------------------------------------

	///normalize weights here?
	void cSubMesh::CompileBonePairs()
	{
		mpVertexWeights = new float[4 * mpVtxBuffer->GetVertexCount()];
		mpVertexBones = new unsigned char[4 * mpVtxBuffer->GetVertexCount()];
		memset(mpVertexWeights, 0, 4 * mpVtxBuffer->GetVertexCount() * sizeof(float));

		for (int i=0; i < (int)mvVtxBonePairs.size(); i++)
		{
			cVertexBonePair &Pair = mvVtxBonePairs[i];

			float *pWeight = &mpVertexWeights[Pair.vtxIdx*4];
			unsigned char *pBoneIdx = &mpVertexBones[Pair.vtxIdx*4];
			int lPos=-1;
			//Find out where to add the next weight.
			for(int j=0; j<4;j++)
			{
				if(pWeight[j]==0){
					lPos = j;
					break;
				}
			}
			//If no place was found there are too many bones on the vertex.
			if(lPos==-1){
				Warning("More than 4 bones on a vertex!\n");
				continue;
			}

			pWeight[lPos] = Pair.weight;
			pBoneIdx[lPos] = Pair.boneIdx;
		}

		bool bUnconnectedVertexes=false;

		//Normalize the weights
		for(int vtx =0; vtx < mpVtxBuffer->GetVertexCount(); ++vtx)
		{
			float *pWeight = &mpVertexWeights[vtx*4];

			//check if the vertex is missing bone connection
			if(pWeight[0] == 0)
			{
				bUnconnectedVertexes=true;
				continue;
			}

			float fTotal=0;
			int lNum=0;
			while(pWeight[lNum]!=0 && lNum<=4)
			{
				fTotal += pWeight[lNum];
				lNum++;
			}
			for(int i=0; i<lNum; ++i)
			{
				pWeight[lNum] = pWeight[lNum] / fTotal;
			}
		}

		if(bUnconnectedVertexes)
		{
			Warning("Some vertices in sub mesh '%s' in mesh '%s' are not connected to a bone!\n",GetName().c_str(), mpParent->GetName().c_str());
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
}
