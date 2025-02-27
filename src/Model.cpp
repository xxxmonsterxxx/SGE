#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "SGE.h"	

#include "tiny_obj_loader.h"

#include <filesystem>


void ModelMesh::generateInstanceData(GameObject* go, void* data)
{
	MeshInstanceData newData{};
	newData.model = go->getModel();
	*(MeshInstanceData*)data = newData;
}


bool Model::loadGLTFData(std::string path)
{
	return false;
}

bool Model::loadObjData(std::string path)
{
	std::string fullPath = SGE::getResourcesPath() + path;
	std::filesystem::path fsPath(path);
	std::string modelName = fsPath.filename().string();

	std::string objPath = fullPath + "/" + modelName + ".obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str(), fullPath.c_str())) {
		return false;
	}

	bool noMaterials = materials.empty();
	if (noMaterials) {
		
	}

	std::vector<int> addedTextures;
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			ObjModelVertex v{};
			v.vertex.x = attrib.vertices[(size_t)3 * index.vertex_index + 0];
			v.vertex.y = attrib.vertices[(size_t)3 * index.vertex_index + 1];
			v.vertex.z = attrib.vertices[(size_t)3 * index.vertex_index + 2];

			/*auto it = std::find(verts.begin(), verts.end(), v);
			if (it != verts.end()) {
				indices.push_back(std::distance(verts.begin(), it));
				continue;
			}*/

			v.texCoord = { attrib.texcoords[(size_t)2 * index.texcoord_index + 0],
								 1.0f - attrib.texcoords[(size_t)2 * index.texcoord_index + 1] };

			v.texInd = 0;
			v.color[3] = 1;
			if (!noMaterials) {
				int requiredTexture = shape.mesh.material_ids[0];

				v.color[3] = materials[requiredTexture].dissolve;
				if (materials[requiredTexture].diffuse_texname.empty()) {
					v.texInd = -1;
					v.color[0] = materials[requiredTexture].diffuse[0];
					v.color[1] = materials[requiredTexture].diffuse[1];
					v.color[2] = materials[requiredTexture].diffuse[2];
				} else {
					bool added = std::find(addedTextures.begin(), addedTextures.end(), requiredTexture) != addedTextures.end();

					if (!added) {
						textures.push_back(path + "/" + materials[requiredTexture].diffuse_texname);
						addedTextures.push_back(requiredTexture);
						v.texInd = (int)(textures.size()) - 1;
					} else {
						auto it = std::find(addedTextures.begin(), addedTextures.end(), requiredTexture);
						v.texInd = (int)std::distance(addedTextures.begin(), it);
						if (v.texInd < 0 || v.texInd > addedTextures.size() - 1) {
							return false;
						}
					}
				}
			}

			verts.push_back(v);
			indices.push_back((uint32_t)(verts.size()) - 1);
		}
	}

	if (verts.empty())
		return false;

	VkVertexInputBindingDescription vBindingD{};
	vBindingD.binding = 0;
	vBindingD.stride = sizeof(ObjModelVertex);
	vBindingD.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescription.push_back(vBindingD);

	VkVertexInputAttributeDescription posD{};
	posD.binding = 0;
	posD.location = 0;
	posD.format = VK_FORMAT_R32G32B32_SFLOAT;
	posD.offset = offsetof(ObjModelVertex, vertex);
	attributeDescription.push_back(posD);

	VkVertexInputAttributeDescription colorD{};
	colorD.binding = 0;
	colorD.location = 3;
	colorD.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	colorD.offset = offsetof(ObjModelVertex, color);
	attributeDescription.push_back(colorD);

	VkVertexInputAttributeDescription texIndD{};
	texIndD.binding = 0;
	texIndD.location = 1;
	texIndD.format = VK_FORMAT_R32_SINT;
	texIndD.offset = offsetof(ObjModelVertex, texInd);
	attributeDescription.push_back(texIndD);

	VkVertexInputAttributeDescription texCoordD{};
	texCoordD.binding = 0;
	texCoordD.location = 2;
	texCoordD.format = VK_FORMAT_R32G32_SFLOAT;
	texCoordD.offset = offsetof(ObjModelVertex, texCoord);
	attributeDescription.push_back(texCoordD);

	VkDescriptorSetLayoutBinding uboB{};
	uboB.binding = 0;
	uboB.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboB.descriptorCount = 1;
	uboB.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboB.pImmutableSamplers = nullptr;
	layoutBindings.push_back(uboB);

	VkDescriptorSetLayoutBinding instanceUboB{};
	instanceUboB.binding = 1;
	instanceUboB.descriptorCount = 1;
	instanceUboB.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	instanceUboB.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	instanceUboB.pImmutableSamplers = nullptr;
	layoutBindings.push_back(instanceUboB);

	VkDescriptorSetLayoutBinding samplerB{};
	samplerB.binding = 2;
	samplerB.descriptorCount = textures.size();
	samplerB.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerB.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerB.pImmutableSamplers = nullptr;
	layoutBindings.push_back(samplerB);

	return true;
}

void Model::normalize()
{
	SgrVertex centroid{ 0,0,0 };
	glm::vec3 maxCoord{verts[0].vertex.x,verts[0].vertex.y,verts[0].vertex.z};
	glm::vec3 minCoord{verts[0].vertex.x,verts[0].vertex.y,verts[0].vertex.z};
	for (const auto& v : verts) {
		centroid += v.vertex;

		if (v.vertex.x > maxCoord.x) maxCoord.x = v.vertex.x;
		if (v.vertex.y > maxCoord.y) maxCoord.y = v.vertex.y;
		if (v.vertex.z > maxCoord.z) maxCoord.z = v.vertex.z;

		if (v.vertex.x < minCoord.x) minCoord.x = v.vertex.x;
		if (v.vertex.y < minCoord.y) minCoord.y = v.vertex.y;
		if (v.vertex.z < minCoord.z) minCoord.z = v.vertex.z;
	}

	float maxDistance = maxCoord.x - minCoord.x;
	if ((maxCoord.y - minCoord.y) > maxDistance) maxDistance = maxCoord.y - minCoord.y;
	if ((maxCoord.z - minCoord.z) > maxDistance) maxDistance = maxCoord.z - minCoord.z;

	for (auto& v : verts) {
		v.vertex -= (centroid / (float)verts.size());
		v.vertex /= maxDistance / 2;
	}
}

Model::Model(std::string name, std::string modelPath, ModelType type)
{
	switch (type) {
		case OBJ:
			loadObjData(modelPath);
			break;
		case GLTF:
			loadGLTFData(modelPath);
	};

	normalize();
	
	mesh = new ModelMesh(name, (void*)verts.data(), verts.size() * sizeof(ObjModelVertex), indices,
						bindingDescription, attributeDescription, layoutBindings, defaultVertexShader, defaultFragmentShader, true);
}
