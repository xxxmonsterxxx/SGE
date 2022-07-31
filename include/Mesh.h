#pragma once

#include "sge_utils.h"
#include <SGR/SGR.h>

class SGE;

class Mesh {

friend class SGE;

private:

    const std::string _name;
    const std::vector<SGEPosition> _vertices;
    const std::vector<uint16_t> _indices;
	const bool _filled;
	bool _textured;


	// default shaders
    const std::string shaderVertex = "Resources/DefaultShaders/vertInstanceSh.spv";
    const std::string shaderFragmentTexture = "Resources/DefaultShaders/fragTextureSh.spv";
	const std::string shaderFragmentColor = "Resources/DefaultShaders/fragColorSh.spv";

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
		glm::vec3 color = glm::vec3(0, 1, 0);
	};

	void setTextured(bool isTextured) { _textured = isTextured; }

    Mesh(const std::string name, const std::vector<SGEPosition> vertices, const std::vector<uint16_t> indices, const bool filled = true);
};