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
#ifndef HPL_RENDERER3D_H
#define HPL_RENDERER3D_H

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "graphics/Material.h"
#include "math/Frustum.h"

namespace hpl {


#define eRendererDebugFlag_DrawTangents			(0x00000001)
#define eRendererDebugFlag_DrawNormals			(0x00000002)
#define eRendererDebugFlag_DisableLighting		(0x00000004)
#define eRendererDebugFlag_LogRendering			(0x00000008)
#define eRendererDebugFlag_DrawBoundingBox		(0x00000010)
#define eRendererDebugFlag_DrawBoundingSphere	(0x00000020)
#define eRendererDebugFlag_DrawLightBoundingBox	(0x00000040)
#define eRendererDebugFlag_RenderLines			(0x00000080)
#define eRendererDebugFlag_DrawPhysicsBox       (0x00000100)

	typedef tFlag tRendererDebugFlag;

	class iLowLevelGraphics;
	class iTexture;
	class cCamera;
	class cWorld3D;
	class iVertexBuffer;
	class cRenderList;
	class iRenderable;
	class iLight3D;
	class cSector;

	//---------------------------------------------

	enum eRendererShowShadows
	{
		eRendererShowShadows_All,
		eRendererShowShadows_Static,
		eRendererShowShadows_None,
		eRendererShowShadows_LastEnum
	};

	//---------------------------------------------

	class cRenderSettings
	{
	public:
		cRenderSettings();

		void Clear();
		void Reset(iLowLevelGraphics *apLowLevel);

		//Setings that doesn't change:
		iLowLevelGraphics* mpLowLevel;

		//Setting that changes
		cCamera *mpCamera;
		cFrustum* mpFrustum;

		cSector *mpSector;

		bool mbFogActive;
		bool mbFogCulling;
		float mfFogStart;
		float mfFogEnd;
		cColor mFogColor;

		eRendererShowShadows mShowShadows;
		bool mbLog;
		tRendererDebugFlag mDebugFlags;

		//State settings
		bool mbDepthTest;

		eMaterialAlphaMode mAlphaMode;
		eMaterialBlendMode mBlendMode;
		eMaterialChannelMode mChannelMode;

		iGpuProgram* mpProgram;
		iMaterialProgramSetup* mpProgramSetup;

		cColor mAmbientColor;

		iTexture* mpTexture[MAX_TEXTUREUNITS];

		iVertexBuffer* mpVtxBuffer;
	};

	//---------------------------------------------

	class cRenderer3D
	{
	public:
		cRenderer3D(iLowLevelGraphics *apLowLevelGraphics, cTextureManager* textureManager, cGpuProgramManager* programManager);
		~cRenderer3D();

		void UpdateRenderList(cWorld3D* apWorld, cCamera *apCamera, float afFrameTime);

		void RenderWorld(cWorld3D* apWorld, cCamera *apCamera, float afFrameTime);

		void SetSkyBox(iTexture *apTexture, bool abAutoDestroy);
		void SetSkyBoxActive(bool abX);
		void SetSkyBoxColor(const cColor& aColor);

		iTexture* GetSkyBox(){return mpSkyBoxTexture;}
		bool GetSkyBoxActive(){ return mbSkyBoxActive;}
		cColor GetSkyBoxColor(){ return mSkyBoxColor;}

		void SetAmbientColor(const cColor& aColor){ mRenderSettings.mAmbientColor = aColor;}
		cColor GetAmbientColor(){ return mRenderSettings.mAmbientColor;}

		//Fog properties
		void SetFogActive(bool abX);
		void SetFogStart(float afX);
		void SetFogEnd(float afX);
		void SetFogColor(const cColor& aCol){ mRenderSettings.mFogColor = aCol;}
		void SetFogCulling(bool abX){ mRenderSettings.mbFogCulling = abX;}


		bool GetFogActive(){ return mRenderSettings.mbFogActive;}
		float GetFogStart(){ return mRenderSettings.mfFogStart;}
		float GetFogEnd(){ return mRenderSettings.mfFogEnd;}
		cColor GetFogColor(){ return mRenderSettings.mFogColor;}
		bool GetFogCulling(){ return mRenderSettings.mbFogCulling;}

		cBoundingVolume* GetFogBV(){return &mFogBV;}

		//Debug setup
		void SetDebugFlags(tRendererDebugFlag aFlags){ mDebugFlags = aFlags;}
		tRendererDebugFlag GetDebugFlags(){ return mDebugFlags;}

		cRenderList* GetRenderList(){ return mpRenderList;}
		cRenderSettings* GetRenderSettings(){return &mRenderSettings;}

		eRendererShowShadows GetShowShadows();
		void SetShowShadows(eRendererShowShadows aState);

		void FetchOcclusionQueries();

	private:
		void InitSkyBox();

		//Render steps
		void RenderSkyBox(cCamera *apCamera);
		void RenderZ(cCamera *apCamera);
		void RenderOcclusionQueries(cCamera *apCamera);
		void RenderLight(cCamera *apCamera);
		void RenderDiffuse(cCamera *apCamera);
		void RenderTrans(cCamera *apCamera);

		void RenderDebug(cCamera *apCamera);
		void RenderDebugObject(cCamera *apCamera,iRenderable* &apObject);
		void RenderPhysicsDebug(cWorld3D *apWorld, cCamera *apCamera);

		iLowLevelGraphics *_llGfx;
		cTextureManager* _textureManager;
		cGpuProgramManager* _programManager;

		bool mbLog;

		float mfRenderTime;

		iGpuProgram *mpDiffuseProgram;

		cRenderSettings mRenderSettings;

		cBoundingVolume mFogBV;

		cRenderList *mpRenderList;

		iVertexBuffer* mpSkyBox;
		iTexture* mpSkyBoxTexture;
		bool mbAutoDestroySkybox;
		bool mbSkyBoxActive;
		cColor mSkyBoxColor;

		tRendererDebugFlag mDebugFlags;
	};

};
#endif // HPL_RENDERER3D_H
