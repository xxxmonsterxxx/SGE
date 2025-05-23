#include "Model.h"

#include "SGE.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"

#include <filesystem>

void loadGLTFNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, Model::GLTFNode* parent, std::vector<Model::GLTFNode*>& nodes)
{
	Model::GLTFNode* node = new Model::GLTFNode{};
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
			Model::Primitive* primitive = new Model::Primitive{};
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
					Model::GLTFVertex vert{};
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

			//gltfPrimitives[primitive->name] = *new SgrInstancesUBO{};
			//gltfPrimitives[primitive->name].instnaceCount = 1;
			//gltfPrimitives[primitive->name].instanceSize = sizeof(ModelInstanceData);
			//if (MemoryManager::createDynamicUniformMemory(gltfPrimitives[primitive->name]) != sgrOK)
			//	return;

			//if (MemoryManager::get()->createDynamicUniformBuffer(gltfPrimitives[primitive->name].ubo, gltfPrimitives[primitive->name].dataSize, gltfPrimitives[primitive->name].dynamicAlignment) != sgrOK)
			//	return;
		}
	}

	if (parent) {
		parent->children.push_back(node);
	}
	else {
		nodes.push_back(node);
	}
}

bool loadImages(tinygltf::Model input, std::vector<SgrImage*>& images)
{
	printf("\nLoading images\n");
	// Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
	// loading them from disk, we fetch them from the glTF loader and upload the buffers
	images.resize(input.images.size());
	for (size_t i = 0; i < input.images.size(); i++) {
		printf("\rLoad process...[%d]", (int)(i / (float)input.images.size() * 100));
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
		// Load texture from image buffer
		if (TextureManager::createTextureImage((void*)buffer, glTFImage.width, glTFImage.height, test) != sgrOK)
			return false;

		if (deleteBuffer) {
			delete[] buffer;
		}
	}

	printf("\rLoad process...[%d]", 100);
	fflush(stdout);

	return true;
}


void ModelMesh::generateInstanceData(GameObject* go, void* data)
{
	MeshInstanceData newData{};
	newData.model = go->getModel();
	*(MeshInstanceData*)data = newData;
}


bool Model::loadGLTFData(std::string path)
{
	tinygltf::Model glTFInput;
	tinygltf::TinyGLTF gltfContext;
	std::string error, warning;

	bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, SGE::getResourcesPath()+path);

	if (!fileLoaded)
		return false;

	//if (!loadImages(glTFInput, GLTFTextures))
	//	return false;
	/*glTFModel.loadMaterials(glTFInput);
	glTFModel.loadTextures(glTFInput);*/

	const tinygltf::Scene& scene = glTFInput.scenes[0];
	printf("\nLoading nodes\n");
	for (size_t i = 0; i < scene.nodes.size(); i++) {
		printf("\rLoad process...[%d]", (int)(i / (float)scene.nodes.size() * 100));
		fflush(stdout);
		const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
		loadGLTFNode(node, glTFInput, nullptr, GLTFNodes);
	}

	printf("\rLoad process...[%d]", 100);
	fflush(stdout);

	VkVertexInputBindingDescription vertexBindingDescription;
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(GLTFVertex);
	vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescription.push_back(vertexBindingDescription);
	
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

	attributeDescription.push_back(positionDescr);
	attributeDescription.push_back(normalDescr);
	attributeDescription.push_back(uvDescr);
	attributeDescription.push_back(colorDescr);

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

	layoutBindings.push_back(uboLayoutBinding);
	layoutBindings.push_back(samplerLayoutBinding);
	layoutBindings.push_back(instanceUBOLayoutBinding);

	return true;
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

	normalizeOBJ();

	VkVertexInputBindingDescription vBindingD{};
	vBindingD.binding = 0;
	vBindingD.stride = sizeof(ObjModelVertex);
	vBindingD.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDescription.push_back(vBindingD);

	VkVertexInputAttributeDescription posD{};
	posD.binding = 0;
	posD.location = 0;
	posD.format = VK_FORMAT_R32G32B32_SFLOAT;
	posD.offset = offsetof(ObjModelVertex, pos);
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

void Model::normalizeOBJ()
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

Model::Model(std::string name, std::string modelPath, ModelType type)
{
	switch (type) {
		case OBJ:
			loadObjData(modelPath);

			mesh = new ModelMesh(name, (void*)verts.data(), verts.size() * sizeof(ObjModelVertex), indices,
				bindingDescription, attributeDescription, layoutBindings, objVertexShader, objFragmentShader, true);
			break;
		case GLTF:
			loadGLTFData(modelPath);

			mesh = new ModelMesh(name, (void*)verts.data(), verts.size() * sizeof(ObjModelVertex), indices,
				bindingDescription, attributeDescription, layoutBindings, objVertexShader, objFragmentShader, true);
	};
}
