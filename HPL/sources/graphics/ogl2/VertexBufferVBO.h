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
		cVertexBufferVBO(VertexAttributes aFlags,
			VertexBufferPrimitiveType aDrawType,VertexBufferUsageType aUsageType,
			int alReserveVtxSize,int alReserveIdxSize);
		~cVertexBufferVBO();

		void AddVertex(VertexAttributes aType,const cVector3f& avVtx);
		void AddColor(VertexAttributes aType,const cColor& aColor);
		void AddIndex(unsigned int alIndex);

		bool Compile(VertexCompileOptions options);
		void UpdateData(VertexAttributes aTypes, bool abIndices);

		void Transform(const cMatrixf &mtxTransform);

		void Draw();
		void DrawWireframe();
		void DrawIndices(unsigned int *apIndices, int alCount);

		void Bind();
		void UnBind();

		iVertexBuffer* CreateCopy(VertexBufferUsageType aUsageType);

		cBoundingVolume CreateBoundingVolume();

		int GetVertexNum();
		int GetIndexNum();

		float* GetArray(VertexAttributes aType);
		unsigned int* GetIndices();

		void ResizeArray(VertexAttributes aType, int alSize);
		void ResizeIndices(int alSize);

		//For debugging purposes
		cVector3f GetVector3(VertexAttributes aType, unsigned alIdx);
		cVector3f GetVector4(VertexAttributes aType, unsigned alIdx);
		cColor GetColor(VertexAttributes aType, unsigned alIdx);
		unsigned int GetIndex(VertexAttributes aType, unsigned alIdx);

	private:
		int GetElementNum(VertexAttributes aFlag);

		void SetVertexStates(VertexAttributes aFlags);

		unsigned int mlElementHandle;

		tFloatVec mvVertexArray[klNumOfVertexFlags];

		unsigned int mvArrayHandle[klNumOfVertexFlags];

		tUIntVec mvIndexArray;

		bool mbHasShadowDouble;

		bool mbCompiled;
	};

};

#endif // HPL_VERTEXBUFFER_VBO_H
