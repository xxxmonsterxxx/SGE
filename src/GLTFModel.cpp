#include "GLTFModel.h"

#include "SGE.h"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

#include <filesystem>

GLTFMesh::GLTFMesh(const std::string name, void* vertices, VkDeviceSize verticesSize, const std::vector<uint32_t> indices, const bool filled) :
        Mesh(name, vertices, verticesSize, indices, filled)
{
    VkVertexInputBindingDescription vertexBindingDescription;
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(GLTFVertex);
	vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	_meshBindingDescriptions.push_back(vertexBindingDescription);
	
	VkVertexInputAttributeDescription positionDescr;
	positionDescr.binding = 0;
	positionDescr.location = 0;
	positionDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionDescr.offset = offsetof(GLTFVertex, pos);

	VkVertexInputAttributeDescription normalDescr;
	normalDescr.binding = 0;
	normalDescr.location = 1;
	normalDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalDescr.offset = offsetof(GLTFVertex, normal);

	VkVertexInputAttributeDescription uvDescr;
	uvDescr.binding = 0;
	uvDescr.location = 2;
	uvDescr.format = VK_FORMAT_R32G32_SFLOAT;
	uvDescr.offset = offsetof(GLTFVertex, uv);

	VkVertexInputAttributeDescription colorDescr;
	colorDescr.binding = 0;
	colorDescr.location = 3;
	colorDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorDescr.offset = offsetof(GLTFVertex, color);

	_meshAttributeDescriptions.push_back(positionDescr);
	_meshAttributeDescriptions.push_back(normalDescr);
	_meshAttributeDescriptions.push_back(uvDescr);
	_meshAttributeDescriptions.push_back(colorDescr);

	VkDescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding instanceUBOLayoutBinding{};
	instanceUBOLayoutBinding.binding = 2;
	instanceUBOLayoutBinding.descriptorCount = 1;
	instanceUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	instanceUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	instanceUBOLayoutBinding.pImmutableSamplers = nullptr;

	_additionalDataLayouts.push_back(uboLayoutBinding);
	_additionalDataLayouts.push_back(samplerLayoutBinding);
	_additionalDataLayouts.push_back(instanceUBOLayoutBinding);

	shaderVertex = vertexSh;
	shaderFragment = fragmentSh;
}


void GLTFModel::loadGLTFNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, GLTFModel::GLTFNode* parent, std::vector<GLTFModel::GLTFNode*>& nodes)
{
	GLTFModel::GLTFNode* node = new GLTFModel::GLTFNode{};
	node->name = inputNode.name;
	node->matrix = glm::mat4(1.0f);
	node->parent = parent;

	// Get the local node matrix
	// It's either made up from translation, rotation, scale or a 4x4 matrix
	if (inputNode.translation.size() == 3) {
		node->matrix = glm::translate(node->matrix, glm::vec3(glm::make_vec3(inputNode.translation.data())));
	}
	if (inputNode.rotation.size() == 4) {
		glm::quat q = glm::make_quat(inputNode.rotation.data());
		node->matrix *= glm::mat4(q);
	}
	if (inputNode.scale.size() == 3) {
		node->matrix = glm::scale(node->matrix, glm::vec3(glm::make_vec3(inputNode.scale.data())));
	}
	if (inputNode.matrix.size() == 16) {
		node->matrix = glm::make_mat4x4(inputNode.matrix.data());
	};

	// Load node's children
	if (inputNode.children.size() > 0) {
		for (size_t i = 0; i < inputNode.children.size(); i++) {
			loadGLTFNode(input.nodes[inputNode.children[i]], input, node, nodes);
		}
	}

	// If the node contains mesh data, we load vertices and indices from the buffers
	// In glTF this is done via accessors and buffer views
	if (inputNode.mesh > -1) {
		const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
		// Iterate through all primitives of this node's mesh
		for (size_t i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
			GLTFModel::Primitive* primitive = new GLTFModel::Primitive{};
			primitive->name = mesh.name + "_" + std::to_string(i);
			uint32_t indexCount = 0;
			// Vertices
			{
				const float* positionBuffer = nullptr;
				const float* normalsBuffer = nullptr;
				const float* texCoordsBuffer = nullptr;
				size_t vertexCount = 0;

				// Get buffer data for vertex positions
				if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					positionBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					vertexCount = accessor.count;
				}
				// Get buffer data for vertex normals
				if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}
				// Get buffer data for vertex texture coordinates
				// glTF supports multiple sets, we only load the first one
				if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					texCoordsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}

				// Append data to model's vertex buffer
				for (size_t v = 0; v < vertexCount; v++) {
					GLTFMesh::GLTFVertex vert{};
					vert.pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
					vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
					vert.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
					vert.color = glm::vec3(1.0f);
					primitive->verts.push_back(vert);
				}

				primitive->textureInd = input.materials[glTFPrimitive.material].values.find("baseColorTexture")->second.TextureIndex();
			}
			// Indices
			{
				const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
				const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

				indexCount += static_cast<uint32_t>(accessor.count);

				// glTF supports different component types of indices
				switch (accessor.componentType) {
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
					for (size_t index = 0; index < accessor.count; index++) {
						primitive->inds.push_back(buf[index]);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
					for (size_t index = 0; index < accessor.count; index++) {
						primitive->inds.push_back(buf[index]);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
					for (size_t index = 0; index < accessor.count; index++) {
						primitive->inds.push_back(buf[index]);
					}
					break;
				}
				default:
					return;
				}
			}
			//primitive.materialIndex = glTFPrimitive.material;
			node->primitives.push_back(*primitive);
		}
	}

	if (parent) {
		parent->children.push_back(node);
	}
	else {
		nodes.push_back(node);
	}
}

