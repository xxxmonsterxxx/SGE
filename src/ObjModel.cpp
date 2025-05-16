#include "ObjModel.h"

#include "SGE.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <filesystem>


ObjModelMesh::ObjModelMesh(const std::string name, void* vertices, VkDeviceSize verticesSize, const std::vector<uint32_t> indices, uint8_t texturesCount, const bool filled) : 
			Mesh(name, vertices, verticesSize, indices, filled)
{
	VkVertexInputBindingDescription vBindingD{};
	vBindingD.binding = 0;
	vBindingD.stride = sizeof(ObjModelVertex);
	vBindingD.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	_meshBindingDescriptions.push_back(vBindingD);

	VkVertexInputAttributeDescription posD{};
	posD.binding = 0;
	posD.location = 0;
	posD.format = VK_FORMAT_R32G32B32_SFLOAT;
	posD.offset = offsetof(ObjModelVertex, pos);
	_meshAttributeDescriptions.push_back(posD);

	VkVertexInputAttributeDescription colorD{};
	colorD.binding = 0;
	colorD.location = 3;
	colorD.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	colorD.offset = offsetof(ObjModelVertex, color);
	_meshAttributeDescriptions.push_back(colorD);

	VkVertexInputAttributeDescription texIndD{};
	texIndD.binding = 0;
	texIndD.location = 1;
	texIndD.format = VK_FORMAT_R32_SINT;
	texIndD.offset = offsetof(ObjModelVertex, texInd);
	_meshAttributeDescriptions.push_back(texIndD);

	VkVertexInputAttributeDescription texCoordD{};
	texCoordD.binding = 0;
	texCoordD.location = 2;
	texCoordD.format = VK_FORMAT_R32G32_SFLOAT;
	texCoordD.offset = offsetof(ObjModelVertex, texCoord);
	_meshAttributeDescriptions.push_back(texCoordD);

	VkDescriptorSetLayoutBinding uboB{};
	uboB.binding = 0;
	uboB.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboB.descriptorCount = 1;
	uboB.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboB.pImmutableSamplers = nullptr;
	_additionalDataLayouts.push_back(uboB);

	VkDescriptorSetLayoutBinding instanceUboB{};
	instanceUboB.binding = 1;
	instanceUboB.descriptorCount = 1;
	instanceUboB.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	instanceUboB.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	instanceUboB.pImmutableSamplers = nullptr;
	_additionalDataLayouts.push_back(instanceUboB);

	VkDescriptorSetLayoutBinding samplerB{};
	samplerB.binding = 2;
	samplerB.descriptorCount = texturesCount;
	samplerB.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerB.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerB.pImmutableSamplers = nullptr;
	_additionalDataLayouts.push_back(samplerB);

	shaderVertex = vertexSh;
	shaderFragment = fragmentSh;
}


void ObjModelMesh::generateInstanceData(GameObject* go, void* data)
{
	MeshInstanceData newData{};
	newData.model = go->getModel();
	*(MeshInstanceData*)data = newData;
}

void ObjModel::normalize()
{
	SgrVertex centroid{ 0,0,0 };
	glm::vec3 maxCoord{verts[0].pos.x,verts[0].pos.y,verts[0].pos.z};
	glm::vec3 minCoord{verts[0].pos.x,verts[0].pos.y,verts[0].pos.z};
	for (const auto& v : verts) {
		centroid += v.pos;

		if (v.pos.x > maxCoord.x) maxCoord.x = v.pos.x;
		if (v.pos.y > maxCoord.y) maxCoord.y = v.pos.y;
		if (v.pos.z > maxCoord.z) maxCoord.z = v.pos.z;

		if (v.pos.x < minCoord.x) minCoord.x = v.pos.x;
		if (v.pos.y < minCoord.y) minCoord.y = v.pos.y;
		if (v.pos.z < minCoord.z) minCoord.z = v.pos.z;
	}

	float maxDistance = maxCoord.x - minCoord.x;
	if ((maxCoord.y - minCoord.y) > maxDistance) maxDistance = maxCoord.y - minCoord.y;
	if ((maxCoord.z - minCoord.z) > maxDistance) maxDistance = maxCoord.z - minCoord.z;

	for (auto& v : verts) {
		v.pos -= (centroid / (float)verts.size());
		v.pos /= maxDistance / 2;
	}
}

bool ObjModel::loadData(std::string path)
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
			ObjModelMesh::ObjModelVertex v{};
			v.pos.x = attrib.vertices[(size_t)3 * index.vertex_index + 0];
			v.pos.y = attrib.vertices[(size_t)3 * index.vertex_index + 1];
			v.pos.z = attrib.vertices[(size_t)3 * index.vertex_index + 2];

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

	normalize();

	return true;
}

ObjModel::ObjModel(std::string name, std::string modelPath)
{
	loadData(modelPath);

	mesh = new ObjModelMesh(name, (void*)verts.data(), verts.size() * sizeof(ObjModelMesh::ObjModelVertex), indices, textures.size(), true);
}