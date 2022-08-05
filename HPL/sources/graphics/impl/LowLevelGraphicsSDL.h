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

#include <SDL2/SDL.h>
#ifdef __APPLE__
#include <OpenGL/gltypes.h>
#else
#include <GL/gltypes.h>
#endif
// Unix's X11 Defines DestroyAll which collides with methods
#undef DestroyAll

#include "graphics/LowLevelGraphics.h"
#include "math/MathTypes.h"

struct ImGuiContext;

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

		bool Init(int alWidth, int alHeight, bool abFullscreen, int alMultisampling,
					const tString& asWindowCaption) override;

		int GetCaps(eGraphicCaps aType) override;

		void ShowCursor(bool abX) override;
		void SetInputGrab(bool abX);

		void SetVsyncActive(bool abX) override;

		void SetGammaCorrection(float afX) override;
		float GetGammaCorrection() override;

		void SetMultisamplingActive(bool abX) override;
		int GetMultisampling() override { return mlMultisampling;}

		void SetClipPlane(int alIdx, const cPlanef& aPlane) override;
		cPlanef GetClipPlane(int alIdx, const cPlanef& aPlane) override;
		void SetClipPlaneActive(int alIdx, bool abX) override;

		cVector2f GetScreenSize() override;
		cVector2f GetVirtualSize() override;
		void SetVirtualSize(cVector2f avSize) override;

		iTexture* CreateTexture(bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget) override;
		iTexture* CreateTexture(const tString &asName,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget) override;
		iTexture* CreateTexture(const Bitmap& source, bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget) override;
		iTexture* CreateTexture(const cVector2l& avSize,int alBpp,cColor aFillCol,
								bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget) override;

		iGpuProgram* CreateGpuProgram(const tString& asName) override;

		void SaveScreenToBMP(const tString& asFile) override;

		/////////// MATRIX METHODS /////////////////////////

		void PushMatrix(eMatrix aMtxType) override;
		void PopMatrix(eMatrix aMtxType) override;
		void SetIdentityMatrix(eMatrix aMtxType) override;

		void SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA) override;

		void TranslateMatrix(eMatrix aMtxType, const cVector3f &avPos) override;
		void RotateMatrix(eMatrix aMtxType, const cVector3f &avRot) override;
		void ScaleMatrix(eMatrix aMtxType, const cVector3f &avScale) override;

		void SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax) override;

		/////////// DRAWING METHODS /////////////////////////

		// OCCLUSION
		iOcclusionQuery* CreateOcclusionQuery() override;
		void DestroyOcclusionQuery(iOcclusionQuery *apQuery) override;

		// CLEARING THE FRAMEBUFFER
		void ClearScreen() override;

		void SetClearColor(const cColor& aCol) override;
		void SetClearDepth(float afDepth) override;
		void SetClearStencil(int alVal) override;

		void SetClearColorActive(bool abX) override;
		void SetClearDepthActive(bool abX) override;
		void SetClearStencilActive(bool abX) override;

		void SetColorWriteActive(bool abR,bool abG,bool abB,bool abA) override;
		void SetDepthWriteActive(bool abX) override;

		void SetCullActive(bool abX) override;
		void SetCullMode(eCullMode aMode) override;

		//DEPTH
		void SetDepthTestActive(bool abX) override;
		void SetDepthTestFunc(eDepthTestFunc aFunc) override;

		//ALPHA
		void SetAlphaTestActive(bool abX) override;
		void SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef) override;

		//STENCIL
		void SetStencilActive(bool abX) override;
		void SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
				eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp) override;
		void SetStencilTwoSide(eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
				int alRef, unsigned int aMask,
				eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
				eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp) override;

		//SCISSOR
		void SetScissorActive(bool abX) override;
		void SetScissorRect(const cRect2l &aRect) override;

		//BLENDING
		void SetBlendActive(bool abX) override;
		void SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor) override;
		void SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
			eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha) override;


		// TEXTURE
		void SetTexture(unsigned int alUnit,iTexture* apTex) override;
		void SetActiveTextureUnit(unsigned int alUnit) override;
		void SetTextureEnv(eTextureParam aParam, int alVal) override;
		void SetTextureConstantColor(const cColor &aColor) override;

		// POLYGONS
		iVertexBuffer* CreateVertexBuffer(tVertexFlag aFlags, eVertexBufferDrawType aDrawType,
										eVertexBufferUsageType aUsageType,
										int alReserveVtxSize=0,int alReserveIdxSize=0) override;

		void DrawQuad(const tVertexVec &avVtx) override;
		void DrawQuad(const tVertexVec &avVtx, const cColor aCol) override;
		void DrawQuadMultiTex(const tVertexVec &avVtx,const tVector3fVec &avExtraUvs) override;

		void DrawBatch(const cGfxBatch &batch, tGfxBatchAttrs attrs, eBatchDrawMode drawMode) override;

		//PRIMITIVES
		void DrawLine(const cVector3f& avBegin, const cVector3f& avEnd, cColor aCol) override;
		void DrawBoxMaxMin(const cVector3f& avMax, const cVector3f& avMin, cColor aCol) override;
		void DrawSphere(const cVector3f& avPos, float afRadius, cColor aCol) override;

		//FRAMEBUFFER
		void CopyContextToTexure(iTexture* apTex, const cVector2l &avPos,
			const cVector2l &avSize, const cVector2l &avTexOffset=0) override;

		void StartFrame() override;
		void FlushRendering() override;
		void SwapBuffers() override;
		void EndFrame() override;
		
		///// SDL Specific ////////////////////////////
		void SetupGL();

		GLenum GetGLTextureTargetEnum(eTextureTarget aType);

	private:
		cVector2l mvScreenSize;
		cVector2f mvVirtualSize;
		int mlMultisampling;

		//Gamma
		float mfGammaCorrection;

		//Clipping
		cPlanef mvClipPlanes[kMaxClipPlanes];

		//SDL Variables
		SDL_Window *mpWindow;
		SDL_GLContext mpGLContext;

		//Clearing
		bool mbClearColor;
		bool mbClearDepth;
		bool mbClearStencil;

		//Texture
		iTexture* mpCurrentTexture[MAX_TEXTUREUNITS];

		// ImGui Setup
		ImGuiContext* mpImGuiContext;

		//Multisample
		void CheckMultisampleCaps();

		//CG Helper
		void InitCG();
		void ExitCG();

		//Batch helper
		void PrepareBatchDraw(const cGfxBatch &batch, tGfxBatchAttrs attrs);

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
	};
};
#endif // HPL_LOWLEVELGRAPHICS_SDL_H
