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
#ifndef HPL_VERTEXBUFFER_VBO_H
#define HPL_VERTEXBUFFER_VBO_H

#include "graphics/VertexBuffer.h"


namespace hpl {

	class cVertexBufferVBO : public iVertexBuffer
	{
	public:
		cVertexBufferVBO(VertexAttributes attrs,
			VertexBufferPrimitiveType aDrawType,VertexBufferUsageType aUsageType,
			int alReserveVtxSize, int alReserveIdxSize);
		~cVertexBufferVBO();

		cVertexBufferVBO(const cVertexBufferVBO& vb) = delete;
		cVertexBufferVBO(cVertexBufferVBO&& vb) = delete;

		void AddVertex(VertexAttr attr,const cVector3f& avVtx) override;
		void AddColor(VertexAttr attr,const cColor& aColor) override;
		void AddIndex(unsigned int alIndex) override;

		bool GenerateTangents() override;
		void Transform(const cMatrixf &mtxTransform) override;

		// <-- start hw buffer section
		bool Compile() override;
		void UpdateData(VertexAttributes attrs, bool updateIndices) override;

		void Draw() override;
		void DrawWireframe() override;

		void Bind() override;
		void UnBind() override;
		// --> end hw buffer section

		iVertexBuffer* CreateCopy(VertexBufferUsageType aUsageType) override;

		cBoundingVolume CreateBoundingVolume() override;

		int GetVertexNum() override;
		int GetIndexNum() override;

		float* GetArray(VertexAttr attr) override;
		int GetArrayStride(VertexAttr attr) override;
		unsigned int* GetIndices() override;

		// these 2 methods will be deprecated
		void ResizeArray(VertexAttr attr, int newCount) override;
		void ResizeIndices(int newCount) override;

		//For debugging purposes
		cVector3f GetVector3(VertexAttr attr, unsigned index) override;
		cColor GetColor(VertexAttr attr, unsigned index) override;
		unsigned int GetIndex(unsigned index) override;

	private:
		tFloatVec mvVertexArray[VERTEX_ATTR_COUNT];
		unsigned int mvArrayHandle[VERTEX_ATTR_COUNT];

		tUIntVec mvIndexArray;
		unsigned int mlElementHandle;
	};

};

#endif // HPL_VERTEXBUFFER_VBO_H
