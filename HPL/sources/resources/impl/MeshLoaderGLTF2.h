/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_MESH_LOADER_GLTF2_H
#define HPL_MESH_LOADER_GLTF2_H

#include "resources/MeshLoader.h"

namespace hpl {

	class cMesh;

	class cMeshLoaderGLTF2 : public iMeshLoader
	{
	public:
		cMeshLoaderGLTF2(iLowLevelGraphics *apLowLevelGraphics);

		cMesh* LoadMesh(const tString& asFile, tMeshLoadFlag aFlags);
		bool SaveMesh(cMesh* apMesh,const tString& asFile) { return false; }

		cWorld3D* LoadWorld(const tString& asFile, cScene* apScene,tWorldLoadFlag aFlags) { return NULL; }
		cAnimation* LoadAnimation(const tString& asFile) { return NULL; }

		bool IsSupported(const tString asFileType);
		void AddSupportedTypes(tStringVec* avFileTypes);
	};

};

#endif // HPL_MESH_LOADER_GLTF2_H
