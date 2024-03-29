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
#ifndef HPL_GRAPHICSTYPES_H
#define HPL_GRAPHICSTYPES_H

#include <list>
#include <vector>
#include "graphics/Color.h"
#include "math/MathTypes.h"
#include "system/NumberTypes.h"

namespace hpl {

	const int MAX_TEXTUREUNITS = 8;
	const int MAX_NUM_OF_LIGHTS = 30;

	enum ePrimitiveType
	{
		ePrimitiveType_Tri,
		ePrimitiveType_Quad
	};

	typedef tFlag tAnimTransformFlag;

	#define eAnimTransformFlag_Translate	(0x00000001)
	#define eAnimTransformFlag_Scale		(0x00000002)
	#define eAnimTransformFlag_Rotate		(0x00000004)

	#define klNumOfAnimTransformFlags (3)

	const tAnimTransformFlag kvAnimTransformFlags[] = {eAnimTransformFlag_Translate,
					eAnimTransformFlag_Scale, eAnimTransformFlag_Rotate};

	//-----------------------------------------

	enum eFontAlign
	{
		eFontAlign_Left,
		eFontAlign_Right,
		eFontAlign_Center
	};

	//---------------------------------------

	struct QuadUVs {
		cVector2f uv0 = {0,0};
		cVector2f uv1 = {1,0};
		cVector2f uv2 = {1,1};
		cVector2f uv3 = {0,1};
	};

	//---------------------------------------

	class cKeyFrame
	{
	public:
		cVector3f trans;
		cVector3f scale;
		cQuaternion rotation;
		float time;
	};

	typedef std::vector<cKeyFrame*> tKeyFramePtrVec;
	typedef tKeyFramePtrVec::iterator tKeyFramePtrVecIt;

	typedef std::vector<cKeyFrame> tKeyFrameVec;
	typedef tKeyFrameVec::iterator tKeyFrameVecIt;

	//---------------------------------------

	enum eAnimationEventType
	{
		eAnimationEventType_PlaySound,
		eAnimationEventType_LastEnum
	};

	//---------------------------------------

	class cVertexBonePair
	{
	public:
		cVertexBonePair(unsigned int aVtx, unsigned int aBone, float aW)
		{
			vtxIdx = aVtx;
			boneIdx = aBone;
			weight = aW;
		}
		cVertexBonePair(){}


		unsigned int vtxIdx;
		unsigned int boneIdx;
		float weight;
	};


	typedef std::vector<cVertexBonePair> tVertexBonePairVec;
	typedef tVertexBonePairVec::iterator tVertexBonePairVecIt;

	//---------------------------------------

	class cVertex
	{
	public:
		cVertex():pos(0),tex(0),col(0){}
		cVertex(const cVector3f &avPos,const cVector3f &avTex,const cColor &aCol )
		{
			pos = avPos;
			tex = avTex;
			col = aCol;
		}

		cVertex(const cVector3f &avPos,const cColor &aCol )
		{
			pos = avPos;
			col = aCol;
		}

		cVector3f pos;
		cVector3f tex;
		cColor col;
		cVector3f norm;
		cVector3f tan;
	};

	typedef std::list<cVertex> tVertexList;
	typedef tVertexList::iterator tVertexListIt;

	typedef std::vector<cVertex> tVertexVec;
	typedef tVertexVec::iterator tVertexVecIt;
}

#endif // HPL_GRAPHICSTYPES_H
