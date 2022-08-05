/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */

#include "resources/impl/MeshLoaderGLTF2.h"

#include "system/Log.h"
#include "system/Files.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/Material.h"
#include "resources/MaterialManager.h"
#include "math/Math.h"

#include "cgltf/cgltf.h"

namespace hpl {

	cMeshLoaderGLTF2::cMeshLoaderGLTF2(iLowLevelGraphics *apLowLevelGraphics)
	: iMeshLoader(apLowLevelGraphics)
	{}


	cMesh* cMeshLoaderGLTF2::LoadMesh(const tString& asFile, tMeshLoadFlag aFlags) {
		cMesh* pMesh = new cMesh(cString::GetFileName(asFile), mpMaterialManager, mpAnimationManager);

		/////////////////////////////////////////////////
		// LOAD THE DOCUMENT
		cgltf_options readOptions {};
		cgltf_data *model = nullptr;
		auto loadResult = cgltf_parse_file(&readOptions, asFile.c_str(), &model);
		
		if (loadResult != cgltf_result_success) {
			Error("Could not load gltf file `%s`", asFile.c_str());
			delete pMesh;
			return nullptr;
		}
		
		// only support 1 buffer (file) for now
		auto bufferSize = model->buffers->size;
		uint8_t *bufferData = new uint8_t[bufferSize];
		if (model->buffers->data == nullptr) {
			auto binFilePath = cString::SetFilePath(model->buffers->uri, cString::GetFilePath(asFile));
			FileReader reader { binFilePath };
			reader.readBytes(bufferData, bufferSize);
			Log("gltf2: loaded buffer data from external file %s", model->buffers->uri);
		}
		else {
			memcpy(bufferData, model->buffers->data, bufferSize);
			Log("gltf2: loaded buffer data glb file");
		}

		/////////////////////////////////////////////////
		// LOAD SUBMESHES

		//Iterate through the sub meshes
		
		auto curMesh = model->meshes;
		auto curPrim = curMesh->primitives;
		tString meshName = curMesh->name;
		if (meshName.length() == 0) {
			meshName = "mesh";
		}

		for (size_t primIx = 0; primIx < curMesh->primitives_count; ++primIx) {
			//////////////////
			//Create sub mesh
			cSubMesh *pSubMesh = pMesh->CreateSubMesh(meshName + "_" + std::to_string(primIx));

			//////////////////
			//Set material
			iMaterial *pMaterial = mpMaterialManager->CreateMaterial(curPrim->material->name);
			pSubMesh->SetMaterial(pMaterial);

			////////////////////
			//Get the vertices
			tVertexFlag vtxFlags = 0;
			int lVtxSize = static_cast<int>(curPrim->attributes->data->count);

			//Check what type of vertices are included.
			auto curAttr = curPrim->attributes;
			for (size_t attrIx = 0; attrIx < curPrim->attributes_count; ++attrIx) {
				if (curAttr->type == cgltf_attribute_type_normal) vtxFlags |= eVertexFlag_Normal;
				if (curAttr->type == cgltf_attribute_type_position) vtxFlags |= eVertexFlag_Position;
				if (curAttr->type == cgltf_attribute_type_texcoord) vtxFlags |= eVertexFlag_Texture0;
				if (curAttr->type == cgltf_attribute_type_color) vtxFlags |= eVertexFlag_Color0;
				if (curAttr->type == cgltf_attribute_type_tangent) vtxFlags |= eVertexFlag_Texture1;
				curAttr++;
			}

			//Create the vertex buffer
			iVertexBuffer* pVtxBuff = mpLowLevelGraphics->CreateVertexBuffer(vtxFlags,
							eVertexBufferDrawType_Tri,
							eVertexBufferUsageType_Static,
							0, 0);

			pVtxBuff->SetTangents((vtxFlags & eVertexFlag_Texture1) != 0);

			//Fill the arrays
			for (int i = 0; i < klNumOfVertexFlags; i++) {
				if (kvVertexFlags[i] & vtxFlags) {
					int lElemPerVtx = 3;
					if (kvVertexFlags[i] & eVertexFlag_Texture1 || kvVertexFlags[i] & eVertexFlag_Color0) {
						lElemPerVtx = 4;
					}

					/*
					TiXmlElement* pElem = pVtxElem->FirstChildElement(GetVertexName(kvVertexFlags[i]));

					pVtxBuff->ResizeArray(kvVertexFlags[i], lVtxSize * lElemPerVtx);
					float *pArray = pVtxBuff->GetArray(kvVertexFlags[i]);

					FillVtxArray(pArray, pElem->Attribute("data"), lVtxSize * lElemPerVtx);
					*/
				}
			}


			++curPrim;
		}

		/*
		TiXmlElement* pSubMeshElem = pSubMeshesRootElem->FirstChildElement();
		while(pSubMeshElem)
		{
			//////////////////
			//Create sub mesh
			cSubMesh *pSubMesh = pMesh->CreateSubMesh(pSubMeshElem->Attribute("name"));

			//////////////////
			//Set material
			const char *pMatName = pSubMeshElem->Attribute("material");
			if(pMatName==NULL){
				Error("No material found for mesh '%s'\n",asFile.c_str());
				ExitLoad();
			}

			iMaterial *pMaterial = mpMaterialManager->CreateMaterial(pMatName);
			pSubMesh->SetMaterial(pMaterial);


			////////////////////
			//Get the vertices
			TiXmlElement* pVtxElem = pSubMeshElem->FirstChildElement("Vertices");
			int lVtxSize = cString::ToInt(pVtxElem->Attribute("size"),0);
			tVertexFlag vtxFlags =0;
			bool bTangents = false;

			//Check what type of vertices are included.
			if(pVtxElem->FirstChild("Normal"))vtxFlags |= eVertexFlag_Normal;
			if(pVtxElem->FirstChild("Position"))vtxFlags |= eVertexFlag_Position;
			if(pVtxElem->FirstChild("Texture"))vtxFlags |= eVertexFlag_Texture0;
			if(pVtxElem->FirstChild("Color"))vtxFlags |= eVertexFlag_Color0;
			if(pVtxElem->FirstChild("Tangent")){
				vtxFlags |= eVertexFlag_Texture1;
				bTangents = true;
			}

			//Create the vertex buffer
			eVertexBufferUsageType usageType = eVertexBufferUsageType_Static;
			iVertexBuffer* pVtxBuff = mpLowLevelGraphics->CreateVertexBuffer(vtxFlags,
							eVertexBufferDrawType_Tri,
							usageType,
							0,0);

			pVtxBuff->SetTangents(bTangents);

			//Fill the arrays
			for(int i=0;i<klNumOfVertexFlags;i++)
			{
				if(kvVertexFlags[i] & vtxFlags)
				{
					int lElemPerVtx = 3;
					if(kvVertexFlags[i] & eVertexFlag_Texture1 || kvVertexFlags[i] & eVertexFlag_Color0){
						lElemPerVtx = 4;
					}

					TiXmlElement* pElem = pVtxElem->FirstChildElement(GetVertexName(kvVertexFlags[i]));

					pVtxBuff->ResizeArray(kvVertexFlags[i],lVtxSize * lElemPerVtx);
					float *pArray = pVtxBuff->GetArray(kvVertexFlags[i]);

					//Log("TYPE: %s:\n",GetVertexName(kvVertexFlags[i]));
					FillVtxArray(pArray, pElem->Attribute("data"),lVtxSize * lElemPerVtx);
				}
			}

			////////////////////
			//Get Indices

			TiXmlElement* pIdxElem = pSubMeshElem->FirstChildElement("Indices");
			int lIdxSize = cString::ToInt(pIdxElem->Attribute("size"),0);

			//Log("TYPE: Indices\n");
			pVtxBuff->ResizeIndices(lIdxSize);
			FillIdxArray(pVtxBuff->GetIndices(), pIdxElem->Attribute("data"),lIdxSize);

			///////////////////
			//Compile vertex buffer
			pVtxBuff->Compile(0);

			pSubMesh->SetVertexBuffer(pVtxBuff);


			/////////////////
			//Next element
			pSubMeshElem = pSubMeshesRootElem->NextSiblingElement();
		}

		*/

		delete[] bufferData;
		cgltf_free(model);

		return pMesh;
	}


	bool cMeshLoaderGLTF2::IsSupported(const tString asFileType) {
		if (asFileType == "gltf") return true;
		if (asFileType == "glb") return true;
		return false;
	}


	void cMeshLoaderGLTF2::AddSupportedTypes(tStringVec* avFileTypes) {
		avFileTypes->push_back("gltf");
		avFileTypes->push_back("glb");
	}

}
