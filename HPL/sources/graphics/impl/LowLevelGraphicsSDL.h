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

	GLenum TextureTargetToGL(eTextureTarget aTarget);

	//-------------------------------------------------

	class cLowLevelGraphicsSDL : public iLowLevelGraphics
	{
	public:
		cLowLevelGraphicsSDL();
		~cLowLevelGraphicsSDL();

		bool Init(int alWidth, int alHeight, bool abFullscreen, const tString& asWindowCaption) override;

		void ShowCursor(bool abX) override;
		void SetInputGrab(bool abX);

		void SetVsyncActive(bool abX) override;

		cVector2f GetScreenSize() override;
		cVector2f GetVirtualSize() override;
		void SetVirtualSize(cVector2f avSize) override;

		// RESOURCES
		iTexture* CreateTexture(const tString &asName, eTextureTarget aTarget) override;
		iGpuProgram* CreateGpuProgram(const tString& asName) override;
		iVertexBuffer* CreateVertexBuffer(VertexAttributes aFlags, VertexBufferPrimitiveType aDrawType,
										VertexBufferUsageType aUsageType,
										int alReserveVtxSize, int alReserveIdxSize) override;
		Sampler CreateSampler(const SamplerDesc& descriptor) override;

		Bitmap GetScreenPixels() override;

		/////////// DRAWING METHODS /////////////////////////

		// OCCLUSION
		iOcclusionQuery* CreateOcclusionQuery() override;
		void DestroyOcclusionQuery(iOcclusionQuery *apQuery) override;

		// CLEARING THE FRAMEBUFFER
		void ClearScreen() override;

		void SetClearColor(const cColor& aCol) override;
		void SetClearDepth(float afDepth) override;
		void SetClearStencil(int alVal) override;

		void SetColorWriteActive(bool abR,bool abG,bool abB,bool abA) override;
		void SetDepthWriteActive(bool abX) override;

		void SetCullActive(bool abX) override;
		void SetCullMode(eCullMode aMode) override;

		//DEPTH
		void SetDepthTestActive(bool abX) override;
		void SetDepthTestFunc(eDepthTestFunc aFunc) override;

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

		// FRAME
		void StartFrame() override;
		void SwapBuffers() override;
		void EndFrame() override;

	private:
		cVector2l mvScreenSize;
		cVector2f mvVirtualSize;

		//SDL Variables
		SDL_Window *mpWindow;
		SDL_GLContext mpGLContext;

		//Texture
		iTexture* mpCurrentTexture[MAX_TEXTUREUNITS];

		// ImGui Setup
		ImGuiContext* mpImGuiContext;
	};
};
#endif // HPL_LOWLEVELGRAPHICS_SDL_H
