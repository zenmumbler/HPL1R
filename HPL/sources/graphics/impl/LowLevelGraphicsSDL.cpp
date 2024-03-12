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
#ifdef WIN32
#pragma comment(lib, "OpenGL32.lib")
#endif

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif

#include <assert.h>
#include <stdlib.h>

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "graphics/Bitmap.h"

#include "graphics/impl/LowLevelGraphicsSDL.h"

#include "graphics/ogl2/SDLTexture.h"
#include "graphics/ogl2/GLSLProgram.h"
#include "graphics/ogl2/VertexBufferVBO.h"
#include "graphics/ogl2/OcclusionQueryOGL.h"

#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static GLenum GetGLBlendEnum(eBlendFunc aType)
	{
		switch(aType)
		{
			case eBlendFunc_Zero:					return GL_ZERO;
			case eBlendFunc_One:					return GL_ONE;
			case eBlendFunc_SrcColor:				return GL_SRC_COLOR;
			case eBlendFunc_OneMinusSrcColor:		return GL_ONE_MINUS_SRC_COLOR;
			case eBlendFunc_DestColor:				return GL_DST_COLOR;
			case eBlendFunc_OneMinusDestColor:		return GL_ONE_MINUS_DST_COLOR;
			case eBlendFunc_SrcAlpha:				return GL_SRC_ALPHA;
			case eBlendFunc_OneMinusSrcAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
			case eBlendFunc_DestAlpha:				return GL_DST_ALPHA;
			case eBlendFunc_OneMinusDestAlpha:		return GL_ONE_MINUS_DST_ALPHA;
			case eBlendFunc_SrcAlphaSaturate:		return GL_SRC_ALPHA_SATURATE;
			default: return 0;
		}
	}

	//-----------------------------------------------------------------------

	static GLenum GetGLDepthTestFuncEnum(eDepthTestFunc aType)
	{
		switch(aType)
		{
			case eDepthTestFunc_Never:			return GL_NEVER;
			case eDepthTestFunc_Less:			return GL_LESS;
			case eDepthTestFunc_LessOrEqual:	return GL_LEQUAL;
			case eDepthTestFunc_Greater:		return GL_GREATER;
			case eDepthTestFunc_GreaterOrEqual:	return GL_GEQUAL;
			case eDepthTestFunc_Equal:			return GL_EQUAL;
			case eDepthTestFunc_NotEqual:		return GL_NOTEQUAL;
			case eDepthTestFunc_Always:			return GL_ALWAYS;
			default: return 0;
		}
	}

	//-----------------------------------------------------------------------

	static GLenum GetGLStencilFuncEnum(eStencilFunc aType)
	{
		switch(aType)
		{
			case eStencilFunc_Never:			return GL_NEVER;
			case eStencilFunc_Less:				return GL_LESS;
			case eStencilFunc_LessOrEqual:		return GL_LEQUAL;
			case eStencilFunc_Greater:			return GL_GREATER;
			case eStencilFunc_GreaterOrEqual:	return GL_GEQUAL;
			case eStencilFunc_Equal:			return GL_EQUAL;
			case eStencilFunc_NotEqual:			return GL_NOTEQUAL;
			case eStencilFunc_Always:			return GL_ALWAYS;
			default: return 0;
		}
	}

	//-----------------------------------------------------------------------

	static GLenum GetGLStencilOpEnum(eStencilOp aType)
	{
		switch(aType) {
			case eStencilOp_Keep:			return GL_KEEP;
			case eStencilOp_Zero:			return GL_ZERO;
			case eStencilOp_Replace:		return GL_REPLACE;
			case eStencilOp_Increment:		return GL_INCR;
			case eStencilOp_Decrement:		return GL_DECR;
			case eStencilOp_Invert:			return GL_INVERT;
			case eStencilOp_IncrementWrap:	return GL_INCR_WRAP;
			case eStencilOp_DecrementWrap:	return GL_DECR_WRAP;
			default: return 0;
		}
	}

	//-------------------------------------------------

	// used by SDLTexture.cpp
	GLenum TextureTargetToGL(eTextureTarget aTarget)
	{
		switch (aTarget) {
			case eTextureTarget_1D:		return GL_TEXTURE_1D;
			case eTextureTarget_2D:		return GL_TEXTURE_2D;
			case eTextureTarget_CubeMap:return GL_TEXTURE_CUBE_MAP;
			case eTextureTarget_3D:		return GL_TEXTURE_3D;
			default:					return 0;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelGraphicsSDL::cLowLevelGraphicsSDL() : iLowLevelGraphics()
	{
		mvVirtualSize.x = 800;
		mvVirtualSize.y = 600;

		for (int i = 0; i < MAX_TEXTUREUNITS; i++) {
			mpCurrentTexture[i] = NULL;
		}
	}

	//-----------------------------------------------------------------------

	cLowLevelGraphicsSDL::~cLowLevelGraphicsSDL()
	{

		if (mpImGuiContext) {
			ImGui_ImplSDL2_Shutdown();
			ImGui_ImplOpenGL3_Shutdown();
			ImGui::DestroyContext(mpImGuiContext);
		}

		if (mpGLContext) {
			SDL_GL_DeleteContext(mpGLContext);
			mpGLContext = NULL;
		}

		if (mpWindow) {
			SDL_DestroyWindow(mpWindow);
			mpWindow = NULL;
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cLowLevelGraphicsSDL::Init(int alWidth, int alHeight, bool abFullscreen, const tString& asWindowCaption)
	{
		mvScreenSize.x = alWidth;
		mvScreenSize.y = alHeight;

		// Request GL Core Profile context
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		unsigned int mlFlags = SDL_WINDOW_OPENGL;

		if (abFullscreen)
			mlFlags |= SDL_WINDOW_FULLSCREEN;

		Log(" Creating display: %d x %d\n", alWidth, alHeight);
		mpWindow = SDL_CreateWindow(asWindowCaption.c_str(),
									SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
									alWidth, alHeight, mlFlags);
		if (mpWindow == NULL) {
			FatalError("Unable to initialize display!\n");
			return false;
		}
		
		// GL context
		Log(" Setting up OpenGL\n");
		mpGLContext = SDL_GL_CreateContext(mpWindow);
		if (mpGLContext == NULL) {
			Error(SDL_GetError());
			FatalError("Unable to create GL context!\n");
			return false;
		}

		// GL defaults
		SetClearColor(cColor::Black);
		SetClearDepth(1.0f);
		SetClearStencil(0);
		SetCullMode(eCullMode_CounterClockwise);
		SetDepthTestActive(true);
		SetDepthTestFunc(eDepthTestFunc_Equal);

		// ImGui context
		mpImGuiContext = ImGui::CreateContext();
		ImGui_ImplOpenGL3_Init();
		ImGui_ImplSDL2_InitForOpenGL(mpWindow, mpGLContext);

		// Hide cursor by default
		ShowCursor(false);

		return true;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ShowCursor(bool abX)
	{
		if(abX)
			SDL_ShowCursor(SDL_ENABLE);
		else
			SDL_ShowCursor(SDL_DISABLE);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetInputGrab(bool abX) {
//		SDL_SetWindowGrab(mpWindow, abX ? SDL_TRUE : SDL_FALSE);
		SDL_SetRelativeMouseMode(abX ? SDL_TRUE : SDL_FALSE);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetVsyncActive(bool abX)
	{
		SDL_GL_SetSwapInterval(abX ? 1 : 0);
	}

	//-----------------------------------------------------------------------

	Bitmap cLowLevelGraphicsSDL::GetScreenPixels()
	{
		glFinish();

		Bitmap bmp;
		bmp.Create(mvScreenSize.x, mvScreenSize.y);

		auto pixels = bmp.GetRawData<unsigned char>();

		glReadBuffer(GL_BACK);
		glReadPixels(0, 0, mvScreenSize.x, mvScreenSize.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		return bmp;
	}

	//-----------------------------------------------------------------------

	iGpuProgram* cLowLevelGraphicsSDL::CreateGpuProgram(const tString& asName)
	{
		return new cGLSLProgram(asName);
	}

	//-----------------------------------------------------------------------

	iTexture* cLowLevelGraphicsSDL::CreateTexture(const tString &asName, eTextureTarget aTarget)
	{
		return new cSDLTexture(asName, aTarget);
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cLowLevelGraphicsSDL::CreateVertexBuffer(VertexAttributes aFlags,
														VertexBufferPrimitiveType aDrawType,
														VertexBufferUsageType aUsageType,
														int alReserveVtxSize,int alReserveIdxSize)
	{
		return new cVertexBufferVBO(aFlags, aDrawType, aUsageType, alReserveVtxSize, alReserveIdxSize);
	}

	//-----------------------------------------------------------------------

	Sampler cLowLevelGraphicsSDL::CreateSampler(const SamplerDesc& descriptor)
	{
		return { 0 };
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetTexture(unsigned int alUnit, iTexture* apTex)
	{
		if(apTex == mpCurrentTexture[alUnit]) return;

		glActiveTexture(GL_TEXTURE0 + alUnit);

		if (apTex != NULL) {
			GLenum target = TextureTargetToGL(apTex->GetTarget());
			glBindTexture(target, apTex->GetCurrentLowlevelHandle());
		}
		else {
			// glBindTexture(GL_TEXTURE_2D, 0); // use arbitrary target
		}

		mpCurrentTexture[alUnit] = apTex;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::StartFrame() {
		ClearScreen();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}

	void cLowLevelGraphicsSDL::SwapBuffers()
	{
		SDL_GL_SwapWindow(mpWindow);
	}
	
	void cLowLevelGraphicsSDL::EndFrame() {
		// render the debug views last on top of everything else
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SwapBuffers();
	}

	//-----------------------------------------------------------------------

	iOcclusionQuery* cLowLevelGraphicsSDL::CreateOcclusionQuery()
	{
		return new cOcclusionQueryOGL();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DestroyOcclusionQuery(iOcclusionQuery *apQuery)
	{
		if (apQuery) delete apQuery;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ClearScreen()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetClearColor(const cColor& aCol){
		glClearColor(aCol.r, aCol.g, aCol.b, aCol.a);
	}
	void cLowLevelGraphicsSDL::SetClearDepth(float afDepth){
		glClearDepth(afDepth);
	}
	void cLowLevelGraphicsSDL::SetClearStencil(int alVal){
		glClearStencil(alVal);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetColorWriteActive(bool abR,bool abG,bool abB,bool abA)
	{
		glColorMask(abR,abG,abB,abA);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetDepthWriteActive(bool abX)
	{
		glDepthMask(abX);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetDepthTestActive(bool abX)
	{
		if(abX) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetDepthTestFunc(eDepthTestFunc aFunc)
	{
		glDepthFunc(GetGLDepthTestFuncEnum(aFunc));
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetStencilActive(bool abX)
	{
		if(abX) glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
					eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp)
	{
		glStencilFunc(GetGLStencilFuncEnum(aFunc), alRef, aMask);

		glStencilOp(GetGLStencilOpEnum(aFailOp), GetGLStencilOpEnum(aZFailOp),
					GetGLStencilOpEnum(aZPassOp));
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetStencilTwoSide(eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
					int alRef, unsigned int aMask,
					eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
					eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp)
	{
		glStencilOpSeparate(GL_FRONT, GetGLStencilOpEnum(aFrontFailOp),
							GetGLStencilOpEnum(aFrontZFailOp),
							GetGLStencilOpEnum(aFrontZPassOp));
		glStencilOpSeparate(GL_BACK, GetGLStencilOpEnum(aBackFailOp),
							GetGLStencilOpEnum(aBackZFailOp),
							GetGLStencilOpEnum(aBackZPassOp));

		glStencilFuncSeparate(GL_FRONT,
							  GetGLStencilFuncEnum(aBackFunc),
							  alRef, aMask);
		glStencilFuncSeparate(GL_BACK,
							  GetGLStencilFuncEnum(aBackFunc),
							  alRef, aMask);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetCullActive(bool abX)
	{
		if(abX) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
	}

	void cLowLevelGraphicsSDL::SetCullMode(eCullMode aMode)
	{
		if(aMode == eCullMode_Clockwise) glFrontFace(GL_CCW);
		else							glFrontFace(GL_CW);
		glCullFace(GL_BACK);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetScissorActive(bool abX)
	{
		if(abX) glEnable(GL_SCISSOR_TEST);
		else glDisable(GL_SCISSOR_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetScissorRect(const cRect2l &aRect)
	{
		glScissor(aRect.x, (mvScreenSize.y - aRect.y - 1)-aRect.h, aRect.w, aRect.h);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetBlendActive(bool abX)
	{
		if(abX)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor)
	{
		glBlendFunc(GetGLBlendEnum(aSrcFactor),GetGLBlendEnum(aDestFactor));
	}

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
		eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha)
	{
		glBlendFuncSeparate(GetGLBlendEnum(aSrcFactorColor),
							GetGLBlendEnum(aDestFactorColor),
							GetGLBlendEnum(aSrcFactorAlpha),
							GetGLBlendEnum(aDestFactorAlpha));
	}

	//-----------------------------------------------------------------------

	cVector2f cLowLevelGraphicsSDL::GetScreenSize()
	{
		return cVector2f((float)mvScreenSize.x, (float)mvScreenSize.y);
	}

	//-----------------------------------------------------------------------

	cVector2f cLowLevelGraphicsSDL::GetVirtualSize()
	{
		return mvVirtualSize;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetVirtualSize(cVector2f avSize)
	{
		mvVirtualSize = avSize;
	}

	//-----------------------------------------------------------------------

}
