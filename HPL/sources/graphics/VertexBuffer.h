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

	using VertexAttributes = tFlag;

	enum VertexAttributeMask {
		VertexMask_Position = 0x00000001,
		VertexMask_Normal	= 0x00000002,
		VertexMask_Color0	= 0x00000004,
		VertexMask_UV0      = 0x00000008,
		VertexMask_Tangent  = 0x00000010,
	};

	const int klNumOfVertexFlags = 5;

	const VertexAttributeMask kvVertexFlags[] = {
		VertexMask_Position,
		VertexMask_Normal,
		VertexMask_Color0,
		VertexMask_UV0,
		VertexMask_Tangent,
	};

	const int kvVertexElements[] = {
		4, // Position
		3, // Normal
		4, // Color0
		3, // UV0
		3, // Tangent
	};


	using VertexCompileOptions = tFlag;

	enum VertexCompileOption {
		CreateTangents = 1
	};


	class iVertexBuffer
	{
	public:
		iVertexBuffer(VertexAttributes aFlags,
			VertexBufferPrimitiveType aDrawType,
			VertexBufferUsageType aUsageType
		) :
			mVertexFlags(aFlags),
			mDrawType(aDrawType), mUsageType(aUsageType), mlElementNum(-1),
			mbTangents(false)
		{}

		virtual ~iVertexBuffer() = default;

		VertexAttributes GetFlags(){ return mVertexFlags;}

		virtual void AddVertex(VertexAttributes aType,const cVector3f& avVtx)=0;
		virtual void AddColor(VertexAttributes aType,const cColor& aColor)=0;
		virtual void AddIndex(unsigned int alIndex)=0;

		virtual bool Compile(VertexCompileOptions options)=0;
		virtual void UpdateData(VertexAttributes aTypes, bool abIndices)=0;

		/**
		 * Transform the entire buffer with transform.
		*/
		virtual void Transform(const cMatrixf &mtxTransform)=0;

		virtual void Draw() = 0;
		virtual void DrawWireframe() = 0;
		virtual void DrawIndices(unsigned int *apIndices, int alCount) = 0;

		virtual void Bind()=0;
		virtual void UnBind()=0;

		virtual iVertexBuffer* CreateCopy(VertexBufferUsageType aUsageType)=0;

		virtual cBoundingVolume CreateBoundingVolume()=0;

		virtual float* GetArray(VertexAttributes aType)=0;
		virtual unsigned int* GetIndices()=0;

		virtual int GetVertexNum()=0;
		virtual int GetIndexNum()=0;

		/**
		 * Resizes an array to a custom size, the size is number of elements and NOT number of vertices.
		 */
		virtual void ResizeArray(VertexAttributes aType, int alSize)=0;
		virtual void ResizeIndices(int alSize)=0;

		//For debugging purposes, quite slow to use.
		virtual cVector3f GetVector3(VertexAttributes aType, unsigned alIdx)=0;
		virtual cVector3f GetVector4(VertexAttributes aType, unsigned alIdx)=0;
		virtual cColor GetColor(VertexAttributes aType, unsigned alIdx)=0;
		virtual unsigned int GetIndex(VertexAttributes aType, unsigned alIdx)=0;

		/**
		 * Set the number of of elements to draw.
		 * \param alNum If < 0, draw all indices.
		 */
		void SetElementNum(int alNum){ mlElementNum = alNum;}
		int GetElementNum(){ return mlElementNum;}

		VertexAttributes GetVertexFlags(){ return mVertexFlags;}

		bool HasTangents(){ return mbTangents;}
		void SetTangents(bool abX){ mbTangents = abX;}

	protected:
		VertexAttributes mVertexFlags;
		VertexBufferPrimitiveType mDrawType;
		VertexBufferUsageType mUsageType;

		int mlElementNum;
		bool mbTangents;
	};

};
#endif // HPL_VERTEXBUFFER_H
