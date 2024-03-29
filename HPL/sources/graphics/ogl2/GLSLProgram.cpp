/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */
#include "graphics/ogl2/GLSLProgram.h"
#include "graphics/ogl2/SDLTexture.h"
#include "math/Math.h"
#include "system/String.h"
#include "system/Files.h"
#include "system/Log.h"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	cGLSLProgram::cGLSLProgram(tString asName)
	: iGpuProgram(asName)
	{
		mProgram = 0;
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

	GLuint LoadShaderFromFile(const tString& shaderPath, GLint type) {
		int sourceSize;
		auto source = LoadEntireFile(shaderPath, sourceSize);
		if (source == nullptr) {
			Log("Error loading shader: '%s'!\n", shaderPath.c_str());
			return 0;
		}
		auto shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, &sourceSize);
		int compileOK;
		glCompileShader(shader);
		delete[] source;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileOK);
		if (compileOK == GL_FALSE) {
			int logLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			char *infoLog = new char[logLength];
			glGetShaderInfoLog(shader, logLength, &logLength, infoLog);

			Log("Error compiling shader: %s\n", infoLog);
			delete[] infoLog;

			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

	bool cGLSLProgram::CreateFromFile(const tString& asVertexFile, const tString& asFragmentFile)
	{
		auto vtxShader = LoadShaderFromFile(asVertexFile, GL_VERTEX_SHADER);
		if (vtxShader == 0) {
			Log("Error loading vertex shader: '%s'!\n", asVertexFile.c_str());
			return false;
		}
		auto fragShader = LoadShaderFromFile(asFragmentFile, GL_FRAGMENT_SHADER);
		if (fragShader == 0) {
			Log("Error loading fragment shader: '%s'!\n", asFragmentFile.c_str());
			glDeleteShader(vtxShader);
			return false;
		}

		mProgram = glCreateProgram();
		glAttachShader(mProgram, vtxShader);
		glAttachShader(mProgram, fragShader);
		glLinkProgram(mProgram);

		glDetachShader(mProgram, vtxShader);
		glDetachShader(mProgram, fragShader);
		glDeleteShader(vtxShader);
		glDeleteShader(fragShader);

		int linkOK;
		glGetProgramiv(mProgram, GL_LINK_STATUS, &linkOK);
		if (linkOK == GL_FALSE) {
			int logLength;
			glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &logLength);
			char *infoLog = new char[logLength];
			glGetProgramInfoLog(mProgram, logLength, &logLength, infoLog);

			glDeleteProgram(mProgram);
			mProgram = 0;

			Log("Error linking program: %s\n", infoLog);
			delete[] infoLog;
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	void cGLSLProgram::Bind()
	{
		glUseProgram(mProgram);
	}

	//-----------------------------------------------------------------------

	void cGLSLProgram::UnBind()
	{
		glUseProgram(0);
	}

	//-----------------------------------------------------------------------

	bool cGLSLProgram::SetFloat(const tString& asName, float afX)
	{
		auto loc = glGetUniformLocation(mProgram, asName.c_str());
		if (loc < 0) return false;
		glUniform1f(loc, afX);
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cGLSLProgram::SetVec2f(const tString& asName, float afX, float afY)
	{
		auto loc = glGetUniformLocation(mProgram, asName.c_str());
		if (loc < 0) return false;
		glUniform2f(loc, afX, afY);
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cGLSLProgram::SetVec3f(const tString& asName, float afX,float afY,float afZ)
	{
		auto loc = glGetUniformLocation(mProgram, asName.c_str());
		if (loc < 0) return false;
		glUniform3f(loc, afX, afY, afZ);
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cGLSLProgram::SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW)
	{
		auto loc = glGetUniformLocation(mProgram, asName.c_str());
		if (loc < 0) {
			Warning("Could not get vec4 uniform location for %s!", asName.c_str());
			return false;
		}
		glUniform4f(loc, afX, afY, afZ, afW);
		return true;
	}

	//-----------------------------------------------------------------------

	bool cGLSLProgram::SetMatrixf(const tString& asName, const cMatrixf& mMtx)
	{
		auto loc = glGetUniformLocation(mProgram, asName.c_str());
		if (loc < 0) {
			Warning("Could not get matrix uniform location for %s!\n", asName.c_str());
			return false;
		}

		// transpose matrix for GL
		glUniformMatrix4fv(loc, 1, GL_TRUE, &mMtx.v[0]);
		return true;
	}

	//-----------------------------------------------------------------------

	bool cGLSLProgram::SetTextureBindingIndex(const tString& asName, int index)
	{
		auto loc = glGetUniformLocation(mProgram, asName.c_str());
		if (loc < 0) {
			Warning("Could not get texture uniform location for %s!\n", asName.c_str());
			return false;
		}

		glUniform1i(loc, index);
		return true;
	}

}
