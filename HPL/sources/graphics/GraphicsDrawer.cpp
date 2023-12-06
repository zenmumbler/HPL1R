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

#include "system/STLHelpers.h"
#include "system/Log.h"
#include "graphics/GraphicsDrawer.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/ResourceImage.h"
#include "resources/FrameBitmap.h"
#include "math/Math.h"

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
	, m_setGfxBuffer{}
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
		return image->GetSize();
	}

	cVector2f cGfxObject::GetFloatSize() const {
		auto size = GetSize();
		return { static_cast<float>(size.x), static_cast<float>(size.y) };
	}

	//-----------------------------------------------------------------------
	
	bool cGfxBufferCompare::operator()(const GfxDrawable& aObjectA, const GfxDrawable& aObjectB) const
	{
		float zA = aObjectA.mvPosition.z;
		float zB = aObjectB.mvPosition.z;
		if (zA != zB)
		{
			return zA < zB;
		}
		else if (aObjectA.texture != aObjectB.texture)
		{
			return aObjectA.texture > aObjectB.texture;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	static void FlushImage(const cGfxObject* object)
	{
		object->image->GetFrameBitmap()->FlushToTexture();
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::Draw(GfxDrawable drawable) {
		m_setGfxBuffer.insert(drawable);
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DrawTexture(iTexture *apTex, const cVector3f& avPos, const cVector2f& avSize, const cColor &aColor) {
		m_setGfxBuffer.insert({
			.texture = apTex,
			.mvPosition = avPos,
			.mvSize = avSize,
			.mColor = aColor,
		});
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos,
										const cVector2f& avSize, const cColor& aColor)
	{
		FlushImage(apObject);

		m_setGfxBuffer.insert({
			.texture = apObject->image->GetTexture(),
			.material = apObject->material,
			.mvPosition = avPos,
			.mvSize = avSize,
			.mColor = aColor,
			.uvs = apObject->uvs
		});
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos)
	{
		DrawGfxObject(apObject, avPos, apObject->GetFloatSize(), cColor::White);
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
				mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
				break;
				
			default:
				break;
		}
	}
	
	//-----------------------------------------------------------------------

	void cGraphicsDrawer::Render()
	{
		//Set all states
		mpLowLevelGraphics->SetDepthTestActive(false);
		mpLowLevelGraphics->SetBlendActive(true);
		_program->Bind();

		auto orthoDim = mpLowLevelGraphics->GetVirtualSize();
		cMatrixf orthoProjection = cMatrixf::CreateOrtho(0, orthoDim.x, orthoDim.y, 0, -1, 1);
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
			auto newMaterial = pObj.material;
			auto newTexture = pObj.texture;

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

			// determine object attributes
			auto [fX, fY, fZ] = pObj.mvPosition.v;
			auto [fW, fH] = pObj.mvSize.v;
			auto color = pObj.mColor;

			cVector2f vPos[4] = {
				{ fX,      fY      },
				{ fX + fW, fY      },
				{ fX + fW, fY + fH },
				{ fX,      fY + fH }
			};

			// add quad as 2 triangles
			_batch.AddVertex(vPos[0], color, pObj.uvs.uv0);
			_batch.AddVertex(vPos[1], color, pObj.uvs.uv1);
			_batch.AddVertex(vPos[2], color, pObj.uvs.uv2);

			_batch.AddVertex(vPos[2], color, pObj.uvs.uv2);
			_batch.AddVertex(vPos[3], color, pObj.uvs.uv3);
			_batch.AddVertex(vPos[0], color, pObj.uvs.uv0);
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
		};

		mvGfxObjects.push_back(go);
		return go;
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DestroyGfxObject(const cGfxObject* obj)
	{
		mpImageManager->Destroy(obj->image);
		STLFindAndDelete(mvGfxObjects, obj);
	}

	//-----------------------------------------------------------------------

}
