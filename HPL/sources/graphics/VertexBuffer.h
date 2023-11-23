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
#ifndef HPL_VERTEXBUFFER_H
#define HPL_VERTEXBUFFER_H

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "math/BoundingVolume.h"
#include "system/STLBufferView.h"

namespace hpl {

	enum class VertexBufferPrimitiveType {
		Triangles,
		Quads,
		LineStrips
	};

	enum class VertexBufferUsageType {
		Static,
		Dynamic,
		Stream
	};

	enum VertexAttr {
		VertexAttr_Position = 0,
		VertexAttr_Normal,
		VertexAttr_Color0,
		VertexAttr_UV0,
		VertexAttr_Tangent
	};

	using VertexAttributes = tFlag;

	enum VertexAttributeMask {
		VertexMask_Position = 1,
		VertexMask_Normal	= 2,
		VertexMask_Color0	= 4,
		VertexMask_UV0      = 8,
		VertexMask_Tangent  = 16
	};

	const int VERTEX_ATTR_COUNT = 5;


	class iVertexBuffer
	{
	public:
		iVertexBuffer(VertexAttributes attrs,
			VertexBufferPrimitiveType aDrawType,
			VertexBufferUsageType aUsageType
		) :
			mVertexFlags(attrs),
			mDrawType(aDrawType), mUsageType(aUsageType), _indexCount(-1)
		{}

		virtual ~iVertexBuffer() = default;

		virtual bool GenerateTangents() = 0;
		virtual bool Compile() = 0;
		virtual void UpdateData(VertexAttributes attrs, bool updateIndices) = 0;

		/**
		 * Transform the entire buffer with transform.
		*/
		virtual void Transform(const cMatrixf &mtxTransform)=0;

		virtual void Draw() = 0;
		virtual void DrawWireframe() = 0;

		virtual void Bind()=0;
		virtual void UnBind()=0;

		virtual iVertexBuffer* CreateCopy(VertexBufferUsageType aUsageType)=0;

		virtual cBoundingVolume CreateBoundingVolume() = 0;

		virtual float* GetArray(VertexAttr attr) = 0;
		virtual int GetArrayStride(VertexAttr attr) = 0;
		virtual unsigned int* GetIndices()=0;

		virtual int GetVertexCount() = 0;
		virtual int GetMaxIndexCount() = 0; // maximum number of indices this buffer can hold
		virtual int GetIndexCount() = 0;

		STLBufferIterator<cVector2f> GetVec2View(VertexAttr attr) {
			return { GetArray(attr), static_cast<int>(GetArrayStride(attr) * sizeof(float)) };
		}

		STLBufferIterator<cVector3f> GetVec3View(VertexAttr attr) {
			return { GetArray(attr), static_cast<int>(GetArrayStride(attr) * sizeof(float)) };
		}

		STLBufferIterator<cColor> GetColorView(VertexAttr attr) {
			return { GetArray(attr), static_cast<int>(GetArrayStride(attr) * sizeof(float)) };
		}

		STLBufferIterator<int> GetIndexView() {
			return { GetIndices(), sizeof(int) };
		}

		//For debugging purposes, quite slow to use.
		virtual cVector3f GetVector3(VertexAttr attr, unsigned index)=0;
		virtual cColor GetColor(VertexAttr attr, unsigned index)=0;
		virtual unsigned int GetIndex(unsigned index)=0;

		/**
		 * Set the number of of elements to draw.
		 * \param count If < 0, draw all indices.
		 */
		void SetIndexCount(int count) { _indexCount = count <= GetMaxIndexCount() ? count : GetMaxIndexCount(); }

		VertexAttributes HasAllAttributes(VertexAttributeMask mask) { return (mVertexFlags & mask) == mask;}
		bool HasAttribute(VertexAttr attr) { return (mVertexFlags & attr) != 0; }

	protected:
		VertexAttributes mVertexFlags;
		VertexBufferPrimitiveType mDrawType;
		VertexBufferUsageType mUsageType;

		int _indexCount;
	};
};

#endif // HPL_VERTEXBUFFER_H
