#pragma once

#include "sge_utils.h"
#include <SGR.h>

class SGE;

class Mesh {

friend class SGE;

private:

    const std::string _name;
    const std::vector<SGEPosition> _vertices;
    const std::vector<uint16_t> _indices;


	// default shaders
    const std::string shaderVertex = "DefaultShaders/vertex.spv";
    const std::string shaderFragment = "DefaultShaders/fragment.spv";

protected:

	std::vector<VkVertexInputBindingDescription> _meshBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> _meshAttributeDescriptions;
	std::vector<VkDescriptorSetLayoutBinding> _additionalDataLayouts;

public:
	bool init();
	struct MeshInstanceData {
		glm::mat4 model = glm::mat4(1);
		glm::vec2 texCoord = glm::vec2(0.5);
		glm::vec2 texSize = glm::vec2(2);
	};

    Mesh(const std::string name, const std::vector<SGEPosition> vertices, const std::vector<uint16_t> indices);
};