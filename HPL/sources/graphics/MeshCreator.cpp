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
#include "graphics/MeshCreator.h"
#include "system/String.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "resources/Resources.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "resources/MaterialManager.h"
#include "resources/AnimationManager.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMeshCreator::cMeshCreator(iLowLevelGraphics *apLowLevelGraphics, cResources *apResources)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
		mpResources = apResources;
	}

	//-----------------------------------------------------------------------

	cMeshCreator::~cMeshCreator()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMesh* cMeshCreator::CreateBox(const tString &asName,cVector3f avSize, const tString &asMaterial)
	{
		cMesh *pMesh = new cMesh(asName, mpResources->GetMaterialManager(), mpResources->GetAnimationManager());

		cSubMesh *pSubMesh = pMesh->CreateSubMesh("Main");

		iMaterial *pMat = mpResources->GetMaterialManager()->CreateMaterial(asMaterial);
		pSubMesh->SetMaterial(pMat);
		iVertexBuffer *pVtxBuff = CreateBoxVertexBuffer(avSize);
		pSubMesh->SetVertexBuffer(pVtxBuff);

		return pMesh;
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cMeshCreator::CreateSkyBoxVertexBuffer(float afSize)
	{
		auto pSkyBox = mpLowLevelGraphics->CreateVertexBuffer(
			VertexMask_Color0 | VertexMask_Position | VertexMask_UV0,
			VertexBufferPrimitiveType::Quads, VertexBufferUsageType::Static,
			24, 24
		);

		float fSize = afSize;

		auto posView = pSkyBox->GetVec3View(VertexAttr_Position);
		auto colorView = pSkyBox->GetColorView(VertexAttr_Color0);
		auto uvView = pSkyBox->GetVec2View(VertexAttr_UV0);

		for(int x=-1; x<=1;x++)
			for(int y=-1; y<=1;y++)
				for(int z=-1; z<=1;z++)
				{
					if(x==0 && y==0 && z==0)continue;
					if(std::abs(x) + std::abs(y) + std::abs(z) > 1)continue;

					//Direction (could say inverse normal) of the quad.
					cVector3f vDir;
					cVector3f vSide;

					cVector3f vAdd[4];
					if(std::abs(x)){
						vDir.x = (float)x;

						vAdd[0].y =  1; vAdd[0].z =  1;
						vAdd[1].y = -1; vAdd[1].z =  1;
						vAdd[2].y = -1; vAdd[2].z = -1;
						vAdd[3].y =  1; vAdd[3].z = -1;
					}
					else if(std::abs(y)){
						vDir.y = (float)y;

						vAdd[0].z =  1; vAdd[0].x =  1;
						vAdd[1].z = -1; vAdd[1].x =  1;
						vAdd[2].z = -1; vAdd[2].x = -1;
						vAdd[3].z =  1; vAdd[3].x = -1;
					}
					else if(std::abs(z)){
						vDir.z = (float)z;

						vAdd[0].y =  1; vAdd[0].x =  1;
						vAdd[1].y =  1; vAdd[1].x = -1;
						vAdd[2].y = -1; vAdd[2].x = -1;
						vAdd[3].y = -1; vAdd[3].x =  1;
					}

					//Log("Side: (%.0f : %.0f : %.0f) [ ", vDir.x,  vDir.y,vDir.z);
					for (int i=0; i < 4; i++)
					{
						int idx = i;
						if (x + y + z < 0) idx = 3-i;

						*colorView++ = { 1, 1, 1, 1 };
						*posView++ = (vDir + vAdd[idx]) * fSize;
						*uvView++ = (vDir + vAdd[idx]).xy;

						vSide = vDir + vAdd[idx];
						//Log("%d: (%.1f : %.1f : %.1f) ", i, vSide.x, vSide.y, vSide.z);
					}
					//Log("\n");
				}

		auto indexView = pSkyBox->GetIndexView();
		for (int i=0; i < 24; i++) *indexView++ = i;

		if(!pSkyBox->Compile())
		{
			delete pSkyBox;
			return NULL;
		}
		return pSkyBox;
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cMeshCreator::CreateBoxVertexBuffer(cVector3f avSize)
	{
		iVertexBuffer* pBox = mpLowLevelGraphics->CreateVertexBuffer(
			VertexMask_Color0 | VertexMask_Position | VertexMask_UV0 |
			VertexMask_Tangent | VertexMask_Normal,
			VertexBufferPrimitiveType::Triangles, VertexBufferUsageType::Static,
			24, 36);

		auto posView = pBox->GetVec3View(VertexAttr_Position);
		auto colorView = pBox->GetColorView(VertexAttr_Color0);
		auto uvView = pBox->GetVec2View(VertexAttr_UV0);
		auto normalView = pBox->GetVec3View(VertexAttr_Normal);
		auto indexView = pBox->GetIndexView();

		avSize = avSize * 0.5;
		int lVtxIdx = 0;

		for(int x=-1; x<=1;x++)
			for(int y=-1; y<=1;y++)
				for(int z=-1; z<=1;z++)
				{
					if (x==0 && y==0 && z==0) continue;
					if (std::abs(x) + std::abs(y) + std::abs(z) > 1) continue;

					//Direction (could say inverse normal) of the quad.
					cVector3f vDir;
					cVector3f vSide;

					cVector3f vAdd[4];
					if(std::abs(x)) {
						vDir.x = (float)x;

						vAdd[0].y =  1; vAdd[0].z =  1;
						vAdd[1].y = -1; vAdd[1].z =  1;
						vAdd[2].y = -1; vAdd[2].z = -1;
						vAdd[3].y =  1; vAdd[3].z = -1;
					}
					else if(std::abs(y)) {
						vDir.y = (float)y;

						vAdd[0].z =  1; vAdd[0].x =  1;
						vAdd[1].z = -1; vAdd[1].x =  1;
						vAdd[2].z = -1; vAdd[2].x = -1;
						vAdd[3].z =  1; vAdd[3].x = -1;
					}
					else if(std::abs(z)) {
						vDir.z = (float)z;

						vAdd[0].y =  1; vAdd[0].x =  1;
						vAdd[1].y =  1; vAdd[1].x = -1;
						vAdd[2].y = -1; vAdd[2].x = -1;
						vAdd[3].y = -1; vAdd[3].x =  1;
					}

					//Log("Side: (%.0f : %.0f : %.0f) [ ", vDir.x,  vDir.y,vDir.z);
					for(int i=0;i<4;i++)
					{
						int idx = GetBoxIdx(i,x,y,z);
						auto vTex = GetBoxTex(i,x,y,z,vAdd);

						*colorView++ = cColor::White;
						*posView++ = (vDir + vAdd[idx]) * avSize;
						*normalView++ = vDir;

						//texture coord
						cVector2f vCoord = { (vTex.x+1) * 0.5f, (vTex.y+1) * 0.5f };
						*uvView++ = vCoord;

						vSide = vDir + vAdd[idx];
						//Log("%d: Tex: (%.1f : %.1f : %.1f) ", i,vTex.x,  vTex.y,vTex.z);
						//Log("%d: (%.1f : %.1f : %.1f) ", i,vSide.x,  vSide.y,vSide.z);
					}

					*indexView++ = lVtxIdx + 0;
					*indexView++ = lVtxIdx + 1;
					*indexView++ = lVtxIdx + 2;

					*indexView++ = lVtxIdx + 2;
					*indexView++ = lVtxIdx + 3;
					*indexView++ = lVtxIdx + 0;

					lVtxIdx += 4;

					//Log("\n");
				}

		pBox->GenerateTangents();
		if (! pBox->Compile())
		{
			delete pBox;
			return NULL;
		}
		return pBox;
	}

	cVector2f cMeshCreator::GetBoxTex(int i,int x, int y, int z, cVector3f *vAdd)
	{
		cVector2f vTex;

		if(std::abs(x)){
			vTex.x = vAdd[i].z;
			vTex.y = vAdd[i].y;
		}
		else if(std::abs(y)){
			vTex.x = vAdd[i].x;
			vTex.y = vAdd[i].z;
		}
		else if(std::abs(z)){
			vTex.x = vAdd[i].x;
			vTex.y = vAdd[i].y;
		}

		//Inverse for negative directions
		if(x+y+z <0)
		{
			vTex.x = -vTex.x;
			vTex.y = -vTex.y;
		}

		return vTex;
	}

	int cMeshCreator::GetBoxIdx(int i,int x, int y, int z)
	{
		int idx = i;
		if(x + y + z > 0) idx = 3-i;

		return idx;
	}

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------

}
