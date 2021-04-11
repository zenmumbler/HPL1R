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

#include "graphics/GraphicsDrawer.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/FrameBitmap.h"
#include "graphics/GfxObject.h"

#include "math/Math.h"

#include "resources/ResourceImage.h"
#include "resources/ImageManager.h"

#include "graphics/Material_Smoke2D.h"
#include "graphics/Material_DiffuseAdditive2D.h"
#include "graphics/Material_DiffuseAlpha2D.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGraphicsDrawer::cGraphicsDrawer(iLowLevelGraphics *apLowLevelGraphics,	cImageManager* apImageManager)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
		mpImageManager = apImageManager;
	}

	//-----------------------------------------------------------------------

	cGraphicsDrawer::~cGraphicsDrawer()
	{
		STLDeleteAll(mlstGfxObjects);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iOldMaterial* cGfxBufferObject::GetMaterial() const
	{
		return static_cast<iOldMaterial*>(mpObject->GetMaterial());
	}

	//-----------------------------------------------------------------------

	bool cGfxBufferCompare::operator()(const cGfxBufferObject& aObjectA,const cGfxBufferObject& aObjectB) const
	{
		if(aObjectA.GetZ() != aObjectB.GetZ())
		{
			return aObjectA.GetZ() < aObjectB.GetZ();
		}
		else if(aObjectA.GetMaterial()->GetTexture() !=
			aObjectB.GetMaterial()->GetTexture())
		{
			return aObjectA.GetMaterial()->GetTexture() >
				aObjectB.GetMaterial()->GetTexture();
		}
		else
		{

		}
		return false;
	}


	//-----------------------------------------------------------------------

	static void FlushImage(cGfxObject* apObject)
	{
		cResourceImage *pImage = apObject->GetMaterial()->GetImage();
		pImage->GetFrameBitmap()->FlushToTexture();
	}

	void cGraphicsDrawer::DrawGfxObject(cGfxObject* apObject, const cVector3f& avPos,
										const cVector2f& avSize, const cColor& aColor)
	{
		FlushImage(apObject);

		cGfxBufferObject BuffObj;
		BuffObj.mpObject = apObject;
		BuffObj.mvTransform = avPos;
		BuffObj.mvSize= avSize;
		BuffObj.mColor = aColor;

		BuffObj.mbIsColorAndSize = true;

		m_setGfxBuffer.insert(BuffObj);
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DrawGfxObject(cGfxObject* apObject, const cVector3f& avPos)
	{
		FlushImage(apObject);

		cGfxBufferObject BuffObj;
		BuffObj.mpObject = apObject;
		BuffObj.mvTransform = avPos;
		BuffObj.mbIsColorAndSize = false;

		m_setGfxBuffer.insert(BuffObj);
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DrawAll()
	{
		//Set all states
		mpLowLevelGraphics->SetDepthTestActive(false);
		mpLowLevelGraphics->SetIdentityMatrix(eMatrix_ModelView);

		mpLowLevelGraphics->SetOrthoProjection(mpLowLevelGraphics->GetVirtualSize(),-1000,1000);

		int lIdxAdd=0;
		iOldMaterial *pPrevMat=NULL;
		iOldMaterial *pMat =NULL;
		const cGfxBufferObject* pObj=NULL;
		tGfxBufferSetIt ObjectIt = m_setGfxBuffer.begin();

		if(ObjectIt != m_setGfxBuffer.end())
			pMat = ObjectIt->GetMaterial();

		while(ObjectIt != m_setGfxBuffer.end())
		{
			pMat->StartRendering(mpLowLevelGraphics);

			do
			{
				pObj = &(*ObjectIt);
				if(pObj->mbIsColorAndSize)
				{
					auto [fX, fY, fZ] = pObj->mvTransform.v;
					float fW = pObj->mvSize.x;
					float fH = pObj->mvSize.y;
					cVector3f vPos[4] = {
						{ fX,      fY,      fZ },
						{ fX + fW, fY,      fZ },
						{ fX + fW, fY + fH, fZ },
						{ fX,      fY + fH, fZ }
					};

					mpLowLevelGraphics->AddVertexToBatch_Size2D(pObj->mpObject->GetVtxPtr(0),
																&vPos[0],
																&pObj->mColor);
					mpLowLevelGraphics->AddVertexToBatch_Size2D(pObj->mpObject->GetVtxPtr(1),
																&vPos[1],
																&pObj->mColor);
					mpLowLevelGraphics->AddVertexToBatch_Size2D(pObj->mpObject->GetVtxPtr(2),
																&vPos[2],
																&pObj->mColor);
					mpLowLevelGraphics->AddVertexToBatch_Size2D(pObj->mpObject->GetVtxPtr(3),
																&vPos[3],
																&pObj->mColor);

					for(int i=0;i<4;i++)
						mpLowLevelGraphics->AddIndexToBatch(lIdxAdd + i);
				}
				else
				{
					for(int i=0;i<(int)pObj->mpObject->GetVertexVec()->size();i++)
					{
						mpLowLevelGraphics->AddVertexToBatch(pObj->mpObject->GetVtxPtr(i),
															&pObj->mvTransform);
						mpLowLevelGraphics->AddIndexToBatch(lIdxAdd + i);
					}
				}
				lIdxAdd+=(int)pObj->mpObject->GetVertexVec()->size();

				pPrevMat = pMat;
				ObjectIt++;

				if(ObjectIt == m_setGfxBuffer.end()){
					pMat=NULL;
					break;
				}
				else{
					pMat = ObjectIt->GetMaterial();
				}
			}
			while(	pMat->mType == pPrevMat->mType
					&&
					pMat->GetTexture() == pPrevMat->GetTexture()
				);

			lIdxAdd =0;

			mpLowLevelGraphics->DrawBatch(eVtxBatchFlag_Position | eVtxBatchFlag_Texture0 | eVtxBatchFlag_Color0, eBatchDrawMode_Quads);
			mpLowLevelGraphics->ClearBatch();

			pPrevMat->EndRendering(mpLowLevelGraphics);
		}

		//Clear the buffer of objects.
		m_setGfxBuffer.clear();

		//Reset all states
		mpLowLevelGraphics->SetDepthTestActive(true);
	}

	//-----------------------------------------------------------------------

	iOldMaterial* cGraphicsDrawer::CreateMaterial(eOldMaterialType type, cResourceImage* apImage) {
		iOldMaterial *pMat = NULL;

		if (type == eOldMaterialType::DiffuseAlpha) {
			pMat = hplNew(cMaterial_DiffuseAlpha2D, (mpImageManager));
		}
		else if (type == eOldMaterialType::DiffuseAdditive) {
			pMat = hplNew(cMaterial_DiffuseAdditive2D, (mpImageManager));
		}
		else if (type == eOldMaterialType::Smoke) {
			pMat = hplNew(cMaterial_Smoke2D, (mpImageManager));
		}
		
		if (pMat != NULL) {
			pMat->SetImage(apImage);
		}
		return pMat;
	}

	//-----------------------------------------------------------------------

	cGfxObject* cGraphicsDrawer::CreateGfxObject(const tString &asFileName, eOldMaterialType type,
												bool abAddToList)
	{
		cResourceImage* pImage = mpImageManager->CreateImage(asFileName);
		if(pImage==NULL){
			FatalError("Couldn't load image '%s'!\n", asFileName.c_str());
			return NULL;
		}

		iOldMaterial* pMat = CreateMaterial(type, pImage);
		if(pMat==NULL){
			FatalError("Couldn't create material '%d'!\n", type);
			return NULL;
		}

		cGfxObject* pObject = hplNew(cGfxObject,(pMat, asFileName));

		if(abAddToList) mlstGfxObjects.push_back(pObject);

		return pObject;
	}

	//-----------------------------------------------------------------------

	cGfxObject* cGraphicsDrawer::CreateGfxObject(iBitmap2D *apBmp, eOldMaterialType type,
												bool abAddToList)
	{
		cResourceImage* pImage = mpImageManager->CreateFromBitmap(apBmp);
		if(pImage==NULL){
			FatalError("Couldn't create image\n");
			return NULL;
		}

		iOldMaterial* pMat = CreateMaterial(type, pImage);
		if(pMat==NULL){
			FatalError("Couldn't create material '%d'!\n", type);
			return NULL;
		}

		cGfxObject* pObject = hplNew(cGfxObject,(pMat, ""));

		if(abAddToList) mlstGfxObjects.push_back(pObject);

		return pObject;
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DestroyGfxObject(cGfxObject* apObject)
	{
		STLFindAndDelete(mlstGfxObjects,apObject);
	}

	//-----------------------------------------------------------------------

}
