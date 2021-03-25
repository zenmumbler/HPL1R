/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#include "impl/GLSLProgram.h"
#include "impl/SDLTexture.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"

#define GL_GLEXT_LEGACY
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	cGLSLProgram::cGLSLProgram(tString asName, eGpuProgramType aType)
	: iGpuProgram(asName, aType)
	{
		mProgram = 0;

		if(mProgramType == eGpuProgramType_Vertex)
		{
		}
		else
		{
		}

		if(mbDebugInfo)
		{
		}

		for(int i=0; i< MAX_TEXTUREUNITS; ++i)
		{
			// mvTexUnitParam[i] = NULL;
		}
	}

	cGLSLProgram::~cGLSLProgram()
	{
		if (mProgram) {
			glDeleteProgram(mProgram);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGLSLProgram::Destroy()
	{
	}

	//-----------------------------------------------------------------------

	bool cGLSLProgram::CreateFromFile(const tString &asFile, const tString &asEntry)
	{
		mProgram = glCreateProgram();

		if (mProgram == 0) {
			Log("Error loading: '%s'!\n", asFile.c_str());
			return false;
		}

		msFile = asFile;
		msEntry = asEntry;

		///////////////////////////////
		//Look for texture units.

		return true;
	}

	//-----------------------------------------------------------------------

	void cGLSLProgram::Bind()
	{
	}

	//-----------------------------------------------------------------------

	void cGLSLProgram::UnBind()
	{
	}

	//-----------------------------------------------------------------------

	bool  cGLSLProgram::SetFloat(const tString& asName, float afX)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cGLSLProgram::SetVec2f(const tString& asName, float afX,float afY)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cGLSLProgram::SetVec3f(const tString& asName, float afX,float afY,float afZ)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cGLSLProgram::SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cGLSLProgram::SetMatrixf(const tString& asName, const cMatrixf& mMtx)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cGLSLProgram::SetMatrixf(const tString& asName, eGpuProgramMatrix mType,
		eGpuProgramMatrixOp mOp)
	{
//		CGparameter Param = GetParam(asName, CG_FLOAT4x4);
//		if(Param==NULL)return false;
//
//		CGGLenum MtxType;
//		CGGLenum OpType;
//		switch(mType){
//			case eGpuProgramMatrix_View:				MtxType=CG_GL_MODELVIEW_MATRIX;break;
//			case eGpuProgramMatrix_Projection:		MtxType=CG_GL_PROJECTION_MATRIX;break;
//			case eGpuProgramMatrix_Texture:			MtxType=CG_GL_TEXTURE_MATRIX;break;
//			case eGpuProgramMatrix_ViewProjection:	MtxType=CG_GL_MODELVIEW_PROJECTION_MATRIX;break;
//		}
//		switch(mOp){
//			case eGpuProgramMatrixOp_Identity:		OpType=CG_GL_MATRIX_IDENTITY; break;
//			case eGpuProgramMatrixOp_Inverse:		OpType=CG_GL_MATRIX_INVERSE; break;
//			case eGpuProgramMatrixOp_Transpose:		OpType=CG_GL_MATRIX_TRANSPOSE; break;
//			case eGpuProgramMatrixOp_InverseTranspose:OpType=CG_GL_MATRIX_INVERSE_TRANSPOSE; break;
//		}
//
//		cgGLSetStateMatrixParameter(Param,MtxType, OpType);
		return true;
	}

	//-----------------------------------------------------------------------

	bool cGLSLProgram::SetTexture(const tString& asName, iTexture* apTexture, bool abAutoDisable)
	{
		auto textureType = TextureTargetToGL(apTexture->GetTarget());

		if(apTexture)
		{
//			CGparameter Param = GetParam(asName, textureType);
//			if(Param==NULL)return false;

			cSDLTexture* pSDLTex = static_cast<cSDLTexture*>(apTexture);

			//Log("Intializing Tex %s(%d): %d\n",cgGetParameterName(Param),
			//		Param,pSDLTex->GetTextureHandle());
//			cgGLSetTextureParameter(Param, pSDLTex->GetTextureHandle());
//			cgGLEnableTextureParameter(Param);
		}
		else
		{
//			CGparameter Param = GetParam(asName, CG_SAMPLER2D);
//			if(Param==NULL)return false;
//
//			cgGLDisableTextureParameter(Param);
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cGLSLProgram::SetTextureToUnit(int alUnit, iTexture* apTexture)
	{
		// if(mvTexUnitParam[alUnit]==NULL || alUnit >= MAX_TEXTUREUNITS) return false;

		cSDLTexture* pSDLTex = static_cast<cSDLTexture*>(apTexture);

		if(apTexture)
		{
			// cgGLSetTextureParameter(mvTexUnitParam[alUnit], pSDLTex->GetTextureHandle());
			// cgGLEnableTextureParameter(mvTexUnitParam[alUnit]);
		}
		else
		{
			// cgGLDisableTextureParameter(mvTexUnitParam[alUnit]);
		}

		return true;
	}

}
