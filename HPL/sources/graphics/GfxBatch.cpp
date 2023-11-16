/*
 * 2021-3 by zenmumbler
 * This file is part of Rehatched
 */

#include "GfxBatch.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"

namespace hpl {

	cGfxBatch::cGfxBatch(iLowLevelGraphics *llGfx) : cGfxBatch(20'000, llGfx) {}
	
	cGfxBatch::cGfxBatch(int vertexCount, iLowLevelGraphics *llGfx) {
		vertexBuffer = llGfx->CreateVertexBuffer(
			VertexMask_Position | VertexMask_Color0 | VertexMask_UV0,
			VertexBufferPrimitiveType::Triangles,
			VertexBufferUsageType::Stream,
			vertexCount, vertexCount
		);
		vertexBuffer->Compile();

		Clear();
	}

	cGfxBatch::~cGfxBatch() {
		delete vertexBuffer;
	}

	void cGfxBatch::AddVertex(const cVector2f &position, const cColor &color, const cVector2f& tex) {
		*posView++ = position;
		*colorView++ = color;
		*uvView++ = tex;
		*indexView++ = indexCount++;
		vertexBuffer->SetIndexCount(indexCount);
	}

	void cGfxBatch::Clear() {
		posView = vertexBuffer->GetVec3View(VertexAttr_Position);
		colorView = vertexBuffer->GetColorView(VertexAttr_Color0);
		uvView = vertexBuffer->GetVec2View(VertexAttr_UV0);
		indexView = vertexBuffer->GetIndexView();

		indexCount = 0;
		vertexBuffer->SetIndexCount(indexCount);
	}

}
