/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_GLSLPROGRAM_H
#define HPL_GLSLPROGRAM_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUProgram.h"

namespace hpl {

	class cGLSLProgram : public iGpuProgram
	{
	public:
		cGLSLProgram(tString asName);
		~cGLSLProgram();

		void Destroy();

		tString GetProgramName(){ return msName; }

		bool CreateFromFile(const tString& asFile, const tString& asEntry="main");

		void Bind();
		void UnBind();

		bool SetFloat(const tString& asName, float afX);
		bool SetVec2f(const tString& asName, float afX,float afY);
		bool SetVec3f(const tString& asName, float afX,float afY,float afZ);
		bool SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW);

		bool SetMatrixf(const tString& asName, const cMatrixf& mMtx);
		bool SetMatrixIdentityf(const tString& asName, eGpuProgramMatrix mType);

		bool SetTexture(const tString& asName,iTexture* apTexture);

	protected:
		unsigned int mProgram;
		tString msName;
	};
};
#endif // HPL_GLSLPROGRAM_H
