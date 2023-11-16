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
#include "graphics/ogl2/OcclusionQueryOGL.h"
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

	cOcclusionQueryOGL::cOcclusionQueryOGL()
	{
		glGenQueries(1, (GLuint *)&mlQueryId);
		mlLastSampleCount =0;
	}

	//-----------------------------------------------------------------------

	cOcclusionQueryOGL::~cOcclusionQueryOGL()
	{
		glDeleteQueries(1, (GLuint *)&mlQueryId);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cOcclusionQueryOGL::Begin()
	{
		glBeginQuery(GL_SAMPLES_PASSED,mlQueryId);
	}

	void cOcclusionQueryOGL::End()
	{
		glEndQuery(GL_SAMPLES_PASSED);
	}

	bool cOcclusionQueryOGL::FetchResults()
	{
		int lAvailable=0;
		glGetQueryObjectiv(mlQueryId,GL_QUERY_RESULT_AVAILABLE,(GLint *)&lAvailable);
		if(lAvailable==0) return false;

		glGetQueryObjectiv(mlQueryId,GL_QUERY_RESULT,(GLint *)&mlLastSampleCount);
		return true;
	}

	unsigned int cOcclusionQueryOGL::GetSampleCount()
	{
		return mlLastSampleCount;
	}

	//-----------------------------------------------------------------------
}
