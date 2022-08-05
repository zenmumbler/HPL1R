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
#ifndef HPL_LOWLEVELGRAPHICS_H
#define HPL_LOWLEVELGRAPHICS_H

#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Bitmap.h"
#include "graphics/Texture.h"
#include "graphics/GPUProgram.h"
#include "graphics/VertexBuffer.h"
#include "graphics/GfxBatch.h"

namespace hpl {

	enum eBlendFunc
	{
		eBlendFunc_Zero,
		eBlendFunc_One,
		eBlendFunc_SrcColor,
		eBlendFunc_OneMinusSrcColor,
		eBlendFunc_DestColor,
		eBlendFunc_OneMinusDestColor,
		eBlendFunc_SrcAlpha,
		eBlendFunc_OneMinusSrcAlpha,
		eBlendFunc_DestAlpha,
		eBlendFunc_OneMinusDestAlpha,
		eBlendFunc_SrcAlphaSaturate,
		eBlendFunc_LastEnum
	};

	enum eTextureParam
	{
		eTextureParam_ColorFunc,
		eTextureParam_AlphaFunc,
		eTextureParam_ColorSource0,
		eTextureParam_ColorSource1,
		eTextureParam_ColorSource2,
		eTextureParam_AlphaSource0,
		eTextureParam_AlphaSource1,
		eTextureParam_AlphaSource2,
		eTextureParam_ColorOp0,
		eTextureParam_ColorOp1,
		eTextureParam_ColorOp2,
		eTextureParam_LastEnum
	};

	enum eTextureOp
	{
		eTextureOp_Color,
		eTextureOp_OneMinusColor,
		eTextureOp_Alpha,
		eTextureOp_OneMinusAlpha,
		eTextureOp_LasEnum
	};

	enum eTextureSource
	{
		eTextureSource_Texture,
		eTextureSource_Constant,
		eTextureSource_Primary,
		eTextureSource_Previous,
		eTextureSource_LastEnum
	};

	enum eTextureFunc
	{
		eTextureFunc_Modulate,
		eTextureFunc_Replace,
		eTextureFunc_Add,
		eTextureFunc_Substract,
		eTextureFunc_AddSigned,
		eTextureFunc_Interpolate,
		eTextureFunc_Dot3RGB,
		eTextureFunc_Dot3RGBA,
		eTextureFunc_LastEnum
	};

	enum eStencilFunc
	{
		eStencilFunc_Never,
		eStencilFunc_Less,
		eStencilFunc_LessOrEqual,
		eStencilFunc_Greater,
		eStencilFunc_GreaterOrEqual,
		eStencilFunc_Equal,
		eStencilFunc_NotEqual,
		eStencilFunc_Always,
		eStencilFunc_LastEnum
	};

	enum eStencilFace
	{
		eStencilFace_Front,
		eStencilFace_Back,
		eStencilFace_LastEnum,
	};

	enum eDepthTestFunc
	{
		eDepthTestFunc_Never,
		eDepthTestFunc_Less,
		eDepthTestFunc_LessOrEqual,
		eDepthTestFunc_Greater,
		eDepthTestFunc_GreaterOrEqual,
		eDepthTestFunc_Equal,
		eDepthTestFunc_NotEqual,
		eDepthTestFunc_Always,
		eDepthTestFunc_LastEnum
	};

	enum eAlphaTestFunc
	{
		eAlphaTestFunc_Never,
		eAlphaTestFunc_Less,
		eAlphaTestFunc_LessOrEqual,
		eAlphaTestFunc_Greater,
		eAlphaTestFunc_GreaterOrEqual,
		eAlphaTestFunc_Equal,
		eAlphaTestFunc_NotEqual,
		eAlphaTestFunc_Always,
		eAlphaTestFunc_LastEnum
	};

	enum eStencilOp
	{
		eStencilOp_Keep,
		eStencilOp_Zero,
		eStencilOp_Replace,
		eStencilOp_Increment,
		eStencilOp_Decrement,
		eStencilOp_Invert,
		eStencilOp_IncrementWrap,
		eStencilOp_DecrementWrap,
		eStencilOp_LastEnum
	};

