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
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "GLaux.lib")
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <assert.h>
#include <stdlib.h>

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_opengl2.h"

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

	GLenum ColorFormatToGL(eColorDataFormat aFormat)
	{
		switch (aFormat) {
			case eColorDataFormat_RGB:		return GL_RGB;
			case eColorDataFormat_RGBA:		return GL_RGBA;
			case eColorDataFormat_ALPHA:	return GL_ALPHA;
			case eColorDataFormat_BGR:		return GL_BGR;
			case eColorDataFormat_BGRA:		return GL_BGRA;
			default:						return 0;
		}
	}

	//-------------------------------------------------

	GLenum TextureTargetToGL(eTextureTarget aTarget)
	{
		switch (aTarget) {
			case eTextureTarget_1D:		return GL_TEXTURE_1D;
			case eTextureTarget_2D:		return GL_TEXTURE_2D;
			case eTextureTarget_Rect:	return GL_TEXTURE_RECTANGLE_ARB;
			case eTextureTarget_CubeMap:	return GL_TEXTURE_CUBE_MAP;
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
		mlMultisampling = 0;

		mvVirtualSize.x = 800;
		mvVirtualSize.y = 600;

		for (int i = 0; i < MAX_TEXTUREUNITS; i++) {
			mpCurrentTexture[i] = NULL;
		}

		mbClearColor = true;
		mbClearDepth = true;
		mbClearStencil = false;
	}

	//-----------------------------------------------------------------------

	cLowLevelGraphicsSDL::~cLowLevelGraphicsSDL()
	{
		if (mpImGuiContext) {
			ImGui_ImplSDL2_Shutdown();
			ImGui_ImplOpenGL2_Shutdown();
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

	bool cLowLevelGraphicsSDL::Init(int alWidth, int alHeight, bool abFullscreen,
									int alMultisampling, const tString& asWindowCaption)
	{
		mvScreenSize.x = alWidth;
		mvScreenSize.y = alHeight;

		mlMultisampling = alMultisampling;

		//Set some GL Attributes
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		// Multisampling
		if(mlMultisampling > 0)
		{
			if(SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1)==-1)
			{
				Error("Multisample buffers not supported!\n");
			}
			else
			{
				if(SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, mlMultisampling)==-1)
				{
					Error("Couldn't set multisampling samples to %d\n",mlMultisampling);
				}
			}
		}

		unsigned int mlFlags = SDL_WINDOW_OPENGL;

		if(abFullscreen)
			mlFlags |= SDL_WINDOW_FULLSCREEN;

		Log(" Creating display: %d x %d\n", alWidth, alHeight);
		mpWindow = SDL_CreateWindow(asWindowCaption.c_str(),
									SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
									alWidth, alHeight, mlFlags);
		if(mpWindow == NULL) {
			Error("Could not set display mode setting a lower one!\n");
			mvScreenSize = cVector2l(800,600);
			mpWindow = SDL_CreateWindow(asWindowCaption.c_str(),
										SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										mvScreenSize.x, mvScreenSize.y, mlFlags);

			if(mpWindow == NULL)
			{
				FatalError("Unable to initialize display!\n");
				return false;
			}
			else
			{
				Warning("Could not set displaymode and 800x600 is used instead!\n");
			}
		}
		
		mpGLContext = SDL_GL_CreateContext(mpWindow);
		if(mpGLContext == NULL) {
			Error(SDL_GetError());
			FatalError("Unable to create GL context!\n");
			return false;
		}

		//Turn off cursor as default
		ShowCursor(false);

		//GL
		Log(" Setting up OpenGL\n");
		SetupGL();

		//Set the clear color
		SDL_GL_SwapWindow(mpWindow);
		
		//Setup ImGui
		mpImGuiContext = ImGui::CreateContext();
		ImGui_ImplOpenGL2_Init();
		ImGui_ImplSDL2_InitForOpenGL(mpWindow, mpGLContext);

		return true;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetupGL()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClearStencil(0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_ALPHA_TEST);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//Show some info
		Log("  Max texture image units: %d\n",GetCaps(eGraphicCaps_MaxTextureImageUnits));
		Log("  Max texture coord units: %d\n",GetCaps(eGraphicCaps_MaxTextureCoordUnits));

		if (GetCaps(eGraphicCaps_AnisotropicFiltering))
			Log("  Max Anisotropic degree: %d\n",GetCaps(eGraphicCaps_MaxAnisotropicFiltering));
	}
	//-----------------------------------------------------------------------

	int cLowLevelGraphicsSDL::GetCaps(eGraphicCaps aType)
	{
		switch(aType)
		{
			//Max Texture Image Units
			case eGraphicCaps_MaxTextureImageUnits:
			{
				//DEBUG:
				//return 2;

				int lUnits;
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,(GLint *)&lUnits);
				return lUnits;
			}

			//Max Texture Coord Units
			case eGraphicCaps_MaxTextureCoordUnits:
			{
				int lUnits;
				glGetIntegerv(GL_MAX_TEXTURE_COORDS,(GLint *)&lUnits);
				return lUnits;
			}
			//Texture Anisotropy
			case eGraphicCaps_AnisotropicFiltering:
			{
				return SDL_GL_ExtensionSupported("GL_EXT_texture_filter_anisotropic") ? 1 : 0;
			}

			//Texture Anisotropy
			case eGraphicCaps_MaxAnisotropicFiltering:
			{
				if (SDL_GL_ExtensionSupported("GL_EXT_texture_filter_anisotropic") == false) {
					return 0;
				}

				float fMax;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&fMax);
				return (int)fMax;
			}
		
			default:
				return 0;
		}
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
		#if defined(WIN32)
		if(GLEE_WGL_EXT_swap_control)
		{
			wglSwapIntervalEXT(abX ? 1 : 0);
		}
		#elif defined(__linux__)
		if (GLEE_GLX_SGI_swap_control)
		{
			glXSwapIntervalSGI(abX ? 1 : 0);
		}
		#endif
	}

	//-----------------------------------------------------------------------

	Bitmap cLowLevelGraphicsSDL::GetScreenPixels()
	{
		glFinish();

		Bitmap bmp{mvScreenSize.x, mvScreenSize.y};

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
		return new cSDLTexture(asName, this, aTarget);
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

	void cLowLevelGraphicsSDL::SetTexture(unsigned int alUnit, iTexture* apTex)
	{
		if(apTex == mpCurrentTexture[alUnit]) return;

		glActiveTexture(GL_TEXTURE0 + alUnit);

		if (apTex != NULL) {
			GLenum target = GetGLTextureTargetEnum(apTex->GetTarget());
			cSDLTexture *pSDLTex = static_cast<cSDLTexture*>(apTex);
			glBindTexture(target, pSDLTex->GetTextureHandle());
		}
		else {
			glBindTexture(GL_TEXTURE_2D, 0); // use arbitrary target
		}

		mpCurrentTexture[alUnit] = apTex;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::StartFrame() {
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}
	
	void cLowLevelGraphicsSDL::FlushRendering()
	{
		glFlush();
	}

	void cLowLevelGraphicsSDL::SwapBuffers()
	{
		glFlush();
		SDL_GL_SwapWindow(mpWindow);
	}
	
	void cLowLevelGraphicsSDL::EndFrame() {
		// render the debug views last on top of everything else
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		
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
		if(apQuery)	delete apQuery;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ClearScreen()
	{
		GLbitfield bitmask = 0;

		if (mbClearColor)   bitmask |= GL_COLOR_BUFFER_BIT;
		if (mbClearDepth)   bitmask |= GL_DEPTH_BUFFER_BIT;
		if (mbClearStencil) bitmask |= GL_STENCIL_BUFFER_BIT;

		glClear(bitmask);
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

	void cLowLevelGraphicsSDL::SetClearColorActive(bool abX){
		mbClearColor=abX;
	}
	void cLowLevelGraphicsSDL::SetClearDepthActive(bool abX){
		mbClearDepth=abX;
	}
	void cLowLevelGraphicsSDL::SetClearStencilActive(bool abX){
		mbClearStencil=abX;
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

	void cLowLevelGraphicsSDL::SetAlphaTestActive(bool abX)
	{
		if(abX) glEnable(GL_ALPHA_TEST);
		else glDisable(GL_ALPHA_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef)
	{
		glAlphaFunc(GetGLAlphaTestFuncEnum(aFunc),afRef);
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
		glCullFace(GL_BACK);
	}

	void cLowLevelGraphicsSDL::SetCullMode(eCullMode aMode)
	{
		glCullFace(GL_BACK);
		if(aMode == eCullMode_Clockwise) glFrontFace(GL_CCW);
		else							glFrontFace(GL_CW);
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

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	GLenum cLowLevelGraphicsSDL::GetGLBlendEnum(eBlendFunc aType)
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

	GLenum cLowLevelGraphicsSDL::GetGLTextureTargetEnum(eTextureTarget aType)
	{
		switch(aType)
		{
			case eTextureTarget_1D:		return GL_TEXTURE_1D;
			case eTextureTarget_2D:		return GL_TEXTURE_2D;
			case eTextureTarget_Rect:   return GL_TEXTURE_RECTANGLE_ARB;
			case eTextureTarget_CubeMap:	return GL_TEXTURE_CUBE_MAP;
			case eTextureTarget_3D:		return GL_TEXTURE_3D;
			default: return 0;
		}
	}

	//-----------------------------------------------------------------------

	GLenum cLowLevelGraphicsSDL::GetGLDepthTestFuncEnum(eDepthTestFunc aType)
	{
		switch(aType)
		{
			case eDepthTestFunc_Never:			return GL_NEVER;
			case eDepthTestFunc_Less:				return GL_LESS;
			case eDepthTestFunc_LessOrEqual:		return GL_LEQUAL;
			case eDepthTestFunc_Greater:			return GL_GREATER;
			case eDepthTestFunc_GreaterOrEqual:	return GL_GEQUAL;
			case eDepthTestFunc_Equal:			return GL_EQUAL;
			case eDepthTestFunc_NotEqual:			return GL_NOTEQUAL;
			case eDepthTestFunc_Always:			return GL_ALWAYS;
			default: return 0;
		}
	}

	//-----------------------------------------------------------------------

	GLenum cLowLevelGraphicsSDL::GetGLAlphaTestFuncEnum(eAlphaTestFunc aType)
	{
		switch(aType)
		{
			case eAlphaTestFunc_Never:			return GL_NEVER;
			case eAlphaTestFunc_Less:				return GL_LESS;
			case eAlphaTestFunc_LessOrEqual:		return GL_LEQUAL;
			case eAlphaTestFunc_Greater:			return GL_GREATER;
			case eAlphaTestFunc_GreaterOrEqual:	return GL_GEQUAL;
			case eAlphaTestFunc_Equal:			return GL_EQUAL;
			case eAlphaTestFunc_NotEqual:			return GL_NOTEQUAL;
			case eAlphaTestFunc_Always:			return GL_ALWAYS;
			default: return 0;
		}
	}

	//-----------------------------------------------------------------------

	GLenum cLowLevelGraphicsSDL::GetGLStencilFuncEnum(eStencilFunc aType)
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

	GLenum cLowLevelGraphicsSDL::GetGLStencilOpEnum(eStencilOp aType)
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

	//-----------------------------------------------------------------------

}
