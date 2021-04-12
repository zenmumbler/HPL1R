/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_GFXBATCH_H
#define HPL_GFXBATCH_H

#include "graphics/GraphicsTypes.h"

namespace hpl {
	
	typedef uint32_t tGfxBatchAttrs;

	enum eGfxBatchAttr {
		eGfxBatchAttr_Position =	0x0001,
		eGfxBatchAttr_Normal =		0x0002,
		eGfxBatchAttr_Color0 =		0x0004,
		eGfxBatchAttr_Texture0 =	0x0008,
		eGfxBatchAttr_Texture1 =	0x0010,
		eGfxBatchAttr_Texture2 =	0x0020
	};

	enum eBatchDrawMode
	{
		eBatchDrawMode_Tris,
		eBatchDrawMode_Quads
	};

	class cGfxBatch
	{
	public:
		cGfxBatch();
		cGfxBatch(int vertexCount);
		~cGfxBatch();

		void AddVertex(const cVertex &vertex, const cVector3f &offset);
		void AddVertexExt(const cVertex &vertex, const cVector3f &position, const cColor &color);
		void AddIndex(uint32_t index);
		void Clear();

		float* mpVertexArray;
		unsigned int mlVertexCount;
		unsigned int* mpIndexArray;
		unsigned int mlIndexCount;

		unsigned int mlBatchStride;
		unsigned int mlBatchArraySize;
	};
}

#endif /* HPL_GFXBATCH_H */
