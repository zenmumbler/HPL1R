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
		mpVertexArray = hplNewArray(float, mlBatchStride * mlBatchArraySize);
		mpIndexArray = hplNewArray(unsigned int, mlBatchArraySize);
	}

	cGfxBatch::~cGfxBatch() {
		hplDeleteArray(mpVertexArray);
		hplDelete(mpIndexArray);
	}

	void cGfxBatch::AddVertex(const cVertex &vertex, const cVector3f &offset) {
		//Coord
		mpVertexArray[mlVertexCount + 0] =	vertex.pos.x + offset.x;
		mpVertexArray[mlVertexCount + 1] =	vertex.pos.y + offset.y;
		mpVertexArray[mlVertexCount + 2] =	vertex.pos.z + offset.z;

		//Color
		mpVertexArray[mlVertexCount + 3] =	vertex.col.r;
		mpVertexArray[mlVertexCount + 4] =	vertex.col.g;
		mpVertexArray[mlVertexCount + 5] =	vertex.col.b;
		mpVertexArray[mlVertexCount + 6] =	vertex.col.a;

		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	vertex.tex.x;
		mpVertexArray[mlVertexCount + 8] =	vertex.tex.y;

		mlVertexCount = mlVertexCount + mlBatchStride;

		if (mlVertexCount / mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	void cGfxBatch::AddVertexExt(const cVertex &vertex, const cVector3f &position, const cColor &color) {
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
		mpVertexArray[mlVertexCount + 7] =	vertex.tex.x;
		mpVertexArray[mlVertexCount + 8] =	vertex.tex.y;

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
