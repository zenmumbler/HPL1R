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

	const int BATCH_VERTEX_COUNT = 20'000;

	cGraphicsDrawer::cGraphicsDrawer(iLowLevelGraphics *apLowLevelGraphics,	cImageManager* apImageManager, cGpuProgramManager* programManager)
	: mpLowLevelGraphics{apLowLevelGraphics}
	, mpImageManager{apImageManager}
	, _programManager{programManager}
	, _batch(BATCH_VERTEX_COUNT, apLowLevelGraphics)
	{
		_program = _programManager->CreateProgram("Drawer.vert", "Drawer.frag");
		if (! _program) {
			FatalError("Could not create GraphicsDrawer program!");
		}
		_program->Bind();
		_program->SetTextureBindingIndex("image", 0);
		_program->UnBind();
	}

	//-----------------------------------------------------------------------

	cGraphicsDrawer::~cGraphicsDrawer()
	{
		for (auto obj : mvGfxObjects) {
			if (obj->isImage)
				mpImageManager->Destroy(obj->image);
		}
		_programManager->Destroy(_program);
		STLDeleteAll(mvGfxObjects);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cVector2l cGfxObject::GetSize() const {
		if (isImage) return image->GetSize();
		return cVector2l(texture->GetWidth(), texture->GetHeight());
	}

	cVector2f cGfxObject::GetFloatSize() const {
		auto size = GetSize();
		return { static_cast<float>(size.x), static_cast<float>(size.y) };
	}

	//-----------------------------------------------------------------------
	
	iTexture *cGfxBufferObject::GetTexture() const
	{
		return mpObject->isImage ? mpObject->image->GetTexture() : mpObject->texture;
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

	static void FlushImage(const cGfxObject* object)
	{
		if (object->isImage)
			object->image->GetFrameBitmap()->FlushToTexture();
	}

	void cGraphicsDrawer::DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos,
										const cVector2f& avSize, const cColor& aColor)
	{
		FlushImage(apObject);

		cGfxBufferObject BuffObj {
			.mpObject = apObject,
			.mvPosition = avPos,
			.mvSize = avSize,
			.mColor = aColor,
			.mbIsColorAndSize = true
		};

		m_setGfxBuffer.insert(BuffObj);
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos)
	{
		FlushImage(apObject);

		cGfxBufferObject BuffObj {
			.mpObject = apObject,
			.mvPosition = avPos,
			.mbIsColorAndSize = false
		};

		m_setGfxBuffer.insert(BuffObj);
	}
	
	//-----------------------------------------------------------------------

	void cGraphicsDrawer::UseMaterial(eGfxMaterial material) {
		switch (material) {
			case eGfxMaterial::DiffuseAlpha:
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
				break;

			case eGfxMaterial::DiffuseAdditive:
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_One);
				break;

			case eGfxMaterial::Smoke:
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_Zero, eBlendFunc_OneMinusSrcColor);
				break;

			case eGfxMaterial::Null:
				// reset to defaults
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
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
		_program->Bind();

		auto orthoDim = mpLowLevelGraphics->GetVirtualSize();
		cMatrixf orthoProjection = cMatrixf::CreateOrtho(0, orthoDim.x, orthoDim.y, 0, -1000, 1000);
		_program->SetMatrixf("projection", orthoProjection);

		eGfxMaterial material = eGfxMaterial::Null;
		iTexture *curTexture = nullptr;
		
		const auto renderBatch = [this]() {
			if (_batch.HasContent()) {
				// TODO: clean this up when upgrading to GL Core
				_batch.vertexBuffer->UpdateData(0xff, true);
				_batch.vertexBuffer->Bind();
				_batch.vertexBuffer->Draw();
				_batch.vertexBuffer->UnBind();
				_batch.Clear();
			}
		};

		for (const auto &pObj : m_setGfxBuffer)
		{
			// get next settings
			auto newMaterial = pObj.GetMaterial();
			auto newTexture = pObj.GetTexture();

			if (newMaterial != material || newTexture != curTexture) {
				renderBatch();

				// apply changes to shader
				if (newTexture != curTexture) {
					curTexture = newTexture;
					mpLowLevelGraphics->SetTexture(0, curTexture);
				}
				if (newMaterial != material) {
					material = newMaterial;
					UseMaterial(material);
				}
			}

			// add quad data
			auto [fX, fY, fZ] = pObj.mvPosition.v;
			auto objectSize = pObj.mpObject->GetFloatSize();
			auto [fW, fH] = pObj.mbIsColorAndSize ? pObj.mvSize.v : objectSize.v;
			auto color = pObj.mbIsColorAndSize ? pObj.mColor : cColor::White;
			cVector3f vPos[4] = {
				{ fX,      fY,      fZ },
				{ fX + fW, fY,      fZ },
				{ fX + fW, fY + fH, fZ },
				{ fX,      fY + fH, fZ }
			};

			for (int i = 0; i < 4; i++)
			{
				_batch.AddVertex(vPos[i], color, pObj.mpObject->uvs[i]);
			}
		}
		
		// render final batch, if any
		renderBatch();

		//Clear the buffer of objects.
		m_setGfxBuffer.clear();

		//Reset all states
		_program->UnBind();
		mpLowLevelGraphics->SetDepthTestActive(true);
		mpLowLevelGraphics->SetBlendActive(false);
		UseMaterial(eGfxMaterial::Null);
	}

	//-----------------------------------------------------------------------

	const cGfxObject* cGraphicsDrawer::CreateGfxObject(iTexture *texture, eGfxMaterial material)
	{
		return new cGfxObject{
			.sourceFile = "",
			.texture = texture,
			.material = material,
			.uvs = { {0,0}, {1,0}, {1,1}, {0,1} },
			.isImage = false,
			.isManaged = false
		};
	}

	//-----------------------------------------------------------------------

	const cGfxObject* cGraphicsDrawer::CreateGfxObject(const tString &asFileName, eGfxMaterial material)
	{
		cResourceImage* pImage = mpImageManager->CreateImage(asFileName);
		if (pImage == nullptr) {
			Error("Couldn't load image '%s'!\n", asFileName.c_str());
			return nullptr;
		}

		cGfxObject *go = new cGfxObject{
			.sourceFile = asFileName,
			.image = pImage,
			.material = material,
			.uvs = pImage->GetUVs(),
			.isImage = true,
			.isManaged = true
		};

		mvGfxObjects.push_back(go);
		return go;
	}

	//-----------------------------------------------------------------------

	const cGfxObject* cGraphicsDrawer::CreateUnmanagedGfxObject(const Bitmap &bitmap, eGfxMaterial material)
	{
		cResourceImage* pImage = mpImageManager->CreateFromBitmap(bitmap);
		if (pImage == nullptr) {
			Error("Couldn't create image from bitmap!\n");
			return nullptr;
		}

		return new cGfxObject{
			.sourceFile = "",
			.image = pImage,
			.material = material,
			.uvs = pImage->GetUVs(),
			.isImage = true,
			.isManaged = false
		};
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DestroyGfxObject(const cGfxObject* obj)
	{
		if (obj->isImage) {
			mpImageManager->Destroy(obj->image);
			if (obj->isManaged) {
				STLFindAndDelete(mvGfxObjects, obj);
			}
		}
	}

	//-----------------------------------------------------------------------

}
