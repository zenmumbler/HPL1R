//  ModelExport.cpp

#include <string>
#include "jmcpp/json.hpp"
#include "ModelExport.h"

using json = nlohmann::json;

/*
static void WriteSubMesh() {

}
*/

static void ExportModel(cMesh *model) {
	json gltf;
	json asset;
	asset["version"] = "2.0";
	asset["generator"] = "HPL1R Model Exporter - by zenmumbler";
	asset["copyright"] = "(c) 2006 Frictional Games";
	gltf["asset"] = asset;
//	gltf["extensionsUsed"].push_back("KHR_materials_pbrSpecularGlossiness");
//	gltf["extensionsRequired"].push_back("KHR_materials_pbrSpecularGlossiness");
	
	auto gltfFileName = cString::SetFileExt(model->GetName(), "gltf");
	auto gltfFilePath = cString::SetFileExt(FileSearcher::GetFilePath(model->GetName()), "gltf");
	auto binFileName = cString::SetFileExt(model->GetName(), "bin");
	auto binFilePath = cString::SetFileExt(FileSearcher::GetFilePath(model->GetName()), "bin");

	Log("gltf path: %s\n", gltfFilePath.c_str());
	Log("bin path: %s\n", binFilePath.c_str());

	gltf["scene"] = 0;
	json scene;
	scene["name"] = model->GetName();
	scene["nodes"].push_back(0);
	gltf["scenes"].push_back(scene);
	
	// Vertex Data
	auto mesh = model->GetSubMesh(0);
	json meshNode;
	meshNode["name"] = mesh->GetName();
	auto vb = mesh->GetVertexBuffer();

	// setup gltf material containers
	json images;
	json textures;
	json materials;
	json sampler;
	sampler["magFilter"] = 0x2601; // LINEAR
	sampler["minFilter"] = 0x2703; // LINEAR_MIPMAP_LINEAR
	sampler["wrapS"] = 0x2901; // REPEAT
	sampler["wrapT"] = 0x2901; // REPEAT
	gltf["samplers"].push_back(sampler);

	// Material
	auto mat = mesh->GetMaterial();
	auto texDiff = mat->GetTexture(eMaterialTexture_Diffuse);

	// -- add image and texture nodes
	json imageNode;
	imageNode["uri"] = texDiff->GetName();
	images.push_back(imageNode);
	json textureNode;
	textureNode["source"] = 0;
	textureNode["sampler"] = 0;
	textures.push_back(textureNode);

	// -- add material mapped as specular gloss
	json matNode;
	matNode["name"] = mat->GetName();
	matNode["doubleSided"] = true; // for now, easies debug viewing
//	json specGloss;
//	specGloss["glossinessFactor"] = 0.0f;
//	specGloss["diffuseTexture"]["index"] = 0;
//	matNode["extensions"]["KHR_materials_pbrSpecularGlossiness"] = specGloss;
	json roughMetal;
	roughMetal["baseColorTexture"]["index"] = 0;
	roughMetal["metallicFactor"] = 0.0;
	roughMetal["roughnessFactor"] = 0.9;
	matNode["pbrMetallicRoughness"] = roughMetal;
	
//					"baseColorFactor": [ 1.000, 0.766, 0.336, 1.0 ],
//					"metallicFactor": 1.0,
//					"roughnessFactor": 0.0
	materials.push_back(matNode);
	
	// add all top-levels
	gltf["images"] = images;
	gltf["textures"] = textures;
	gltf["materials"] = materials;
	
	// Vertex and Index Data
	json buffers;
	json bufferViews;
	json accessors;
	json primitive;
	json attributes;
	int bufferViewIndex = 0;
	int attrIndex = 0;
	int bufferOffset = 0;
	int attrSize = 0;
	
	const auto addBufferView = [&bufferOffset, &bufferViews, &bufferViewIndex](int len, int target) {
		json view;
		view["buffer"] = 0;
		view["byteOffset"] = bufferOffset;
		view["byteLength"] = len;
		view["target"] = target;
		bufferViews.push_back(view);
		bufferOffset += len;
		bufferViewIndex += 1;
	};
	
	const auto addAccessor = [&accessors](int view, int offset, int ctype, int count, const tString& type) {
		json acc;
		acc["bufferView"] = view;
		acc["byteOffset"] = offset;
		acc["componentType"] = ctype;
		acc["count"] = count;
		acc["type"] = type;
		accessors.push_back(acc);
	};

	// indices
	int indicesSizeBytes = vb->GetIndexCount() * sizeof(int);
	addBufferView(indicesSizeBytes, 34963); // ELEMENT_ARRAY_BUFFER
	addAccessor(0, 0, 0x1405, vb->GetIndexCount(), "SCALAR"); // UINT
	primitive["indices"] = attrIndex++;

	const auto addAttr = [&attrIndex, &bufferViewIndex, &attrSize, &attributes, vb, &addBufferView, &addAccessor](VertexAttr vertAttr, const tString& name) {
		json attr;
		auto arr = vb->GetArray(vertAttr);
		if (arr == NULL) {
			return;
		}
		int count = vb->GetVertexCount();
		int elements = vertAttr == VertexAttr_UV0 ? 2 : 3;
		int arrSizeBytes = sizeof(float) * count * elements;

		addAccessor(bufferViewIndex, 0, 0x1406, count, tString("VEC") + std::to_string(elements)); // FLOAT
		addBufferView(arrSizeBytes, 34962); // ARRAY_BUFFER

		attributes[name] = attrIndex++;
		attrSize += arrSizeBytes;
	};
	
	addAttr(VertexAttr_Normal, "NORMAL");
	addAttr(VertexAttr_Position, "POSITION");
//	addAttr(VertexAttr_Color0, "COLOR_0");
	addAttr(VertexAttr_UV0, "TEXCOORD_0");
//	addAttr(VertexAttr_Tangent, "TANGENT");
	primitive["attributes"] = attributes;

	// write buffer nodes
	json buffer;
	buffer["byteLength"] = bufferOffset;
	buffer["uri"] = binFileName;
	buffers.push_back(buffer);
	gltf["buffers"] = buffers;
	gltf["bufferViews"] = bufferViews;
	gltf["accessors"] = accessors;

	// write binary data
	float * const binData = new float[bufferOffset >> 2];
	float *nextBufferPos = binData;
	
	// write indices
	int triCount = vb->GetIndexCount() / 3;
	unsigned int *indexBufferPos = reinterpret_cast<unsigned int*>(nextBufferPos);
	unsigned int *indexData = vb->GetIndices();
//	memcpy(nextBufferPos, vb->GetIndices(), indicesSizeBytes);
	for (int tri = 0; tri < triCount; ++tri) {
		*indexBufferPos++ = *indexData++;
		unsigned int vB = *indexData++;
		unsigned int vC = *indexData++;
		*indexBufferPos++ = vC;
		*indexBufferPos++ = vB;
	}
	nextBufferPos += vb->GetIndexCount();

	const auto writeAttr = [vb, &nextBufferPos](VertexAttr vertAttr) {
		auto arr = vb->GetArray(vertAttr);
		if (arr == NULL) {
			return;
		}
		int count = vb->GetVertexCount();
		int arrElements = vb->GetArrayStride(vertAttr);
		int bufElements = vertAttr == VertexAttr_UV0 ? 2 : 3;

		if (arrElements == bufElements) {
			int sizeBytes = sizeof(float) * count * arrElements;
			memcpy(nextBufferPos, arr, sizeBytes);
			nextBufferPos += count * bufElements;
		}
		else {
			int gap = arrElements - bufElements;
			Log("gap is %d\n", gap);
			for (int v=0; v < count; ++v) {
				if (bufElements >= 2) {
					*nextBufferPos++ = *arr++;
					*nextBufferPos++ = *arr++;
				}
				if (bufElements == 3) {
					*nextBufferPos++ = *arr++;
				}
				arr += gap;
			}
		}
		
	};
	writeAttr(VertexAttr_Normal);
	writeAttr(VertexAttr_Position);
//	writeAttr(VertexAttr_Color0);
	writeAttr(VertexAttr_UV0);

	
	FILE *pFile = fopen(binFilePath.c_str(),"wb+");
	fwrite(binData, bufferOffset, 1, pFile);
	fclose(pFile);
	delete[] binData;

	// write mesh node
	primitive["material"] = 0;
	primitive["mode"] = 4; // always triangles for DAE model imports
	meshNode["primitives"].push_back(primitive);
	gltf["meshes"].push_back(meshNode);

	// singular node
	json node;
	node["mesh"] = 0;
	gltf["nodes"].push_back(node);

	// write out gltf
	pFile = fopen(gltfFilePath.c_str(),"w+");
	const auto gltfString = gltf.dump();
	fwrite(gltfString.data(), gltfString.length(), 1, pFile);
	fclose(pFile);
}

void ExportModels(cGame *apGame) {
	Log("--------------------------------------------------------\n");
	Log("Start Model Export!\n");
	const auto res = apGame->GetResources();
	const auto mm = res->GetMeshManager();
	
	const auto model = mm->CreateMesh("ice_cave_hand_whole.dae");
	if (model) {
		ExportModel(model);
		mm->Destroy(model);
	}
	else {
		Log("Failed to create even a lowly soup can...\n");
	}
	Log("--------------------------------------------------------\n");
}
