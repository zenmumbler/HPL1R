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
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVertexBufferVBO::cVertexBufferVBO(VertexAttributes aFlags,
		VertexBufferPrimitiveType aDrawType,VertexBufferUsageType aUsageType,
		int alReserveVtxSize,int alReserveIdxSize
	) : iVertexBuffer(aFlags, aDrawType, aUsageType)
	{
		if (alReserveVtxSize > 0)
		{
			for (int i=0; i < klNumOfVertexFlags; i++)
			{
				if (aFlags & kvVertexFlags[i])
				{
					mvVertexArray[i].reserve(alReserveVtxSize * kvVertexElements[i]);
				}

				mvArrayHandle[i] = 0;
			}
		}

		if (alReserveIdxSize>0)
			mvIndexArray.reserve(alReserveIdxSize);

		mlElementHandle = 0;
		mbTangents = false;
		mbCompiled = false;
	}

	cVertexBufferVBO::~cVertexBufferVBO()
	{
		for(int i=0;i< klNumOfVertexFlags; i++)
		{
			mvVertexArray[i].clear();
			if(mVertexFlags & kvVertexFlags[i])
			{
				glDeleteBuffers(1,(GLuint *)&mvArrayHandle[i]);
			}
		}

		mvIndexArray.clear();

		glDeleteBuffers(1,(GLuint *)&mlElementHandle);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::AddVertex(VertexAttributes aType,const cVector3f& avVtx)
	{
		int idx = cMath::Log2ToInt((int)aType);

		mvVertexArray[idx].push_back(avVtx.x);
		mvVertexArray[idx].push_back(avVtx.y);
		mvVertexArray[idx].push_back(avVtx.z);
		if(kvVertexElements[idx]==4)
			mvVertexArray[idx].push_back(1);
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::AddColor(VertexAttributes aType,const cColor& aColor)
	{
		int idx = cMath::Log2ToInt((int)aType);

		mvVertexArray[idx].push_back(aColor.r);
		mvVertexArray[idx].push_back(aColor.g);
		mvVertexArray[idx].push_back(aColor.b);
		mvVertexArray[idx].push_back(aColor.a);
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::AddIndex(unsigned int alIndex)
	{
		mvIndexArray.push_back(alIndex);
	}

	//-----------------------------------------------------------------------

	cBoundingVolume cVertexBufferVBO::CreateBoundingVolume()
	{
		cBoundingVolume bv;

		int lNum = cMath::Log2ToInt((int)VertexAttr_Position);

		bv.AddArrayPoints(&(mvVertexArray[lNum][0]), GetVertexNum());
		bv.CreateFromPoints(kvVertexElements[cMath::Log2ToInt(VertexAttr_Position)]);

		return bv;
	}

	//-----------------------------------------------------------------------

	bool cVertexBufferVBO::Compile(VertexCompileOptions options)
	{
		if(mbCompiled) return false;
		mbCompiled = true;

		//Create tangents
		if (options & VertexCompileOption::CreateTangents)
		{
			mbTangents = true;

			mVertexFlags |= VertexAttr_Tangent;

			int idx = cMath::Log2ToInt((int)VertexAttr_Tangent);

			int lSize = GetVertexNum()*4;
			mvVertexArray[idx].resize(lSize);

			cMath::CreateTriTangentVectors(
				&(mvVertexArray[cMath::Log2ToInt((int)VertexAttr_Tangent)][0]),
				&mvIndexArray[0], GetIndexNum(),

				&(mvVertexArray[cMath::Log2ToInt((int)VertexAttr_Position)][0]),
				kvVertexElements[cMath::Log2ToInt((int)VertexAttr_Position)],

				&(mvVertexArray[cMath::Log2ToInt((int)VertexAttr_UV0)][0]),
				&(mvVertexArray[cMath::Log2ToInt((int)VertexAttr_Normal)][0]),
				GetVertexNum()
			);
		}

		GLenum usageType = GL_STATIC_DRAW;
		if(mUsageType== VertexBufferUsageType::Dynamic) usageType = GL_DYNAMIC_DRAW;
		else if(mUsageType== VertexBufferUsageType::Stream) usageType = GL_STREAM_DRAW;

		//Create the VBO vertex arrays
		for(int i=0;i< klNumOfVertexFlags; i++)
		{
			if(mVertexFlags & kvVertexFlags[i])
			{
				glGenBuffers(1,(GLuint *)&mvArrayHandle[i]);
				glBindBuffer(GL_ARRAY_BUFFER, mvArrayHandle[i]);
				glBufferData(GL_ARRAY_BUFFER, mvVertexArray[i].size() * sizeof(float), &(mvVertexArray[i][0]), usageType);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//Log("%d-Handle: %d, size: %d \n",i,mvArrayHandle[i], mvVertexArray);
			}
		}

		//Create the VBO index array
		glGenBuffers(1,(GLuint *)&mlElementHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mlElementHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetIndexNum()*sizeof(unsigned int), &mvIndexArray[0], usageType);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

		//Log("VBO compile done!\n");

		return true;
	}


	//-----------------------------------------------------------------------

	void cVertexBufferVBO::UpdateData(VertexAttributes aTypes, bool abIndices)
	{
		GLenum usageType = GL_STATIC_DRAW;
		if (mUsageType== VertexBufferUsageType::Dynamic) usageType = GL_DYNAMIC_DRAW;
		else if (mUsageType== VertexBufferUsageType::Stream) usageType = GL_STREAM_DRAW;

		//Create the VBO vertex arrays
		for (int i=0; i < klNumOfVertexFlags; i++)
		{
			if ((mVertexFlags & kvVertexFlags[i]) && (aTypes & kvVertexFlags[i]))
			{
				glBindBuffer(GL_ARRAY_BUFFER, mvArrayHandle[i]);
				glBufferData(GL_ARRAY_BUFFER, mvVertexArray[i].size() * sizeof(float), &(mvVertexArray[i][0]), usageType);
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Create the VBO index array
		if (abIndices)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mlElementHandle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetIndexNum()*sizeof(unsigned int), &mvIndexArray[0], usageType);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		}
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::Transform(const cMatrixf &a_mtxTransform)
	{
		float *pPosArray = GetArray(VertexAttr_Position);
		float *pNormalArray = GetArray(VertexAttr_Normal);
		float *pTangentArray = NULL;
		if(mbTangents)pTangentArray = GetArray(VertexAttr_Tangent);

		int lVtxNum = GetVertexNum();

		cMatrixf mtxRot = a_mtxTransform.GetRotation();

		int lVtxStride = kvVertexElements[cMath::Log2ToInt(VertexAttr_Position)];

		int lOffset = GetVertexNum()*4;

		for(int i=0; i<lVtxNum; i++)
		{
			float* pPos = &pPosArray[i*lVtxStride];
			float* pNorm = &pNormalArray[i*3];
			float* pTan = NULL;
			if(mbTangents)pTan = &pTangentArray[i*4];

			cVector3f vPos = cMath::MatrixMul(a_mtxTransform, cVector3f(pPos[0],pPos[1],pPos[2]));
			pPos[0] = vPos.x; pPos[1] = vPos.y; pPos[2] = vPos.z;

			cVector3f vNorm = cMath::MatrixMul(mtxRot, cVector3f(pNorm[0],pNorm[1],pNorm[2]));
			vNorm.Normalise();
			pNorm[0] = vNorm.x; pNorm[1] = vNorm.y; pNorm[2] = vNorm.z;

			if(mbTangents){
				cVector3f vTan = cMath::MatrixMul(mtxRot, cVector3f(pTan[0],pTan[1],pTan[2]));
				vTan.Normalise();
				pTan[0] = vTan.x; pTan[1] = vTan.y; pTan[2] = vTan.z;
			}
		}

		if(mbCompiled)
		{
			if(mbTangents)
				UpdateData(VertexAttr_Position | VertexAttr_Normal | VertexAttr_Tangent,false);
			else
				UpdateData(VertexAttr_Position | VertexAttr_Normal,false);
		}
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::Draw()
	{
		///////////////////////////////
		//Get the draw type
		GLenum mode = GL_TRIANGLES;
		if (mDrawType == VertexBufferPrimitiveType::Quads)		mode = GL_QUADS;
		else if(mDrawType == VertexBufferPrimitiveType::LineStrips)	mode = GL_LINE_STRIP;

		//////////////////////////////////
		//Bind and draw the buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mlElementHandle);

		int lSize = mlElementNum;
		if(mlElementNum<0) lSize = GetIndexNum();

		glDrawElements(mode,lSize,GL_UNSIGNED_INT, (char*) NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	//-----------------------------------------------------------------------

	// Rehatched: this method is introduced to support the hacky wireframe debug view
	// To be obsoleted
	void cVertexBufferVBO::DrawWireframe() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mlElementHandle);

		int lSize = mlElementNum;
		if (mlElementNum < 0) lSize = GetIndexNum();

		glDrawElements(GL_LINE_STRIP, lSize, GL_UNSIGNED_INT, (char*) NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::DrawIndices(unsigned int *apIndices, int alCount)
	{
		///////////////////////////////
		//Get the draw type
		GLenum mode = GL_TRIANGLES;
		if (mDrawType == VertexBufferPrimitiveType::Quads)		mode = GL_QUADS;
		else if (mDrawType == VertexBufferPrimitiveType::LineStrips)	mode = GL_LINE_STRIP;

		//////////////////////////////////
		//Bind and draw the buffer
		glDrawElements(mode, alCount, GL_UNSIGNED_INT, apIndices);
	}


	//-----------------------------------------------------------------------

	void cVertexBufferVBO::Bind()
	{
		SetVertexStates(mVertexFlags);
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::UnBind()
	{
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}

	//-----------------------------------------------------------------------

	float* cVertexBufferVBO::GetArray(VertexAttributes aType)
	{
		int idx = cMath::Log2ToInt((int)aType);

		return &mvVertexArray[idx][0];
	}

	//-----------------------------------------------------------------------

	unsigned int* cVertexBufferVBO::GetIndices()
	{
		return &mvIndexArray[0];
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::ResizeArray(VertexAttributes aType, int alSize)
	{
		int idx = cMath::Log2ToInt((int)aType);

		mvVertexArray[idx].resize(alSize);
	}

	//-----------------------------------------------------------------------

	void cVertexBufferVBO::ResizeIndices(int alSize)
	{
		mvIndexArray.resize(alSize);
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cVertexBufferVBO::CreateCopy(VertexBufferUsageType aUsageType)
	{
		cVertexBufferVBO *pVtxBuff = new cVertexBufferVBO(mVertexFlags, mDrawType, aUsageType, GetVertexNum(), GetIndexNum());

		//Copy the vertices to the new buffer.
		for(int i=0; i < klNumOfVertexFlags; i++)
		{
			if(kvVertexFlags[i] & mVertexFlags)
			{
				int lElements = kvVertexElements[i];
				if(mbTangents && kvVertexFlags[i] == VertexAttr_Tangent)
					lElements=4;

				pVtxBuff->ResizeArray(kvVertexFlags[i], (int)mvVertexArray[i].size());

				memcpy(pVtxBuff->GetArray(kvVertexFlags[i]),
						&mvVertexArray[i][0], mvVertexArray[i].size() * sizeof(float));
			}
		}

		//Copy indices to the new buffer
		pVtxBuff->ResizeIndices(GetIndexNum());
		memcpy(pVtxBuff->GetIndices(), GetIndices(), GetIndexNum() * sizeof(unsigned int) );

		pVtxBuff->mbTangents = mbTangents;

		pVtxBuff->Compile(0);

		return pVtxBuff;
	}

	//-----------------------------------------------------------------------

	int cVertexBufferVBO::GetVertexNum()
	{
		int idx = cMath::Log2ToInt((int)VertexAttr_Position);
		int lSize = (int)mvVertexArray[idx].size()/kvVertexElements[idx];

		return lSize;
	}
	int cVertexBufferVBO::GetIndexNum()
	{
		return (int)mvIndexArray.size();
	}

	cVector3f cVertexBufferVBO::GetVector3(VertexAttributes aType, unsigned alIdx)
	{
		if(!(aType & mVertexFlags)) return cVector3f(0,0,0);

		int idx = cMath::Log2ToInt((int)aType);
		int pos = alIdx * kvVertexElements[idx];

		return cVector3f(mvVertexArray[idx][pos+0],mvVertexArray[idx][pos+1],
			mvVertexArray[idx][pos+2]);
	}
	cVector3f cVertexBufferVBO::GetVector4(VertexAttributes aType, unsigned alIdx)
	{
		if(!(aType & mVertexFlags)) return cVector3f(0,0,0);

		int idx = cMath::Log2ToInt((int)aType);
		int pos = alIdx * 4;//kvVertexElements[idx];

		return cVector3f(mvVertexArray[idx][pos+0],mvVertexArray[idx][pos+1],
			mvVertexArray[idx][pos+2]);
	}
	cColor cVertexBufferVBO::GetColor(VertexAttributes aType, unsigned alIdx)
	{
		if(!(aType & mVertexFlags)) return cColor();

		int idx = cMath::Log2ToInt((int)aType);
		int pos = alIdx * kvVertexElements[idx];

		return cColor(mvVertexArray[idx][pos+0],mvVertexArray[idx][pos+1],
			mvVertexArray[idx][pos+2],mvVertexArray[idx][pos+3]);
	}
	unsigned int cVertexBufferVBO::GetIndex(VertexAttributes aType, unsigned alIdx)
	{
		return mvIndexArray[alIdx];
	}


	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	int cVertexBufferVBO::GetElementNum(VertexAttributes aFlag)
	{
		int idx = cMath::Log2ToInt((int)aFlag);

		return kvVertexElements[idx];
	}
	//-----------------------------------------------------------------------

	void cVertexBufferVBO::SetVertexStates(VertexAttributes attrs)
	{
		// iterate over each attribute and set vertex state accordingly
		for (int index = 0; index < klNumOfVertexFlags; ++index) {
			const tFlag mask = 1 << index;
			if (attrs & mask) {
				glEnableVertexAttribArray(index);
				glBindBuffer(GL_ARRAY_BUFFER, mvArrayHandle[index]);
				glVertexAttribPointer(index, kvVertexElements[index], GL_FLOAT, false, 0, nullptr);
			}
			else {
				glDisableVertexAttribArray(index);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//-----------------------------------------------------------------------

}
