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
#ifndef HPL_LOWLEVELGRAPHICS_SDL_H
#define HPL_LOWLEVELGRAPHICS_SDL_H

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <SDL2/SDL.h>
// Unix's X11 Defines DestoryAll which collides with methods
#undef DestroyAll

#include "graphics/LowLevelGraphics.h"
#include "math/MathTypes.h"



namespace hpl {

	//-------------------------------------------------

	GLenum ColorFormatToGL(eColorDataFormat aFormat);

	GLenum TextureTargetToGL(eTextureTarget aTarget);

	//-------------------------------------------------

	class cLowLevelGraphicsSDL : public iLowLevelGraphics
	{
	public:
		cLowLevelGraphicsSDL();
		~cLowLevelGraphicsSDL();

		bool Init(	int alWidth, int alHeight, int alBpp, int abFullscreen, int alMultisampling,
					const tString& asWindowCaption);

		int GetCaps(eGraphicCaps aType);

		void ShowCursor(bool abX);
		void SetInputGrab(bool abX);

		void SetVsyncActive(bool abX);

		void SetGammaCorrection(float afX);
		float GetGammaCorrection();

		void SetMultisamplingActive(bool abX);
		int GetMultisampling(){ return mlMultisampling;}

		void SetClipPlane(int alIdx, const cPlanef& aPlane);
		cPlanef GetClipPlane(int alIdx, const cPlanef& aPlane);
		void SetClipPlaneActive(int alIdx, bool abX);

		cVector2f GetScreenSize();
		cVector2f GetVirtualSize();
		void SetVirtualSize(cVector2f avSize);

		iBitmap2D* CreateBitmap2D(const cVector2l &avSize, unsigned int alBpp);
		iFontData* CreateFontData(const tString &asName);

		iTexture* CreateTexture(bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget);
		iTexture* CreateTexture(const tString &asName,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget);
		iTexture* CreateTexture(iBitmap2D* apBmp,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget);
		iTexture* CreateTexture(const cVector2l& avSize,int alBpp,cColor aFillCol,
								bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget);

		iGpuProgram* CreateGpuProgram(const tString& asName, eGpuProgramType aType);

		void SaveScreenToBMP(const tString& asFile);

		/////////// MATRIX METHODS /////////////////////////

		void PushMatrix(eMatrix aMtxType);
		void PopMatrix(eMatrix aMtxType);
		void SetIdentityMatrix(eMatrix aMtxType);

		void SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA);

		void TranslateMatrix(eMatrix aMtxType, const cVector3f &avPos);
		void RotateMatrix(eMatrix aMtxType, const cVector3f &avRot);
		void ScaleMatrix(eMatrix aMtxType, const cVector3f &avScale);

