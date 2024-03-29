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
#ifndef HPL_RENDER_LIST_H
#define HPL_RENDER_LIST_H

#include "system/MemoryPool.h"
#include "scene/Light3D.h"
#include "graphics/Material.h"
#include "graphics/OcclusionQuery.h"
#include "graphics/VertexBuffer.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

#include <vector>

namespace hpl {

	class iRenderable;
	class cCamera;

	class iRenderState;
	class cRenderSettings;

	//-------------------------------------------------------------

	class cRenderNode
	{
	public:
		cRenderNode() : mpState(NULL){}

		iRenderState *mpState;

		std::vector<cRenderNode*> m_setNodes;

		void DeleteChildren();
		void Render(cRenderSettings* apSettings);
	};

	//-------------------------------------------------------------

	class cMotionBlurObject_Compare
	{
	public:
		bool operator()(iRenderable* pObjectA,iRenderable* pObjectB) const;
	};

	typedef std::multiset<iRenderable*,cMotionBlurObject_Compare> tMotionBlurObjectSet;
	typedef tMotionBlurObjectSet::iterator tMotionBlurObjectSetIt;

	typedef cSTLIterator<iRenderable*, tMotionBlurObjectSet,
							tMotionBlurObjectSetIt> cMotionBlurObjectIterator;

	//-------------------------------------------------------------


	class cTransperantObject_Compare
	{
	public:
		bool operator()(iRenderable* pObjectA,iRenderable* pObjectB) const;
	};

	typedef std::multiset<iRenderable*,cTransperantObject_Compare> tTransperantObjectSet;
	typedef tTransperantObjectSet::iterator tTransperantObjectSetIt;

	typedef cSTLIterator<iRenderable*, tTransperantObjectSet, tTransperantObjectSetIt> cTransperantObjectIterator;


	//-------------------------------------------------------------

	class cOcclusionQueryObject
	{
	public:
		cOcclusionQueryObject() : mpQuery(NULL), mpVtxBuffer(NULL),_matrix{} {}

		iOcclusionQuery *mpQuery;
		iVertexBuffer *mpVtxBuffer;
		cMatrixf _matrix;
		bool mbDepthTest;
	};

	class cOcclusionQueryObject_Compare
	{
	public:
		bool operator()(const cOcclusionQueryObject* pObjectA,const cOcclusionQueryObject* pObjectB) const;
	};

	typedef std::set<cOcclusionQueryObject*,cOcclusionQueryObject_Compare> tOcclusionQueryObjectSet;
	typedef tOcclusionQueryObjectSet::iterator tOcclusionQueryObjectSetIt;

	typedef cSTLIterator<cOcclusionQueryObject*, tOcclusionQueryObjectSet,
						tOcclusionQueryObjectSetIt> cOcclusionQueryObjectIterator;

	//-------------------------------------------------------------

	typedef std::set<iRenderable*> tRenderableSet;
	typedef tRenderableSet::iterator tRenderableSetIt;

	typedef std::set<iLight3D*> tLight3DSet;
	typedef tLight3DSet::iterator tLight3DSetIt;


	typedef cSTLIterator<iLight3D*,tLight3DSet, tLight3DSetIt> cLight3DIterator;
	typedef cSTLIterator<iRenderable*,tRenderableSet, tRenderableSetIt> cRenderableIterator;

	//-------------------------------------------------------------

	class cRenderList
	{
	public:
		cRenderList();
		~cRenderList();

		void SetCamera(cCamera *apCamera){ mpCamera = apCamera;}
		cCamera *GetCamera(){ return mpCamera;}

		bool Add(iRenderable* apObject);
		void Clear();

		void AddOcclusionQuery(cOcclusionQueryObject *apObject);

		cOcclusionQueryObjectIterator GetQueryIterator();
		cMotionBlurObjectIterator GetMotionBlurIterator();
		cTransperantObjectIterator GetTransperantIterator();

		cLight3DIterator GetLightIt();
		cRenderableIterator GetObjectIt();

		int GetLightNum();
		int GetObjectNum();

		void Compile();

		int GetRenderCount(){ return mlRenderCount;}
		int GetLastRenderCount(){ return mlLastRenderCount;}

		int GetLightObjects(int alLightIdx){ return mvObjectsPerLight[alLightIdx];}

		void SetFrameTime(float afTime){ mfFrameTime = afTime;}

		cRenderNode* GetRootNode();

		static inline int GetGlobalRenderCount(){ return mlGlobalRenderCount;}

	private:
		cRenderNode* InsertNode(cRenderNode* apListNode, cRenderNode* apTempNode);

		void AddToTree(iRenderable* apObject);

		static int mlGlobalRenderCount;

		tLight3DSet m_setLights;
		int mvObjectsPerLight[MAX_NUM_OF_LIGHTS];

		tRenderableSet m_setObjects;

		tOcclusionQueryObjectSet m_setQueries;

		tMotionBlurObjectSet m_setMotionBlurObjects;
		tTransperantObjectSet m_setTransperantObjects;

		cRenderNode mRootNodeDiffuse;

		cRenderNode mTempNode;

		int mlRenderCount;
		int mlLastRenderCount;

		float mfFrameTime;

		cMemoryPool<iRenderState>* m_poolRenderState;
		cMemoryPool<cRenderNode>* m_poolRenderNode;

		cCamera *mpCamera;
	};
};
#endif // HPL_RENDER_LIST_H
