#pragma once

#include "Mesh.h"

class Model {

private:

	Mesh* mesh;

private:
	bool loadObjData(std::string path);
	bool loadGLTFData(std::string path);

	void normalize();

	const std::vector<VkVertexInputAttributeDescription> objAttributeDescription{

	};

	const std::vector<VkVertexInputBindingDescription> objBindingDescription{
	};

	const std::vector<VkDescriptorSetLayoutBinding> objLayoutBindings{

	};

	const std::vector<VkVertexInputAttributeDescription> gltfAttributeDescription;
	const std::vector<VkVertexInputBindingDescription> gltfBindingDescription;
	const std::vector<VkDescriptorSetLayoutBinding> gltfLayoutBindings;

public:
	enum ModelType {
		OBJ,
		GLTF
	};

	struct ObjModelVertex {
		glm::vec3 vertex;
		glm::vec4 color;
		int texInd;
		glm::vec2 texCoord;
	};

	Model(std::string name, std::string modelPath, ModelType type);

	Mesh& getMesh() { return *mesh; }

private:

	void* vertices;
	std::vector<uint32_t> indices;

};