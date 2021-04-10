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
#pragma comment(lib, "Cg.lib")
#pragma comment(lib, "CgGL.lib")
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <assert.h>
#include <stdlib.h>

#include "graphics/FontData.h"

#include "graphics/impl/LowLevelGraphicsSDL.h"
#include "graphics/impl/SDLBitmap2D.h"
#include "graphics/impl/SDLFontData.h"

#include "graphics/ogl2/SDLTexture.h"
#include "graphics/ogl2/CGProgram.h"
#include "graphics/ogl2/VertexBufferVBO.h"
#include "graphics/ogl2/OcclusionQueryOGL.h"

#include "system/LowLevelSystem.h"

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

	cLowLevelGraphicsSDL::cLowLevelGraphicsSDL()
	{
		mlBatchArraySize = 20000;
		mlVertexCount = 0;
		mlIndexCount = 0;
		mlMultisampling = 0;

		mvVirtualSize.x = 800;
		mvVirtualSize.y = 600;

		mpRenderTarget = NULL;

		for (int i = 0; i < MAX_TEXTUREUNITS; i++) {
			mpCurrentTexture[i] = NULL;
		}

		mbClearColor = true;
		mbClearDepth = true;
		mbClearStencil = false;

		//Create the batch arrays:
		mlBatchStride = 13;
		//3 Pos floats, 4 color floats, 3 Tex coord floats .
		mpVertexArray = (float*)hplMalloc(sizeof(float) * mlBatchStride * mlBatchArraySize);
		mpIndexArray = (unsigned int*)hplMalloc(sizeof(unsigned int) * mlBatchArraySize); //Index is one int.

		//Init extra stuff
		InitCG();
	}

	//-----------------------------------------------------------------------

	cLowLevelGraphicsSDL::~cLowLevelGraphicsSDL()
	{
		hplFree(mpVertexArray);
		hplFree(mpIndexArray);
		
		if (mpGLContext) {
			SDL_GL_DeleteContext(mpGLContext);
			mpGLContext = NULL;
		}

		if (mpWindow) {
			SDL_SetWindowGammaRamp(mpWindow, mvStartGammaArray[0],mvStartGammaArray[1],mvStartGammaArray[2]);
			SDL_DestroyWindow(mpWindow);
			mpWindow = NULL;
		}

		//Exit extra stuff
		ExitCG();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cLowLevelGraphicsSDL::Init(int alWidth, int alHeight, int alBpp, int abFullscreen,
									int alMultisampling, const tString& asWindowCaption)
	{
		mvScreenSize.x = alWidth;
		mvScreenSize.y = alHeight;
		mlBpp = alBpp;

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

		Log(" Creating display: %d x %d - %d bpp\n",alWidth, alHeight, alBpp);
		mpWindow = SDL_CreateWindow(asWindowCaption.c_str(),
									SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
									alWidth, alHeight, mlFlags);
		if(mpWindow == NULL) {
			Error("Could not set display mode setting a lower one!\n");
			mvScreenSize = cVector2l(640,480);
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
				CreateMessageBoxW(_W("Warning!"),
									_W("Could not set displaymode and 640x480 is used instead!\n"));
			}
		}
		
		mpGLContext = SDL_GL_CreateContext(mpWindow);
		if(mpGLContext == NULL) {
			Error(SDL_GetError());
			FatalError("Unable to create GL context!\n");
			return false;
		}

		//Check Multisample properties
		CheckMultisampleCaps();

		//Turn off cursor as default
		ShowCursor(false);

		//Gamma
		mfGammaCorrection = 1.0f;
		SDL_GetWindowGammaRamp(mpWindow, mvStartGammaArray[0],mvStartGammaArray[1],mvStartGammaArray[2]);

		SDL_SetWindowBrightness(mpWindow, mfGammaCorrection);

		//GL
		Log(" Setting up OpenGL\n");
		SetupGL();

		//Set the clear color
		SDL_GL_SwapWindow(mpWindow);

		return true;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::CheckMultisampleCaps()
	{

	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetupGL()
	{
		//Inits GL stuff
		//Set Shade model and clear color.
		glShadeModel(GL_SMOOTH);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		//Depth Test setup
		glClearDepth(1.0f);//VAlues buffer is cleared with
		glEnable(GL_DEPTH_TEST); //enable depth testing
		glDepthFunc(GL_LEQUAL); //function to do depth test with
		glDisable(GL_ALPHA_TEST);

		//Set best perspective correction
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		//int lStencilBits=-1;
		//glGetIntegerv(GL_STENCIL_BITS,&lStencilBits);
		//Log(" Stencil bits: %d\n",lStencilBits);

		//Stencil setup
		glClearStencil(0);

		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

		/////  BEGIN BATCH ARRAY STUFF ///////////////

		//Enable all the vertex arrays that are used:
		glEnableClientState(GL_VERTEX_ARRAY ); //The positions
		glEnableClientState(GL_COLOR_ARRAY ); //The color
		glEnableClientState(GL_TEXTURE_COORD_ARRAY); //Tex coords
		glDisableClientState(GL_NORMAL_ARRAY);
		//Disable the once not used.
		glDisableClientState(GL_INDEX_ARRAY); //color index
		glDisableClientState(GL_EDGE_FLAG_ARRAY);

		///// END BATCH ARRAY STUFF ///////////////

		//Show some info
		Log("  Max texture image units: %d\n",GetCaps(eGraphicCaps_MaxTextureImageUnits));
		Log("  Max texture coord units: %d\n",GetCaps(eGraphicCaps_MaxTextureCoordUnits));
		Log("  Vertex Buffer Object: %d\n",GetCaps(eGraphicCaps_VertexBufferObject));

		Log("  Anisotropic filtering: %d\n",GetCaps(eGraphicCaps_AnisotropicFiltering));
		if(GetCaps(eGraphicCaps_AnisotropicFiltering))
			Log("  Max Anisotropic degree: %d\n",GetCaps(eGraphicCaps_MaxAnisotropicFiltering));

		Log("  Multisampling: %d\n",GetCaps(eGraphicCaps_Multisampling));

		Log("  Vertex Program: %d\n",GetCaps(eGraphicCaps_GL_VertexProgram));
		Log("  Fragment Program: %d\n",GetCaps(eGraphicCaps_GL_FragmentProgram));
	}
	//-----------------------------------------------------------------------

	int cLowLevelGraphicsSDL::GetCaps(eGraphicCaps aType)
	{
		switch(aType)
		{
		//Texture Rectangle
		case eGraphicCaps_TextureTargetRectangle:
			{
				return 1;
			}


		//Vertex Buffer Object
		case eGraphicCaps_VertexBufferObject:
			{
				return SDL_GL_ExtensionSupported("GL_ARB_vertex_buffer_object") ? 1 : 0;
			}

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

			//Multisampling
			case eGraphicCaps_Multisampling:
			{
				return SDL_GL_ExtensionSupported("GL_ARB_multisample") ? 1 : 0;
			}


			//GL Vertex program
			case eGraphicCaps_GL_VertexProgram:
			{
				//Debbug:
				//return 0;
				return SDL_GL_ExtensionSupported("GL_ARB_vertex_program") ? 1 : 0;
			}

			//GL Fragment program
			case eGraphicCaps_GL_FragmentProgram:
			{
				//Debbug:
				//return 0;
				return SDL_GL_ExtensionSupported("GL_ARB_fragment_program") ? 1 : 0;
			}

			case eGraphicCaps_GL_CoreProfile:
			{
				return 1;
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

	void cLowLevelGraphicsSDL::SetMultisamplingActive(bool abX)
	{
		if(mlMultisampling<=0) return;

		if(abX)
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetGammaCorrection(float afX)
	{
		//if(mfGammaCorrection == afX) return;

		mfGammaCorrection = afX;

		SDL_SetWindowBrightness(mpWindow, mfGammaCorrection);

		/*Uint16 GammaArray[3][256];

		for (int iIndex = 0; iIndex < 256; iIndex++)
		{
			Uint16 iArrayValue = iIndex * ((int)(afX*127.0f) + 128);

			if (iArrayValue > 65535)
				iArrayValue = 65535;

			GammaArray[0][iIndex] =
			GammaArray[1][iIndex] =
			GammaArray[2][iIndex] = iArrayValue;

		}

		//Set the GammaArray values into the display device context.
		int bReturn = SDL_SetGammaRamp(GammaArray[0],GammaArray[1],GammaArray[2]);*/
		/*if(bReturn!=-1) Log("Setting gamma worked!\n");
		else		Log("Setting gamma FAILED!\n");*/
	}

	float cLowLevelGraphicsSDL::GetGammaCorrection()
	{
		return mfGammaCorrection;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetClipPlane(int alIdx, const cPlanef& aPlane)
	{
		mvClipPlanes[alIdx] = aPlane;

		double vPlane[4];
		vPlane[0] = aPlane.a;
		vPlane[1] = aPlane.b;
		vPlane[2] = aPlane.c;
		vPlane[3] = aPlane.d;
		glClipPlane(GL_CLIP_PLANE0 + alIdx,vPlane);
	}
	cPlanef cLowLevelGraphicsSDL::GetClipPlane(int alIdx, const cPlanef& aPlane)
	{
		return mvClipPlanes[alIdx];
	}
	void cLowLevelGraphicsSDL::SetClipPlaneActive(int alIdx, bool abX)
	{
		if(abX) glEnable(GL_CLIP_PLANE0 + alIdx);
		else	glDisable(GL_CLIP_PLANE0 + alIdx);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SaveScreenToBMP(const tString& asFile)
	{
		glFinish();

		cSDLBitmap2D *pBmp = hplNew( cSDLBitmap2D, () );
		pBmp->Create(cVector2l(mvScreenSize.x,mvScreenSize.y),32);

		unsigned char *pDestPixels = (unsigned char*)pBmp->GetSurface()->pixels;
		unsigned char *pSrcPixels = (unsigned char*)hplMalloc(mvScreenSize.x * mvScreenSize.y * 4);

		SDL_LockSurface(pBmp->GetSurface());
		glReadBuffer(GL_BACK);
		glReadPixels(0,0,mvScreenSize.x,mvScreenSize.y,GL_RGBA,GL_UNSIGNED_BYTE,pSrcPixels);

		for(int y=0; y<mvScreenSize.y; ++y)
		{
			for(int x=0; x<mvScreenSize.x; ++x)
			{
				unsigned char* pDestPix = &pDestPixels[((mvScreenSize.x * y) + x) * 4];
				unsigned char* pSrcPix = &pSrcPixels[((mvScreenSize.x * ((mvScreenSize.y-1) - y))
														+ x) * 4];

				pDestPix[0] = pSrcPix[0];
				pDestPix[1] = pSrcPix[1];
				pDestPix[2] = pSrcPix[2];
				pDestPix[3] = 255;
			}
		}

		SDL_UnlockSurface(pBmp->GetSurface());
		SDL_SaveBMP(pBmp->GetSurface(),asFile.c_str());

		hplFree(pSrcPixels);
		hplDelete(pBmp);

	}

	//-----------------------------------------------------------------------

	iBitmap2D* cLowLevelGraphicsSDL::CreateBitmap2D(const cVector2l &avSize, unsigned int alBpp)
	{
		cSDLBitmap2D *pBmp = hplNew( cSDLBitmap2D, () );
		pBmp->Create(avSize,alBpp);

		return pBmp;
	}

	//-----------------------------------------------------------------------

	iFontData* cLowLevelGraphicsSDL::CreateFontData(const tString &asName)
	{
		return hplNew( cSDLFontData, (asName, this) );
	}

	//-----------------------------------------------------------------------

	iBitmap2D* cLowLevelGraphicsSDL::CreateBitmap2DFromSurface(SDL_Surface* apSurface,const tString& asType)
	{
		cSDLBitmap2D *pBmp = hplNew( cSDLBitmap2D, (apSurface,asType) );

		pBmp->msType = asType;

		return pBmp;
	}


	//-----------------------------------------------------------------------

	iGpuProgram* cLowLevelGraphicsSDL::CreateGpuProgram(const tString& asName, eGpuProgramType aType)
	{
		return hplNew( cCGProgram, (asName,mCG_Context, aType) );
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	iTexture* cLowLevelGraphicsSDL::CreateTexture(bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		return hplNew( cSDLTexture, ("",this,aType, abUseMipMaps, aTarget) );
	}

	//-----------------------------------------------------------------------

	iTexture* cLowLevelGraphicsSDL::CreateTexture(const tString &asName,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		return hplNew( cSDLTexture, (asName,this,aType, abUseMipMaps, aTarget) );
	}

	//-----------------------------------------------------------------------

	iTexture* cLowLevelGraphicsSDL::CreateTexture(iBitmap2D* apBmp,bool abUseMipMaps, eTextureType aType,
												eTextureTarget aTarget)
	{
		cSDLTexture *pTex = hplNew( cSDLTexture, ("",this,aType, abUseMipMaps, aTarget) );
		pTex->CreateFromBitmap(apBmp);

		return pTex;
	}

	//-----------------------------------------------------------------------

	iTexture* cLowLevelGraphicsSDL::CreateTexture(const cVector2l& avSize,int alBpp,cColor aFillCol,
											bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		cSDLTexture *pTex=NULL;

		if(aType==eTextureType_RenderTarget)
		{
			pTex = hplNew( cSDLTexture, ("",this,aType, abUseMipMaps, aTarget) );
			pTex->Create(avSize.x, avSize.y, aFillCol);
		}
		else
		{
			iBitmap2D* pBmp = CreateBitmap2D(avSize,alBpp);
			pBmp->FillRect(cRect2l(0,0,0,0),aFillCol);

			pTex = hplNew( cSDLTexture, ("",this,aType, abUseMipMaps, aTarget) );
			bool bRet = pTex->CreateFromBitmap(pBmp);

			hplDelete(pBmp);

			if(bRet==false){
				hplDelete(pTex);
				return NULL;
			}
		}
		return pTex;
	}


	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::PushMatrix(eMatrix aMtxType)
	{
		SetMatrixMode(aMtxType);
		glPushMatrix();
	}

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::PopMatrix(eMatrix aMtxType)
	{
		SetMatrixMode(aMtxType);
		glPopMatrix();
	}
	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA)
	{
		SetMatrixMode(aMtxType);
		cMatrixf mtxTranpose = a_mtxA.GetTranspose();
		glLoadMatrixf(mtxTranpose.v);
	}

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::SetIdentityMatrix(eMatrix aMtxType)
	{
		SetMatrixMode(aMtxType);
		glLoadIdentity();
	}

	//-----------------------------------------------------------------------


	void cLowLevelGraphicsSDL::TranslateMatrix(eMatrix aMtxType, const cVector3f &avPos)
	{
		SetMatrixMode(aMtxType);
		glTranslatef(avPos.x,avPos.y,avPos.z);
	}

	//-----------------------------------------------------------------------


	/**
	 * \todo fix so that there are X, Y , Z versions of this one.
	 * \param aMtxType
	 * \param &avRot
	 */
	void cLowLevelGraphicsSDL::RotateMatrix(eMatrix aMtxType, const cVector3f &avRot)
	{
		SetMatrixMode(aMtxType);
		glRotatef(1,avRot.x,avRot.y,avRot.z);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ScaleMatrix(eMatrix aMtxType, const cVector3f &avScale)
	{
		SetMatrixMode(aMtxType);
		glScalef(avScale.x,avScale.y,avScale.z);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,avSize.x,avSize.y,0,afMin,afMax);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetTexture(unsigned int alUnit,iTexture* apTex)
	{
		if(apTex == mpCurrentTexture[alUnit]) return;

		GLenum NewTarget=0;
		if(apTex)
			NewTarget = GetGLTextureTargetEnum(apTex->GetTarget());
		GLenum LastTarget=0;
		if(mpCurrentTexture[alUnit])
			LastTarget = GetGLTextureTargetEnum(mpCurrentTexture[alUnit]->GetTarget());

		glActiveTexture(GL_TEXTURE0 + alUnit);

		//If the current texture in this unit is a render target, unbind it.
		if(mpCurrentTexture[alUnit]){
			if(mpCurrentTexture[alUnit]->GetTextureType() == eTextureType_RenderTarget)
			{
				cSDLTexture *pSDLTex = static_cast<cSDLTexture *> (mpCurrentTexture[alUnit]);

				glBindTexture(LastTarget, pSDLTex->GetTextureHandle());
			}
		}

		//Disable this unit if NULL
		if(apTex == NULL)
		{
			glDisable(LastTarget);
			//glBindTexture(LastTarget,0);
		}
		//Enable the unit, set the texture handle
		else
		{
			if(NewTarget != LastTarget) glDisable(LastTarget);

			cSDLTexture *pSDLTex = static_cast<cSDLTexture*> (apTex);

			glBindTexture(NewTarget, pSDLTex->GetTextureHandle());
			glEnable(NewTarget);
		}

		mpCurrentTexture[alUnit] = apTex;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetActiveTextureUnit(unsigned int alUnit)
	{
		glActiveTexture(GL_TEXTURE0 + alUnit);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetTextureEnv(eTextureParam aParam, int alVal)
	{
		GLenum lParam = GetGLTextureParamEnum(aParam);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

		if(aParam==eTextureParam_ColorFunc || aParam==eTextureParam_AlphaFunc){
			glTexEnvi(GL_TEXTURE_ENV,lParam,GetGLTextureFuncEnum((eTextureFunc)alVal));
		}
		else if(aParam>=eTextureParam_ColorSource0 && aParam<=eTextureParam_AlphaSource2){
			glTexEnvi(GL_TEXTURE_ENV,lParam,GetGLTextureSourceEnum((eTextureSource)alVal));
		}
		else if(aParam>=eTextureParam_ColorOp0 && aParam<=eTextureParam_ColorOp2){
			glTexEnvi(GL_TEXTURE_ENV,lParam,GetGLTextureOpEnum((eTextureOp)alVal));
		}
		else {
			glTexEnvi(GL_TEXTURE_ENV,lParam,alVal);
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetTextureConstantColor(const cColor &aColor)
	{
		float vColor[4] = {	aColor.r, aColor.g, aColor.b, aColor.a	};

		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &vColor[0]);
	}

	//-----------------------------------------------------------------------

	iVertexBuffer* cLowLevelGraphicsSDL::CreateVertexBuffer(tVertexFlag aFlags,
														eVertexBufferDrawType aDrawType,
														eVertexBufferUsageType aUsageType,
														int alReserveVtxSize,int alReserveIdxSize)
	{
		return hplNew( cVertexBufferVBO, (this, aFlags,aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize) );
	}


	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::FlushRendering()
	{
		glFlush();
	}
	void cLowLevelGraphicsSDL::SwapBuffers()
	{
		glFlush();
		SDL_GL_SwapWindow(mpWindow);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx)
	{
		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,avVtx[i].tex.z);
				glColor4f(avVtx[i].col.r,avVtx[i].col.g,avVtx[i].col.b,avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuadMultiTex(const tVertexVec &avVtx,const tVector3fVec &avExtraUvs)
	{
		int lExtraUnits = (int)avExtraUvs.size()/4;
		glBegin(GL_QUADS);
		{
			for(int i=0;i<4;i++)
			{
				glMultiTexCoord3f(GL_TEXTURE0,avVtx[i].tex.x,avVtx[i].tex.y,avVtx[i].tex.z);

				for(int unit=0; unit<lExtraUnits; ++unit)
				{
					glMultiTexCoord3f(GL_TEXTURE0 + unit + 1,
										avExtraUvs[unit*4 + i].x, avExtraUvs[unit*4 + i].y, avExtraUvs[unit*4 + i].z);
				}

				glColor4f(avVtx[i].col.r,avVtx[i].col.g,avVtx[i].col.b,avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();

	}


	//-----------------------------------------------------------------------

	iOcclusionQuery* cLowLevelGraphicsSDL::CreateOcclusionQuery()
	{
		return hplNew(cOcclusionQueryOGL, () );
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DestroyOcclusionQuery(iOcclusionQuery *apQuery)
	{
		if(apQuery)	hplDelete(apQuery);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ClearScreen()
	{
		GLbitfield bitmask=0;

		if(mbClearColor)bitmask |= GL_COLOR_BUFFER_BIT;
		if(mbClearDepth)bitmask |= GL_DEPTH_BUFFER_BIT;
		if(mbClearStencil)bitmask |= GL_STENCIL_BUFFER_BIT;

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

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx, const cColor aCol)
	{
		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			//Make all this inline??
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,avVtx[i].tex.z);
				glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx,const float afZ)
	{
		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			//Make all this inline??
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,afZ);
				glColor4f(avVtx[i].col.r,avVtx[i].col.g,avVtx[i].col.b,avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx,const float afZ,const cColor &aCol)
	{
		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			//Make all this inline??
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,afZ);
				glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddVertexToBatch(const cVertex *apVtx, const cVector3f* avTransform)
	{
		//Coord
		mpVertexArray[mlVertexCount + 0] =	apVtx->pos.x+avTransform->x;
		mpVertexArray[mlVertexCount + 1] =	apVtx->pos.y+avTransform->y;
		mpVertexArray[mlVertexCount + 2] =	apVtx->pos.z+avTransform->z;

		/*Log("Trans: %s\n",avTransform->ToString().c_str());
		Log("Adding: %f:%f:%f\n",mpVertexArray[mlVertexCount + 0],
									mpVertexArray[mlVertexCount + 1],
									mpVertexArray[mlVertexCount + 2]);*/
		//Color
		mpVertexArray[mlVertexCount + 3] =	apVtx->col.r;
		mpVertexArray[mlVertexCount + 4] =	apVtx->col.g;
		mpVertexArray[mlVertexCount + 5] =	apVtx->col.b;
		mpVertexArray[mlVertexCount + 6] =	apVtx->col.a;
		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	apVtx->tex.x;
		mpVertexArray[mlVertexCount + 8] =	apVtx->tex.y;
		mpVertexArray[mlVertexCount + 9] =	apVtx->tex.z;

		/*Log("Tex: %f:%f:%f\n",mpVertexArray[mlVertexCount + 7],
			mpVertexArray[mlVertexCount + 8],
			mpVertexArray[mlVertexCount + 9]);*/

		//Normal coord
		mpVertexArray[mlVertexCount + 10] =	apVtx->norm.x;
		mpVertexArray[mlVertexCount + 11] =	apVtx->norm.y;
		mpVertexArray[mlVertexCount + 12] =	apVtx->norm.z;

		mlVertexCount = mlVertexCount + mlBatchStride;

		if(mlVertexCount/mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f* avTransform,
		const cColor* apCol, float mfW, float mfH)
	{
		//Coord
		mpVertexArray[mlVertexCount + 0] =	avTransform->x + mfW;
		mpVertexArray[mlVertexCount + 1] =	avTransform->y + mfH;
		mpVertexArray[mlVertexCount + 2] =	avTransform->z;

		//Color
		mpVertexArray[mlVertexCount + 3] =	apCol->r;
		mpVertexArray[mlVertexCount + 4] =	apCol->g;
		mpVertexArray[mlVertexCount + 5] =	apCol->b;
		mpVertexArray[mlVertexCount + 6] =	apCol->a;

		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	apVtx->tex.x;
		mpVertexArray[mlVertexCount + 8] =	apVtx->tex.y;
		mpVertexArray[mlVertexCount + 9] =	apVtx->tex.z;


		mlVertexCount = mlVertexCount + mlBatchStride;

		if(mlVertexCount/mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddIndexToBatch(int alIndex)
	{
		mpIndexArray[mlIndexCount] = alIndex;
		mlIndexCount++;

		if(mlIndexCount>=mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawBatch(tVtxBatchFlag aTypeFlags, eBatchDrawMode aDrawMode)
	{
		SetVtxBatchStates(aTypeFlags);
		SetUpBatchArrays();
		
		GLenum glMode = aDrawMode == eBatchDrawMode_Tris ? GL_TRIANGLES : GL_QUADS;
		glDrawElements(glMode, mlIndexCount, GL_UNSIGNED_INT, mpIndexArray);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ClearBatch()
	{
		mlIndexCount = 0;
		mlVertexCount = 0;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawLine(const cVector3f& avBegin, const cVector3f& avEnd, cColor aCol)
	{
		SetTexture(0,NULL);
		//SetBlendActive(false);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINES);
		{
			glVertex3f(avBegin.x,avBegin.y,avBegin.z);
			glVertex3f(avEnd.x,avEnd.y,avEnd.z);
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawBoxMaxMin(const cVector3f& avMax, const cVector3f& avMin, cColor aCol)
	{
		SetTexture(0,NULL);
		SetBlendActive(false);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);

		glBegin(GL_LINES);
		{
			//Pos Z Quad
			glVertex3f(avMax.x,avMax.y,avMax.z);
			glVertex3f(avMin.x,avMax.y,avMax.z);

			glVertex3f(avMax.x,avMax.y,avMax.z);
			glVertex3f(avMax.x,avMin.y,avMax.z);

			glVertex3f(avMin.x,avMax.y,avMax.z);
			glVertex3f(avMin.x,avMin.y,avMax.z);

			glVertex3f(avMin.x,avMin.y,avMax.z);
			glVertex3f(avMax.x,avMin.y,avMax.z);

			//Neg Z Quad
			glVertex3f(avMax.x,avMax.y,avMin.z);
			glVertex3f(avMin.x,avMax.y,avMin.z);

			glVertex3f(avMax.x,avMax.y,avMin.z);
			glVertex3f(avMax.x,avMin.y,avMin.z);

			glVertex3f(avMin.x,avMax.y,avMin.z);
			glVertex3f(avMin.x,avMin.y,avMin.z);

			glVertex3f(avMin.x,avMin.y,avMin.z);
			glVertex3f(avMax.x,avMin.y,avMin.z);

			//Lines between
			glVertex3f(avMax.x,avMax.y,avMax.z);
			glVertex3f(avMax.x,avMax.y,avMin.z);

			glVertex3f(avMin.x,avMax.y,avMax.z);
			glVertex3f(avMin.x,avMax.y,avMin.z);

			glVertex3f(avMin.x,avMin.y,avMax.z);
			glVertex3f(avMin.x,avMin.y,avMin.z);

			glVertex3f(avMax.x,avMin.y,avMax.z);
			glVertex3f(avMax.x,avMin.y,avMin.z);
		}
		glEnd();

	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawSphere(const cVector3f& avPos, float afRadius, cColor aCol)
	{
		int alSegments = 32;
		float afAngleStep = k2Pif /(float)alSegments;

		SetTexture(0,NULL);
		SetBlendActive(false);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINES);
		{
			//X Circle:
			for(float a=0; a< k2Pif; a+= afAngleStep)
			{
				glVertex3f(avPos.x, avPos.y + sin(a)*afRadius,
									avPos.z + cos(a)*afRadius);

				glVertex3f(avPos.x, avPos.y + sin(a+afAngleStep)*afRadius,
									avPos.z + cos(a+afAngleStep)*afRadius);
			}

			//Y Circle:
			for(float a=0; a< k2Pif; a+= afAngleStep)
			{
				glVertex3f(avPos.x + cos(a)*afRadius, avPos.y,
									avPos.z + sin(a)*afRadius);

				glVertex3f(avPos.x + cos(a+afAngleStep)*afRadius, avPos.y ,
							avPos.z+ sin(a+afAngleStep)*afRadius);
			}

			//Z Circle:
			for(float a=0; a< k2Pif; a+= afAngleStep)
			{
				glVertex3f(avPos.x + cos(a)*afRadius, avPos.y + sin(a)*afRadius, avPos.z);

				glVertex3f(avPos.x + cos(a+afAngleStep)*afRadius,
							avPos.y + sin(a+afAngleStep)*afRadius,
							avPos.z);
			}

		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawLine2D(const cVector2f& avBegin, const cVector2f& avEnd, float afZ, cColor aCol)
	{
		SetTexture(0,NULL);
		SetBlendActive(false);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINES);
		{
			glVertex3f(avBegin.x,avBegin.y,afZ);
			glVertex3f(avEnd.x,avEnd.y,afZ);
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawLineRect2D(const cRect2f& aRect, float afZ, cColor aCol)
	{
		SetTexture(0, NULL);
		SetBlendActive(false);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINE_STRIP);
		{
			glVertex3f(aRect.x,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y+aRect.h,afZ);
			glVertex3f(aRect.x,aRect.y+aRect.h,afZ);
			glVertex3f(aRect.x,aRect.y,afZ);
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawFilledRect2D(const cRect2f& aRect, float afZ, cColor aCol)
	{
		SetTexture(0, NULL);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_QUADS);
		{
			glVertex3f(aRect.x,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y+aRect.h,afZ);
			glVertex3f(aRect.x,aRect.y+aRect.h,afZ);
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	/**
	 * \todo Implement this.
	 * \param avCenter
	 * \param afRadius
	 * \param afZ
	 * \param aCol
	 */
	void cLowLevelGraphicsSDL::DrawLineCircle2D(const cVector2f& avCenter, float afRadius, float afZ, cColor aCol)
	{
		//Implement later
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::CopyContextToTexure(iTexture* apTex, const cVector2l &avPos,
							const cVector2l &avSize, const cVector2l &avTexOffset)
	{
		if(apTex==NULL)return;

		int lScreenY = (mvScreenSize.y - avSize.y) - avPos.y;
		int lTexY = (apTex->GetHeight() - avSize.y) - avTexOffset.y;

		//Log("TExoffset: %d %d\n",avTexOffset.x,lTexY);
		//Log("ScreenOffset: %d %d (h: %d s: %d p: %d)\n",avPos.x,lScreenY,mvScreenSize.y,
		//												avSize.y,avPos.y);

		SetTexture(0, apTex);
		glCopyTexSubImage2D(GetGLTextureTargetEnum(apTex->GetTarget()),0,
							avTexOffset.x, lTexY,
							avPos.x, lScreenY, avSize.x, avSize.y);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetRenderTarget(iTexture* pTex)
	{
		if(pTex == mpRenderTarget)return;
		mpRenderTarget = pTex;
	}

	//-----------------------------------------------------------------------

	bool cLowLevelGraphicsSDL::RenderTargetHasZBuffer()
	{
		return true;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::FlushRenderTarget()
	{


		//Old OGL 1.1 Code:
		/*if(mpRenderTarget!=NULL)
		{
			SetTexture(0, mpRenderTarget);

			//Log("w: %d\n",mpRenderTarget->GetWidth());

			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0,
							mpRenderTarget->GetWidth(), mpRenderTarget->GetHeight(), 0);
		}*/
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

	void cLowLevelGraphicsSDL::SetUpBatchArrays()
	{
		//Set the arrays
		glVertexPointer(3,GL_FLOAT, sizeof(float)*mlBatchStride, mpVertexArray);
		glColorPointer(4,GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[3]);
		glNormalPointer(GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[10]);

		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(3,GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[7]);
		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(3,GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[7]);
		glClientActiveTexture(GL_TEXTURE2);
		glTexCoordPointer(3,GL_FLOAT,sizeof(float)*mlBatchStride, &mpVertexArray[7]);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetVtxBatchStates(tVtxBatchFlag aFlags)
	{
		if(aFlags & eVtxBatchFlag_Position)	glEnableClientState(GL_VERTEX_ARRAY );
		else glDisableClientState(GL_VERTEX_ARRAY );

		if(aFlags & eVtxBatchFlag_Color0) glEnableClientState(GL_COLOR_ARRAY );
		else glDisableClientState(GL_COLOR_ARRAY );

		if(aFlags & eVtxBatchFlag_Normal) glEnableClientState(GL_NORMAL_ARRAY );
		else glDisableClientState(GL_NORMAL_ARRAY );


		if(aFlags & eVtxBatchFlag_Texture0){
			glClientActiveTexture(GL_TEXTURE0);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
		}
		else {
			glClientActiveTexture(GL_TEXTURE0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}

		if(aFlags & eVtxBatchFlag_Texture1){
			glClientActiveTexture(GL_TEXTURE1);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
		}
		else {
			glClientActiveTexture(GL_TEXTURE1);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}

		if(aFlags & eVtxBatchFlag_Texture2){
			glClientActiveTexture(GL_TEXTURE2);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY );
		}
		else {
			glClientActiveTexture(GL_TEXTURE2);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		}


	}

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

	GLenum cLowLevelGraphicsSDL::GetGLTextureParamEnum(eTextureParam aType)
	{
		switch(aType)
		{
			case eTextureParam_ColorFunc:		return GL_COMBINE_RGB;
			case eTextureParam_AlphaFunc:		return GL_COMBINE_ALPHA;
			case eTextureParam_ColorSource0:	return GL_SOURCE0_RGB;
			case eTextureParam_ColorSource1:	return GL_SOURCE1_RGB;
			case eTextureParam_ColorSource2:	return GL_SOURCE2_RGB;
			case eTextureParam_AlphaSource0:	return GL_SOURCE0_ALPHA;
			case eTextureParam_AlphaSource1:	return GL_SOURCE1_ALPHA;
			case eTextureParam_AlphaSource2:	return GL_SOURCE2_ALPHA;
			case eTextureParam_ColorOp0:		return GL_OPERAND0_RGB;
			case eTextureParam_ColorOp1:		return GL_OPERAND1_RGB;
			case eTextureParam_ColorOp2:		return GL_OPERAND2_RGB;
			default: return 0;
		}
	}

	//-----------------------------------------------------------------------

	GLenum cLowLevelGraphicsSDL::GetGLTextureOpEnum(eTextureOp aType)
	{
		switch(aType)
		{
			case eTextureOp_Color:			return GL_SRC_COLOR;
			case eTextureOp_OneMinusColor:	return GL_ONE_MINUS_SRC_COLOR;
			case eTextureOp_Alpha:			return GL_SRC_ALPHA;
			case eTextureOp_OneMinusAlpha:	return GL_ONE_MINUS_SRC_ALPHA;
			default: return 0;
		}
	}

	//-----------------------------------------------------------------------

	GLenum cLowLevelGraphicsSDL::GetGLTextureSourceEnum(eTextureSource aType)
	{
		switch(aType)
		{
			case eTextureSource_Texture:	return GL_TEXTURE;
			case eTextureSource_Constant:	return GL_CONSTANT;
			case eTextureSource_Primary:	return GL_PRIMARY_COLOR;
			case eTextureSource_Previous:	return GL_PREVIOUS;
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

	GLenum cLowLevelGraphicsSDL::GetGLTextureFuncEnum(eTextureFunc aType)
	{
		switch(aType)
		{
			case eTextureFunc_Modulate:		return GL_MODULATE;
			case eTextureFunc_Replace:		return GL_REPLACE;
			case eTextureFunc_Add:			return GL_ADD;
			case eTextureFunc_Substract:	return GL_SUBTRACT;
			case eTextureFunc_AddSigned:	return GL_ADD_SIGNED;
			case eTextureFunc_Interpolate:	return GL_INTERPOLATE;
			case eTextureFunc_Dot3RGB:		return GL_DOT3_RGB;
			case eTextureFunc_Dot3RGBA:		return GL_DOT3_RGBA;
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

	void cLowLevelGraphicsSDL::InitCG()
	{
		mCG_Context = cgCreateContext();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ExitCG()
	{
		cgDestroyContext(mCG_Context);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetMatrixMode(eMatrix mType)
	{
		switch(mType)
		{
			case eMatrix_ModelView: glMatrixMode(GL_MODELVIEW);break;
			case eMatrix_Projection: glMatrixMode(GL_PROJECTION); break;
		}
	}

	//-----------------------------------------------------------------------

}
