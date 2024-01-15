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
#include "resources/AtlasImageManager.h"
#include "resources/GpuProgramManager.h"
#include "resources/ResourceImage.h"
#include "resources/FrameBitmap.h"
#include "math/Math.h"
#include "system/STLHelpers.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	const int BATCH_VERTEX_COUNT = 20'000;

	cGraphicsDrawer::cGraphicsDrawer(iLowLevelGraphics *apLowLevelGraphics,	cAtlasImageManager* atlasImageMgr, cGpuProgramManager* programManager)
	: llGfx_{apLowLevelGraphics}
	, atlasImageMgr_{atlasImageMgr}
	, programManager_{programManager}
	, gfxBuffers_{}
	, batch_(BATCH_VERTEX_COUNT, apLowLevelGraphics)
	{
		program_ = programManager_->CreateProgram("Drawer.vert", "Drawer.frag");
		if (! program_) {
			FatalError("Could not create GraphicsDrawer main program!");
		}
		program_->Bind();
		program_->SetTextureBindingIndex("image", 0);
		program_->UnBind();

		glyphProgram_ = programManager_->CreateProgram("Drawer.vert", "Glyph.frag");
		if (! glyphProgram_) {
			FatalError("Could not create GraphicsDrawer glyph program!");
		}
		glyphProgram_->Bind();
		glyphProgram_->SetTextureBindingIndex("image", 0);
		glyphProgram_->UnBind();

	}

	//-----------------------------------------------------------------------

	cGraphicsDrawer::~cGraphicsDrawer()
	{
		for (auto obj : gfxObjects_) {
			atlasImageMgr_->Destroy(obj->image);
		}
		programManager_->Destroy(program_);
		programManager_->Destroy(glyphProgram_);
		STLDeleteAll(gfxObjects_);
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
		gfxBuffers_.insert(drawable);
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DrawTexture(iTexture *apTex, const cVector3f& avPos, const cVector2f& avSize, const cColor &aColor) {
		gfxBuffers_.insert({
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

		gfxBuffers_.insert({
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
			case eGfxMaterial::Text:
				llGfx_->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
				break;

			case eGfxMaterial::DiffuseAdditive:
				llGfx_->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_One);
				break;

			case eGfxMaterial::Smoke:
				llGfx_->SetBlendFunc(eBlendFunc_Zero, eBlendFunc_OneMinusSrcColor);
				break;

			case eGfxMaterial::Null:
				llGfx_->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
				break;
				
			default:
				break;
		}
	}
	
	//-----------------------------------------------------------------------

	void cGraphicsDrawer::Render()
	{
		// set all states
		llGfx_->SetDepthTestActive(false);
		llGfx_->SetBlendActive(true);

		auto orthoDim = llGfx_->GetVirtualSize();
		cMatrixf orthoProjection = cMatrixf::CreateOrtho(0, orthoDim.x, orthoDim.y, 0, -1, 1);

		// selection of main and text programs
		glyphProgram_->Bind();
		glyphProgram_->SetMatrixf("projection", orthoProjection);
		program_->Bind();
		program_->SetMatrixf("projection", orthoProjection);
		int currentProgram = 0;
		iGpuProgram* programs[2] = { program_, glyphProgram_ };

		eGfxMaterial material = eGfxMaterial::Null;
		iTexture *curTexture = nullptr;
		
		const auto renderBatch = [this]() {
			if (batch_.HasContent()) {
				// TODO: clean this up when upgrading to GL Core
				batch_.vertexBuffer->UpdateData(0xff, true);
				batch_.vertexBuffer->Bind();
				batch_.vertexBuffer->Draw();
				batch_.vertexBuffer->UnBind();
				batch_.Clear();
			}
		};

		for (const auto &pObj : gfxBuffers_)
		{
			// get next settings
			auto newMaterial = pObj.material;
			auto newTexture = pObj.texture;

			if (newMaterial != material || newTexture != curTexture) {
				renderBatch();

				// apply changes to shader
				if (newMaterial != material) {
					material = newMaterial;
					UseMaterial(material);
					int desiredProgram = newMaterial == eGfxMaterial::Text ? 1 : 0;
					if (desiredProgram != currentProgram) {
						programs[desiredProgram]->Bind();
						currentProgram = desiredProgram;
					}
				}
				if (newTexture != curTexture) {
					curTexture = newTexture;
					llGfx_->SetTexture(0, curTexture);
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
			batch_.AddVertex(vPos[0], color, pObj.uvs.uv0);
			batch_.AddVertex(vPos[1], color, pObj.uvs.uv1);
			batch_.AddVertex(vPos[2], color, pObj.uvs.uv2);

			batch_.AddVertex(vPos[2], color, pObj.uvs.uv2);
			batch_.AddVertex(vPos[3], color, pObj.uvs.uv3);
			batch_.AddVertex(vPos[0], color, pObj.uvs.uv0);
		}
		
		// render final batch, if any
		renderBatch();

		//Clear the buffer of objects.
		gfxBuffers_.clear();

		//Reset all states
		programs[currentProgram]->UnBind();
		llGfx_->SetDepthTestActive(true);
		llGfx_->SetBlendActive(false);
		UseMaterial(eGfxMaterial::Null);
	}

	//-----------------------------------------------------------------------

	const cGfxObject* cGraphicsDrawer::CreateGfxObject(const tString &asFileName, eGfxMaterial material)
	{
		cResourceImage* pImage = atlasImageMgr_->CreateImage(asFileName);
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

		gfxObjects_.push_back(go);
		return go;
	}

	//-----------------------------------------------------------------------

	void cGraphicsDrawer::DestroyGfxObject(const cGfxObject* obj)
	{
		atlasImageMgr_->Destroy(obj->image);
		STLFindAndDelete(gfxObjects_, obj);
	}

	//-----------------------------------------------------------------------

}
