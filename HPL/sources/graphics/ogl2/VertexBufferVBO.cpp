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
#include "graphics/ogl2/VertexBufferVBO.h"
#include "math/Math.h"

#include <memory.h>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// UTILITY
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static const VertexAttributes ATTR_TO_MASK[VERTEX_ATTR_COUNT] = {
		VertexMask_Position,
		VertexMask_Normal,
		VertexMask_Color0,
		VertexMask_UV0,
		VertexMask_Tangent
	};

	constexpr int AttrElemCount(int attr) {
		if (attr == VertexAttr_UV0) return 2;
		return (attr == VertexAttr_Color0 || attr == VertexAttr_Tangent) ? 4 : 3;
	}

	constexpr GLenum GetGLUsageType(VertexBufferUsageType vbut) {
		GLenum usageType = GL_STATIC_DRAW;
		if (vbut == VertexBufferUsageType::Dynamic) usageType = GL_DYNAMIC_DRAW;
		else if (vbut == VertexBufferUsageType::Stream) usageType = GL_STREAM_DRAW;
		return usageType;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVertexBufferVBO::cVertexBufferVBO(VertexAttributes attrs,
		VertexBufferPrimitiveType aDrawType,VertexBufferUsageType aUsageType,
		int alReserveVtxSize, int alReserveIdxSize
	) : iVertexBuffer(attrs, aDrawType, aUsageType)
	{
		for (int attr=0; attr < VERTEX_ATTR_COUNT; attr++)
		{
			if (attrs & ATTR_TO_MASK[attr]) {
				mvVertexArray[attr].resize(alReserveVtxSize * AttrElemCount(attr));
			}
			mvArrayHandle[attr] = 0;
		}

		mvIndexArray.resize(alReserveIdxSize);

		mlElementHandle = 0;
		_vaoHandle = 0;
	}

	cVertexBufferVBO::~cVertexBufferVBO()
	{
		for (int i=0; i < VERTEX_ATTR_COUNT; i++)
		{
			if(mVertexFlags & ATTR_TO_MASK[i])
			{
				mvVertexArray[i].clear();
				if (mlElementHandle)
					glDeleteBuffers(1, &mvArrayHandle[i]);
			}
		}

		mvIndexArray.clear();
		if (mlElementHandle)
			glDeleteBuffers(1,(GLuint *)&mlElementHandle);
		if (_vaoHandle)
			glDeleteVertexArrays(1, &_vaoHandle);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBoundingVolume cVertexBufferVBO::CreateBoundingVolume()
	{
		cBoundingVolume bv;

		auto positions = GetArray(VertexAttr_Position);
		bv.AddArrayPoints(positions, GetVertexCount());
		bv.CreateFromPoints(AttrElemCount(VertexAttr_Position));

		return bv;
	}

	//-----------------------------------------------------------------------

	bool cVertexBufferVBO::GenerateTangents() {
		// only try to generate tangents if the data supports it
		VertexAttributes requiredAttrs = VertexMask_Position | VertexMask_Normal | VertexMask_UV0 | VertexMask_Tangent;
		if (
			(mVertexFlags & requiredAttrs) != requiredAttrs
			||
			mDrawType != VertexBufferPrimitiveType::Triangles
		) {
			return false;
		}

		cMath::CreateTriTangentVectors(
			GetArray(VertexAttr_Tangent),
			GetIndices(), GetIndexCount(),

			GetArray(VertexAttr_Position), AttrElemCount(VertexAttr_Position),

			GetArray(VertexAttr_UV0),
			GetArray(VertexAttr_Normal),
			GetVertexCount()
		);

		return true;
	}

	//-----------------------------------------------------------------------

	bool cVertexBufferVBO::Compile()
	{
		if (_vaoHandle) return false;

		GLenum usageType = GetGLUsageType(mUsageType);

		glGenVertexArrays(1, &_vaoHandle);
		glBindVertexArray(_vaoHandle);

		//Create the VBO vertex arrays
		for (int attr=0; attr < VERTEX_ATTR_COUNT; attr++)
		{
			if (mVertexFlags & ATTR_TO_MASK[attr])
			{
				glGenBuffers(1, &mvArrayHandle[attr]);
				glBindBuffer(GL_ARRAY_BUFFER, mvArrayHandle[attr]);
				glBufferData(GL_ARRAY_BUFFER, mvVertexArray[attr].size() * sizeof(float), mvVertexArray[attr].data(), usageType);

				glEnableVertexAttribArray(attr);
				glVertexAttribPointer(attr, AttrElemCount(attr), GL_FLOAT, false, 0, nullptr);
			}
			else {
				glDisableVertexAttribArray(attr);
			}
		}

		//Create the VBO index array
		glGenBuffers(1, &mlElementHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mlElementHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetIndexCount() * sizeof(unsigned int), mvIndexArray.data(), usageType);

		glBindVertexArray(0);

		return true;
	}


	//-----------------------------------------------------------------------

	void cVertexBufferVBO::UpdateData(VertexAttributes attrs, bool updateIndices)
	{
		for (int attr=0; attr < VERTEX_ATTR_COUNT; attr++)
		{
			if (mVertexFlags & attrs & ATTR_TO_MASK[attr])
			{
				glBindBuffer(GL_ARRAY_BUFFER, mvArrayHandle[attr]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, mvVertexArray[attr].size() * sizeof(float), mvVertexArray[attr].data());
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (updateIndices)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mlElementHandle);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, GetIndexCount() * sizeof(unsigned int), mvIndexArray.data());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::Transform(const cMatrixf &a_mtxTransform)
	{
		float *pPosArray = GetArray(VertexAttr_Position);
		float *pNormalArray = GetArray(VertexAttr_Normal);
		bool hasTangents = HasAttribute(VertexAttr_Tangent);
		float *pTangentArray = hasTangents ? GetArray(VertexAttr_Tangent) : nullptr;

		int lVtxNum = GetVertexCount();

		cMatrixf mtxRot = a_mtxTransform.GetRotation();

		for (int i=0; i < lVtxNum; i++)
		{
			float* pPos = pPosArray + (i * AttrElemCount(VertexAttr_Position));
			float* pNorm = pNormalArray + (i * AttrElemCount(VertexAttr_Normal));

			cVector3f vPos = cMath::MatrixMul(a_mtxTransform, cVector3f(pPos[0],pPos[1],pPos[2]));
			pPos[0] = vPos.x; pPos[1] = vPos.y; pPos[2] = vPos.z;

			cVector3f vNorm = cMath::MatrixMul(mtxRot, cVector3f(pNorm[0],pNorm[1],pNorm[2]));
			vNorm.Normalise();
			pNorm[0] = vNorm.x; pNorm[1] = vNorm.y; pNorm[2] = vNorm.z;

			if (hasTangents) {
				float* pTan = pTangentArray + (i * AttrElemCount(VertexAttr_Tangent));

				cVector3f vTan = cMath::MatrixMul(mtxRot, cVector3f(pTan[0], pTan[1], pTan[2]));
				vTan.Normalise();
				pTan[0] = vTan.x; pTan[1] = vTan.y; pTan[2] = vTan.z;
			}
		}

		if (mlElementHandle)
		{
			if(hasTangents)
				UpdateData(VertexMask_Position | VertexMask_Normal | VertexMask_Tangent, false);
			else
				UpdateData(VertexMask_Position | VertexMask_Normal, false);
		}
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::Draw()
	{
		///////////////////////////////
		//Get the draw type
		GLenum mode = GL_TRIANGLES;
		if (mDrawType == VertexBufferPrimitiveType::Quads) mode = GL_QUADS;
		else if(mDrawType == VertexBufferPrimitiveType::LineStrips)	mode = GL_LINE_STRIP;

		//////////////////////////////////
		//Bind and draw the buffer
		glDrawElements(mode, GetIndexCount(), GL_UNSIGNED_INT, NULL);
	}

	//-----------------------------------------------------------------------

	// Rehatched: this method is introduced to support the hacky wireframe debug view
	// To be obsoleted
	void cVertexBufferVBO::DrawWireframe() {
		glDrawElements(GL_LINE_STRIP, GetIndexCount(), GL_UNSIGNED_INT, NULL);
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::Bind()
	{
		glBindVertexArray(_vaoHandle);
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::UnBind()
	{
		glBindVertexArray(0);
	}

	//-----------------------------------------------------------------------

	float* cVertexBufferVBO::GetArray(VertexAttr attr)
	{
		return mvVertexArray[attr].data();
	}

	//-----------------------------------------------------------------------

	int cVertexBufferVBO::GetArrayStride(VertexAttr attr) {
		return AttrElemCount(attr);
	}

	//-----------------------------------------------------------------------

	unsigned int* cVertexBufferVBO::GetIndices()
	{
		return mvIndexArray.data();
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cVertexBufferVBO::CreateCopy(VertexBufferUsageType aUsageType)
	{
		cVertexBufferVBO *pVtxBuff = new cVertexBufferVBO(mVertexFlags, mDrawType, aUsageType, GetVertexCount(), GetIndexCount());

		// Copy the vertices to the new buffer.
		for (int attr=0; attr < VERTEX_ATTR_COUNT; attr++)
		{
			if (mVertexFlags & ATTR_TO_MASK[attr])
			{
				memcpy(
					pVtxBuff->GetArray(static_cast<VertexAttr>(attr)),
					mvVertexArray[attr].data(),
					mvVertexArray[attr].size() * sizeof(float)
				);
			}
		}

		// Copy indices to the new buffer
		memcpy(pVtxBuff->GetIndices(), GetIndices(), GetIndexCount() * sizeof(unsigned int));

		pVtxBuff->Compile();

		return pVtxBuff;
	}

	//-----------------------------------------------------------------------

	int cVertexBufferVBO::GetVertexCount()
	{
		return (int)mvVertexArray[VertexAttr_Position].size() / AttrElemCount(VertexAttr_Position);
	}

	int cVertexBufferVBO::GetMaxIndexCount() {
		return static_cast<int>(mvIndexArray.size());
	}

	int cVertexBufferVBO::GetIndexCount()
	{
		return _indexCount < 0 ? GetMaxIndexCount() : _indexCount;
	}

	//-----------------------------------------------------------------------

	cVector3f cVertexBufferVBO::GetVector3(VertexAttr attr, unsigned index)
	{
		if(!(attr & mVertexFlags)) return cVector3f(0,0,0);

		int pos = index * AttrElemCount(attr);
		auto& array = mvVertexArray[attr];

		return cVector3f(array[pos+0], array[pos+1], array[pos+2]);
	}
	cColor cVertexBufferVBO::GetColor(VertexAttr attr, unsigned index)
	{
		if(!(attr & mVertexFlags)) return cColor{};

		int pos = index * AttrElemCount(attr);
		auto& array = mvVertexArray[attr];

		return cColor(array[pos+0], array[pos+1], array[pos+2], array[pos+3]);
	}
	unsigned int cVertexBufferVBO::GetIndex(unsigned index)
	{
		return mvIndexArray[index];
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

}