	enum eCullMode
	{
		eCullMode_Clockwise,
		eCullMode_CounterClockwise,
		eCullMode_LastEnum
	};

	enum eGraphicCaps
	{
		eGraphicCaps_TextureTargetRectangle,
		eGraphicCaps_VertexBufferObject,

		eGraphicCaps_MaxTextureImageUnits,
		eGraphicCaps_MaxTextureCoordUnits,

		eGraphicCaps_AnisotropicFiltering,
		eGraphicCaps_MaxAnisotropicFiltering,

		eGraphicCaps_Multisampling,

		eGraphicCaps_GL_VertexProgram,
		eGraphicCaps_GL_FragmentProgram,
		
		eGraphicCaps_GL_CoreProfile,

		eGraphicCaps_LastEnum
	};

	#define kMaxClipPlanes (6)

	class iOcclusionQuery;

	class iLowLevelGraphics
	{
	public:
		virtual ~iLowLevelGraphics() = default;
		/**
		 * Sets the video mode. Must only be called ONCE!
		 * \param alWidth
		 * \param alHeight
		 * \param abFullscreen
		 * \param alMultiSampling The amount of multisamplimg, 0 = off.
		 * \return
		 */
		virtual bool Init(int alWidth, int alHeight, bool abFullscreen, int alMultisampling,
							const tString& asWindowCaption) = 0;

		/**
		 * Get the capabilities of the graphics. Th return value depends on the capability
		 * \param aType
		 * \return
		 */
		virtual int GetCaps(eGraphicCaps aType)=0;
		/**
		 * Show the cursor or not. Default is false
		 * \param aX
		 */
		virtual void ShowCursor(bool abX)=0;

		virtual int GetMultisampling()=0;
		/**
		 * Get Size of screen
		 * \return
		 */
		virtual cVector2f GetScreenSize()=0;
		virtual cVector2f GetVirtualSize()=0;
		/**
		 * Sets the virtual screen size. Default is 0-1
		 * \param avSize
		 */
		virtual void SetVirtualSize(cVector2f avSize)=0;

		/**
		 * Set if vsync should eb active
		 * \param abX
		 */
		virtual void SetVsyncActive(bool abX)=0;

		virtual void SetMultisamplingActive(bool abX)=0;

		virtual void SetGammaCorrection(float afX)=0;
		virtual float GetGammaCorrection()=0;

		virtual void SetClipPlane(int alIdx, const cPlanef& aPlane)=0;
		virtual cPlanef GetClipPlane(int alIdx, const cPlanef& aPlane)=0;
		virtual void SetClipPlaneActive(int alIdx, bool abX)=0;

		virtual iTexture* CreateTexture(bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)=0;
		virtual iTexture* CreateTexture(const tString &asName,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)=0;
		virtual iTexture* CreateTexture(const Bitmap& source, bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)=0;
		virtual iTexture* CreateTexture(const cVector2l& avSize,int alBpp,cColor aFillCol,
									bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)=0;


		virtual iGpuProgram* CreateGpuProgram(const tString& asName)=0;

		//TODO: Kinda quick and diry, better to have a screen to Bitmap.
		// and then a save as in the Bitmap.
		virtual void SaveScreenToBMP(const tString& asFile)=0;

		/////////// MATRIX METHODS //////////////////////////

		virtual void PushMatrix(eMatrix aMtxType)=0;
		virtual void PopMatrix(eMatrix aMtxType)=0;
		virtual void SetIdentityMatrix(eMatrix aMtxType)=0;

		virtual void SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA)=0;

		virtual void TranslateMatrix(eMatrix aMtxType, const cVector3f &avPos)=0;
		virtual void RotateMatrix(eMatrix aMtxType, const cVector3f &avRot)=0;
		virtual void ScaleMatrix(eMatrix aMtxType, const cVector3f &avScale)=0;

