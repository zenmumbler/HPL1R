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
#include "graphics/LowLevelGraphics.h"
#include "resources/ResourceImage.h"
#include "resources/FrameBitmap.h"
#include "math/Math.h"
#include "system/Log.h"

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
		for (auto pGo : mvGfxObjects) {
			mpImageManager->Destroy(pGo->mpImage);
		}
		STLDeleteAll(mvGfxObjects);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	iTexture *cGfxBufferObject::GetTexture() const
	{
		return mpObject->mpImage->GetTexture();
	}

	bool cGfxBufferCompare::operator()(const cGfxBufferObject& aObjectA, const cGfxBufferObject& aObjectB) const
	{
		if (aObjectA.GetZ() != aObjectB.GetZ())
		{
			return aObjectA.GetZ() < aObjectB.GetZ();
		}
		else if (aObjectA.GetTexture() != aObjectB.GetTexture())
		{
			return aObjectA.GetTexture() > aObjectB.GetTexture();
		}
		return false;
	}

	//-----------------------------------------------------------------------

	static void FlushImage(const cGfxObject* apObject)
	{
		apObject->mpImage->GetFrameBitmap()->FlushToTexture();
	}

	void cGraphicsDrawer::DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos,
										const cVector2f& avSize, const cColor& aColor)
	{
		FlushImage(apObject);

		cGfxBufferObject BuffObj;
		BuffObj.mpObject = apObject;
		BuffObj.mvPosition = avPos;
		BuffObj.mvSize= avSize;
		BuffObj.mColor = aColor;

		BuffObj.mbIsColorAndSize = true;

		m_setGfxBuffer.insert(BuffObj);
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos)
	{
		FlushImage(apObject);

		cGfxBufferObject BuffObj;
		BuffObj.mpObject = apObject;
		BuffObj.mvPosition = avPos;
		BuffObj.mbIsColorAndSize = false;

		m_setGfxBuffer.insert(BuffObj);
	}
	
	//-----------------------------------------------------------------------

	void cGraphicsDrawer::UseMaterialType(eGfxMaterialType matType) {
		switch (matType) {
			case eGfxMaterialType::DiffuseAlpha:
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
				break;

			case eGfxMaterialType::DiffuseAdditive:
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_One);
				mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Modulate);
				break;

			case eGfxMaterialType::Smoke:
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_Zero,eBlendFunc_OneMinusSrcColor);
				mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorOp1,eTextureOp_Color);
				mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Modulate);
				break;

			case eGfxMaterialType::Null:
				// reset to defaults
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
				mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorOp1,eTextureOp_Color);
				mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Modulate);
				break;
				
			default:
				break;
		}
	}
	
	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DrawAll()
	{
		//Set all states
		mpLowLevelGraphics->SetDepthTestActive(false);
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetIdentityMatrix(eMatrix_ModelView);
		mpLowLevelGraphics->SetOrthoProjection(mpLowLevelGraphics->GetVirtualSize(), -1000, 1000);

		int lIdxAdd = 0;
		eGfxMaterialType matType = eGfxMaterialType::Null;
		iTexture *curTexture = nullptr;
		
		const auto renderBatch = [this, &lIdxAdd]() {
			if (lIdxAdd > 0)
			{
				mpLowLevelGraphics->DrawBatch(mBatch, eGfxBatchAttr_Position | eGfxBatchAttr_Texture0 | eGfxBatchAttr_Color0, eBatchDrawMode_Quads);
				mBatch.Clear();
				lIdxAdd = 0;
			}
		};

		for (const auto &pObj : m_setGfxBuffer)
		{
			// get next settings
			auto newMatType = pObj.GetMaterialType();
			auto newTexture = pObj.GetTexture();

			if (newMatType != matType || newTexture != curTexture) {
				renderBatch();

				// apply changes to shader
				if (newTexture != curTexture) {
					curTexture = newTexture;
					mpLowLevelGraphics->SetTexture(0, curTexture);
				}
				if (newMatType != matType) {
					matType = newMatType;
					UseMaterialType(matType);
				}
			}

			// add quad data
			if(pObj.mbIsColorAndSize)
			{
				auto [fX, fY, fZ] = pObj.mvPosition.v;
				auto [fW, fH] = pObj.mvSize.v;
				cVector3f vPos[4] = {
					{ fX,      fY,      fZ },
					{ fX + fW, fY,      fZ },
					{ fX + fW, fY + fH, fZ },
					{ fX,      fY + fH, fZ }
				};

				for (int i = 0; i < 4; i++)
				{
					const auto& vtx = pObj.mpObject->mvVtx[i];
					mBatch.AddVertex(vPos[i], pObj.mColor, vtx.tex);
					mBatch.AddIndex(lIdxAdd++);
				}
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					const auto& vtx = pObj.mpObject->mvVtx[i];
					mBatch.AddVertex(vtx.pos + pObj.mvPosition, vtx.col, vtx.tex);
					mBatch.AddIndex(lIdxAdd++);
				}
			}
		}
		
		// render final batch, if any
		renderBatch();

		//Clear the buffer of objects.
		m_setGfxBuffer.clear();

		//Reset all states
		mpLowLevelGraphics->SetDepthTestActive(true);
		mpLowLevelGraphics->SetBlendActive(false);
		UseMaterialType(eGfxMaterialType::Null);
	}

	//-----------------------------------------------------------------------

	const cGfxObject* cGraphicsDrawer::CreateGfxObject(const tString &asFileName, eGfxMaterialType matType)
	{
		cResourceImage* pImage = mpImageManager->CreateImage(asFileName);
		if (pImage == nullptr) {
			Error("Couldn't load image '%s'!\n", asFileName.c_str());
			return nullptr;
		}

		cGfxObject *go = new cGfxObject({asFileName, matType, pImage, pImage->GetVertexVecCopy(0, -1) });
		mvGfxObjects.push_back(go);
		return go;
	}

	//-----------------------------------------------------------------------

	const cGfxObject* cGraphicsDrawer::CreateUnmanagedGfxObject(const Bitmap &bmp, eGfxMaterialType matType)
	{
		cResourceImage* pImage = mpImageManager->CreateFromBitmap(bmp);
		if(pImage == nullptr){
			Error("Couldn't create image from bitmap!\n");
			return nullptr;
		}

		return new cGfxObject({ "", matType, pImage, pImage->GetVertexVecCopy(0, -1) });
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DestroyGfxObject(const cGfxObject* apObject)
	{
		mpImageManager->Destroy(apObject->mpImage);
		
		if (apObject->msSourceFile.length() > 0) {
			STLFindAndDelete(mvGfxObjects, apObject);
		}
	}

	//-----------------------------------------------------------------------

}