bool GLTFModel::loadImages(tinygltf::Model input, std::vector<GLTFMesh::GLTFTexture>& textures)
{
	// Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
	// loading them from disk, we fetch them from the glTF loader and upload the buffers
	textures.resize(input.images.size());
	for (size_t i = 0; i < input.images.size(); i++) {
		printf("\rLoad images...[%d]", (int)(i / (float)input.images.size() * 100));
		fflush(stdout);
		tinygltf::Image& glTFImage = input.images[i];
		// Get the image data from the glTF loader
		unsigned char* buffer = nullptr;
		VkDeviceSize bufferSize = 0;
		bool deleteBuffer = false;
		// We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
		if (glTFImage.component == 3) {
			bufferSize = glTFImage.width * glTFImage.height * 4;
			buffer = new unsigned char[bufferSize];
			unsigned char* rgba = buffer;
			unsigned char* rgb = &glTFImage.image[0];
			for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
				memcpy(rgba, rgb, sizeof(unsigned char) * 3);
				rgba += 4;
				rgb += 3;
			}
			deleteBuffer = true;
		}
		else {
			buffer = &glTFImage.image[0];
			bufferSize = glTFImage.image.size();
		}

		textures[i].pixels = new unsigned char[bufferSize];
		textures[i].height = glTFImage.height;
		textures[i].width = glTFImage.width;
		memcpy(textures[i].pixels, buffer, bufferSize);
		// Load texture from image buffer

		if (deleteBuffer) {
			delete[] buffer;
		}
	}

	printf("\rLoad images...[%d]\n", 100);
	fflush(stdout);

	return true;
}


void GLTFMesh::generateInstanceData(GameObject* go, void* data)
{
	MeshInstanceData newData{};
	newData.model = go->getModel();
	*(MeshInstanceData*)data = newData;
}

glm::vec3 GLTFModel::Primitive::calculateCenter()
{
	glm::vec3 centroid{ 0,0,0 };

	for (const auto& v : verts)
		centroid += v.pos / (float)verts.size();

    return centroid;
}

glm::vec3 GLTFModel::getCenter()
{
	glm::vec3 centroid{ 0,0,0 };
	int count = -1;
	for (const auto& parrent : nodes) {
		for (const auto& node : nodes[0]->children) {
			for (auto& prim : node->primitives) {
				count++;
				centroid += prim.calculateCenter();
			}
		}
	}

	centroid /= (float)count;

	// maybe normalize

    // glm::vec3 centroid{ 0,0,0 };
	// glm::vec3 maxCoord{verts[0].pos.x,verts[0].pos.y,verts[0].pos.z};
	// glm::vec3 minCoord{verts[0].pos.x,verts[0].pos.y,verts[0].pos.z};
	// for (const auto& v : verts) {
	// 	centroid += v.pos;

	// 	if (v.pos.x > maxCoord.x) maxCoord.x = v.pos.x;
	// 	if (v.pos.y > maxCoord.y) maxCoord.y = v.pos.y;
	// 	if (v.pos.z > maxCoord.z) maxCoord.z = v.pos.z;

	// 	if (v.pos.x < minCoord.x) minCoord.x = v.pos.x;
	// 	if (v.pos.y < minCoord.y) minCoord.y = v.pos.y;
	// 	if (v.pos.z < minCoord.z) minCoord.z = v.pos.z;
	// }

	// float maxDistance = maxCoord.x - minCoord.x;
	// if ((maxCoord.y - minCoord.y) > maxDistance) maxDistance = maxCoord.y - minCoord.y;
	// if ((maxCoord.z - minCoord.z) > maxDistance) maxDistance = maxCoord.z - minCoord.z;

	// for (auto& v : verts) {
	// 	v.pos -= (centroid / (float)verts.size());
	// 	v.pos /= maxDistance / 2;
	// }

    return centroid;
}

bool GLTFModel::loadData(std::string path)
{
	tinygltf::Model glTFInput;
	tinygltf::TinyGLTF gltfContext;
	std::string error, warning;

	bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, SGE::getResourcesPath()+path);

	if (!fileLoaded)
		return false;

	if (!loadImages(glTFInput, textures))
		return false;
	/*glTFModel.loadMaterials(glTFInput);
	glTFModel.loadTextures(glTFInput);*/

	const tinygltf::Scene& scene = glTFInput.scenes[0];
	for (size_t i = 0; i < scene.nodes.size(); i++) {
		printf("\rLoad nodes...[%d]", (int)(i / (float)scene.nodes.size() * 100));
		fflush(stdout);
		const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
		loadGLTFNode(node, glTFInput, nullptr, nodes);
	}

	printf("\rLoad nodes...[%d]", 100);
	fflush(stdout);

	return true;
}


GLTFModel::GLTFModel(std::string name, std::string modelPath)
{
    loadData(modelPath);

	for (const auto& parrent : nodes) {
		for (const auto& node : nodes[0]->children) {
			for (auto& prim : node->primitives) {
				GLTFMesh* m = new GLTFMesh(name + "_" + prim.name, (void*)prim.verts.data(), prim.verts.size() * sizeof(GLTFMesh::GLTFVertex), prim.inds);
				
				GLTFNode* currentParrent = node->parent;
				while (currentParrent) {
					m->model = currentParrent->matrix * node->matrix;
					currentParrent = currentParrent->parent;
				}

				m->texture = textures[prim.textureInd];

				meshes.push_back(m);
			}
		}
	}
}