		virtual void SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax)=0;

		/////////// DRAWING METHODS /////////////////////////

		//OCCLUSION
		virtual iOcclusionQuery* CreateOcclusionQuery()=0;
		virtual void DestroyOcclusionQuery(iOcclusionQuery *apQuery)=0;

		// CLEARING THE FRAMEBUFFER
		virtual void ClearScreen()=0;

		virtual void SetClearColor(const cColor& aCol)=0;
		virtual void SetClearDepth(float afDepth)=0;
		virtual void SetClearStencil(int alVal)=0;

		virtual void SetClearColorActive(bool abX)=0;
		virtual void SetClearDepthActive(bool abX)=0;
		virtual void SetClearStencilActive(bool abX)=0;

		virtual void SetColorWriteActive(bool abR,bool abG,bool abB,bool abA)=0;
		virtual void SetDepthWriteActive(bool abX)=0;

		virtual void SetCullActive(bool abX)=0;
		virtual void SetCullMode(eCullMode aMode)=0;

		//DEPTH
		virtual void SetDepthTestActive(bool abX)=0;
		virtual void SetDepthTestFunc(eDepthTestFunc aFunc)=0;

		//ALPHA
		virtual void SetAlphaTestActive(bool abX)=0;
		virtual void SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef)=0;

		//STENCIL
		virtual void SetStencilActive(bool abX)=0;
		virtual void SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
						eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp)=0;
		virtual void SetStencilTwoSide(eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
						int alRef, unsigned int aMask,
						eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
						eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp)=0;

		//SCISSOR
		virtual void SetScissorActive(bool abX)=0;
		virtual void SetScissorRect(const cRect2l &aRect)=0;

		//TEXTURE
		virtual void SetTexture(unsigned int alUnit,iTexture* apTex)=0;
		virtual void SetActiveTextureUnit(unsigned int alUnit)=0;
		virtual void SetTextureEnv(eTextureParam aParam, int alVal)=0;
		virtual void SetTextureConstantColor(const cColor &aColor)=0;

		//BLENDING
		virtual void SetBlendActive(bool abX)=0;
		virtual void SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor)=0;
		virtual void SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
										eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha)=0;

		// POLYGONS
		virtual iVertexBuffer* CreateVertexBuffer(tVertexFlag aFlags, eVertexBufferDrawType aDrawType,
								eVertexBufferUsageType aUsageType,
								int alReserveVtxSize=0,int alReserveIdxSize=0)=0;

		virtual void DrawQuad(const tVertexVec &avVtx)=0;
		virtual void DrawQuad(const tVertexVec &avVtx, const cColor aCol)=0;
		virtual void DrawQuadMultiTex(const tVertexVec &avVtx,const tVector3fVec &avExtraUvs)=0;

		//VERTEX BATCHER
		virtual void DrawBatch(const cGfxBatch &batch, tGfxBatchAttrs attrs, eBatchDrawMode drawMode)=0;

		//some primitive:
		virtual void DrawLine(const cVector3f& avBegin, const cVector3f& avEnd, cColor aCol)=0;
		virtual void DrawBoxMaxMin(const cVector3f& avMax, const cVector3f& avMin, cColor aCol)=0;
		virtual void DrawSphere(const cVector3f& avPos, float afRadius, cColor aCol)=0;

		// GENERAL
		/**
		 * Copies the current frame buffer to a texture.
		 * \param apTex The texture the framebuffer is copied to.
		 * \param &avPos The Screenpositon
		 * \param &avSize The size of the screen.
		 * \param &avTexOffset The position on the texture.
		 */
		virtual void CopyContextToTexure(iTexture* apTex, const cVector2l &avPos,
										const cVector2l &avSize, const cVector2l &avTexOffset=0)=0;

		virtual void StartFrame()=0;
		virtual void FlushRendering()=0;
		virtual void SwapBuffers()=0;
		virtual void EndFrame()=0;
	};
};
#endif // HPL_LOWLEVELGRAPHICS_H
