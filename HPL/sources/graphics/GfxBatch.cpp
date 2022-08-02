/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */

#include "GfxBatch.h"

namespace hpl {

	cGfxBatch::cGfxBatch() : cGfxBatch(20'000) {}
	
	cGfxBatch::cGfxBatch(int vertexCount) {
		mlBatchArraySize = vertexCount;
		mlVertexCount = 0;
		mlIndexCount = 0;

		// 3 pos floats, 4 color floats, 2 tex coord floats
		mlBatchStride = 9;
		mpVertexArray = new float[mlBatchStride * mlBatchArraySize];
		mpIndexArray = new unsigned int[mlBatchArraySize];
	}

	cGfxBatch::~cGfxBatch() {
		delete[] mpVertexArray;
		delete[] mpIndexArray;
	}

	void cGfxBatch::AddVertex(const cVector3f &position, const cColor &color, const cVector3f& tex) {
		//Coord
		mpVertexArray[mlVertexCount + 0] =	position.x;
		mpVertexArray[mlVertexCount + 1] =	position.y;
		mpVertexArray[mlVertexCount + 2] =	position.z;

		//Color
		mpVertexArray[mlVertexCount + 3] =	color.r;
		mpVertexArray[mlVertexCount + 4] =	color.g;
		mpVertexArray[mlVertexCount + 5] =	color.b;
		mpVertexArray[mlVertexCount + 6] =	color.a;

		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	tex.x;
		mpVertexArray[mlVertexCount + 8] =	tex.y;

		mlVertexCount = mlVertexCount + mlBatchStride;

		if (mlVertexCount / mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	void cGfxBatch::AddIndex(uint32_t index) {
		mpIndexArray[mlIndexCount] = index;
		mlIndexCount++;

		if (mlIndexCount >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	void cGfxBatch::Clear() {
		mlIndexCount = 0;
		mlVertexCount = 0;
	}

}
