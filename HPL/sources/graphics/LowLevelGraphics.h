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

#include "graphics/Sampler.h"
#include "graphics/Bitmap.h"
#include "graphics/Texture.h"
#include "graphics/GPUProgram.h"
#include "graphics/VertexBuffer.h"
#include "graphics/GfxBatch.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/StringTypes.h"

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

	enum eStencilFunc
	{
		eStencilFunc_Never,
		eStencilFunc_Less,
		eStencilFunc_LessOrEqual,
		eStencilFunc_Greater,
		eStencilFunc_GreaterOrEqual,
		eStencilFunc_Equal,
		eStencilFunc_NotEqual,
		eStencilFunc_Always
	};

	enum eStencilFace
	{
		eStencilFace_Front,
		eStencilFace_Back
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
		eDepthTestFunc_Always
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
		eAlphaTestFunc_Always
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
	};

	enum eCullMode
	{
		eCullMode_Clockwise,
		eCullMode_CounterClockwise
	};

	class iOcclusionQuery;

	struct Sampler {
		int _id;
	};

	class iLowLevelGraphics
	{
	public:
		virtual ~iLowLevelGraphics() = default;
		/**
		 * Sets the video mode. Must only be called ONCE!
		 * \param alWidth
		 * \param alHeight
		 * \param abFullscreen
		 * \return
		 */
		virtual bool Init(int alWidth, int alHeight, bool abFullscreen,	const tString& asWindowCaption) = 0;

		/**
		 * Show the cursor or not. Default is false
		 * \param aX
		 */
		virtual void ShowCursor(bool abX)=0;
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

		// RESOURCES
		virtual iTexture* CreateTexture(const tString &asName, eTextureTarget aTarget)=0;
		virtual iGpuProgram* CreateGpuProgram(const tString& asName)=0;
		virtual iVertexBuffer* CreateVertexBuffer(VertexAttributes aFlags, VertexBufferPrimitiveType aDrawType,
								VertexBufferUsageType aUsageType,
								int alReserveVtxSize, int alReserveIdxSize)=0;
		virtual Sampler CreateSampler(const SamplerDesc& descriptor) = 0;

		virtual Bitmap GetScreenPixels()=0;

		/////////// DRAWING METHODS /////////////////////////

		// OCCLUSION
		virtual iOcclusionQuery* CreateOcclusionQuery()=0;
		virtual void DestroyOcclusionQuery(iOcclusionQuery *apQuery)=0;

		// CLEARING THE FRAMEBUFFER
		virtual void ClearScreen()=0;

		virtual void SetClearColor(const cColor& aCol)=0;
		virtual void SetClearDepth(float afDepth)=0;
		virtual void SetClearStencil(int alVal)=0;

		virtual void SetColorWriteActive(bool abR,bool abG,bool abB,bool abA)=0;
		virtual void SetDepthWriteActive(bool abX)=0;

		virtual void SetCullActive(bool abX)=0;
		virtual void SetCullMode(eCullMode aMode)=0;

		//DEPTH
		virtual void SetDepthTestActive(bool abX)=0;
		virtual void SetDepthTestFunc(eDepthTestFunc aFunc)=0;

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

		//TEXTURE + SAMPLER
		virtual void SetTexture(unsigned int alUnit,iTexture* apTex)=0;

		//BLENDING
		virtual void SetBlendActive(bool abX)=0;
		virtual void SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor)=0;
		virtual void SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
										eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha)=0;

		// FRAME
		virtual void StartFrame()=0;
		virtual void SwapBuffers()=0;
		virtual void EndFrame()=0;
	};
};
#endif // HPL_LOWLEVELGRAPHICS_H