		void SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax);

		/////////// DRAWING METHODS /////////////////////////

		// OCCLUSION
		iOcclusionQuery* CreateOcclusionQuery();
		void DestroyOcclusionQuery(iOcclusionQuery *apQuery);

		// CLEARING THE FRAMEBUFFER
		void ClearScreen();

		void SetClearColor(const cColor& aCol);
		void SetClearDepth(float afDepth);
		void SetClearStencil(int alVal);

		void SetClearColorActive(bool abX);
		void SetClearDepthActive(bool abX);
		void SetClearStencilActive(bool abX);

		void SetColorWriteActive(bool abR,bool abG,bool abB,bool abA);
		void SetDepthWriteActive(bool abX);

		void SetCullActive(bool abX);
		void SetCullMode(eCullMode aMode);

		//DEPTH
		void SetDepthTestActive(bool abX);
		void SetDepthTestFunc(eDepthTestFunc aFunc);

		//ALPHA
		void SetAlphaTestActive(bool abX);
		void SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef);

		//STENCIL
		void SetStencilActive(bool abX);
		void SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
				eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp);
		void SetStencilTwoSide(eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
				int alRef, unsigned int aMask,
				eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
				eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp);

		//SCISSOR
		void SetScissorActive(bool abX);
		void SetScissorRect(const cRect2l &aRect);

		//BLENDING
		void SetBlendActive(bool abX);
		void SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor);
		void SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
			eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha);


		// TEXTURE
		void SetTexture(unsigned int alUnit,iTexture* apTex);
		void SetActiveTextureUnit(unsigned int alUnit);
		void SetTextureEnv(eTextureParam aParam, int alVal);
		void SetTextureConstantColor(const cColor &aColor);

		// POLYGONS
		iVertexBuffer* CreateVertexBuffer(tVertexFlag aFlags, eVertexBufferDrawType aDrawType,
										eVertexBufferUsageType aUsageType,
										int alReserveVtxSize=0,int alReserveIdxSize=0);

		void DrawQuad(const tVertexVec &avVtx);
		void DrawQuad(const tVertexVec &avVtx, const cColor aCol);
		void DrawQuad(const tVertexVec &avVtx,const float afZ);
		void DrawQuad(const tVertexVec &avVtx,const float afZ,const cColor &aCol);
		void DrawQuadMultiTex(const tVertexVec &avVtx,const tVector3fVec &avExtraUvs);

		void AddVertexToBatch(const cVertex *apVtx, const cVector3f* avTransform);
		void AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f* avTransform,
										const cColor* apCol, float mfW, float mfH);

		void AddIndexToBatch(int alIndex);

		void DrawBatch(tVtxBatchFlag aTypeFlags, eBatchDrawMode aDrawMode);
		void ClearBatch();

		//PRIMITIVES
		void DrawLine(const cVector3f& avBegin, const cVector3f& avEnd, cColor aCol);
		void DrawBoxMaxMin(const cVector3f& avMax, const cVector3f& avMin, cColor aCol);
		void DrawSphere(const cVector3f& avPos, float afRadius, cColor aCol);

		void DrawLine2D(const cVector2f& avBegin, const cVector2f& avEnd, float afZ, cColor aCol);
		void DrawLineRect2D(const cRect2f& aRect, float afZ, cColor aCol);
		void DrawLineCircle2D(const cVector2f& avCenter, float afRadius, float afZ, cColor aCol);

		void DrawFilledRect2D(const cRect2f& aRect, float afZ, cColor aCol);

		//FRAMEBUFFER
		void CopyContextToTexure(iTexture* apTex, const cVector2l &avPos,
			const cVector2l &avSize, const cVector2l &avTexOffset=0);
		void SetRenderTarget(iTexture* pTex);
		bool RenderTargetHasZBuffer();
		void FlushRenderTarget();

		void FlushRendering();
		void SwapBuffers();

		///// SDL Specific ////////////////////////////

		iBitmap2D* CreateBitmap2DFromSurface(SDL_Surface* apSurface,const tString& asType);

		CGcontext GetGC_Context(){ return mCG_Context;}

		void SetupGL();

		GLenum GetGLTextureTargetEnum(eTextureTarget aType);

	private:
		cVector2l mvScreenSize;
		cVector2f mvVirtualSize;
		int mlMultisampling;
		int mlBpp;

		//Gamma
		Uint16 mvStartGammaArray[3][256];
		float mfGammaCorrection;

		//Clipping
		cPlanef mvClipPlanes[kMaxClipPlanes];

		//SDL Variables
		SDL_Window *mpWindow;
		SDL_GLContext mpGLContext;

		//Vertex Array variables
		//The vertex arrays used:
		float* mpVertexArray;
		unsigned int mlVertexCount;
		unsigned int* mpIndexArray;
		unsigned int mlIndexCount;

		unsigned int mlBatchStride;
		unsigned int mlBatchArraySize;

		//Clearing
		bool mbClearColor;
		bool mbClearDepth;
		bool mbClearStencil;

		//Rendertarget variables
		iTexture* mpRenderTarget;

		//Texture
		iTexture* mpCurrentTexture[MAX_TEXTUREUNITS];

		//CG Compiler Variables
		CGcontext mCG_Context;

		//Multisample
		void CheckMultisampleCaps();

		//CG Helper
		void InitCG();
		void ExitCG();


		//Batch helper
		void SetUpBatchArrays();

		//Depth helper
		GLenum GetGLDepthTestFuncEnum(eDepthTestFunc aType);

		//Alpha Helper
		GLenum GetGLAlphaTestFuncEnum(eAlphaTestFunc aType);

		//Stencil helper
		GLenum GetGLStencilFuncEnum(eStencilFunc aType);
		GLenum GetGLStencilOpEnum(eStencilOp aType);

		//Matrix Helper
		void SetMatrixMode(eMatrix mType);

		//Texture helper
		GLenum GetGLTextureParamEnum(eTextureParam aType);
		GLenum GetGLTextureOpEnum(eTextureOp aType);
		GLenum GetGLTextureFuncEnum(eTextureFunc aType);
		GLenum GetGLTextureSourceEnum(eTextureSource aType);

		//Blend helper
		GLenum GetGLBlendEnum(eBlendFunc aType);

		//Vtx helper
		void SetVtxBatchStates(tVtxBatchFlag aFlags);
	};
};
#endif // HPL_LOWLEVELGRAPHICS_SDL_H
