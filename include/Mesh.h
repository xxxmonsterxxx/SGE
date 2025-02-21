#pragma once

#include "sge_utils.h"

class SGE;

class Mesh {

friend class SGE;

private:

    const std::string _name;
    const std::vector<SGEPosition> _vertices;
    const std::vector<uint32_t> _indices;
	const bool _filled;
	bool _textured;


	// default shaders
    const std::string shaderVertex = "/Resources/DefaultShaders/vertInstanceSh.spv";
    const std::string shaderFragmentTexture = "/Resources/DefaultShaders/fragTextureSh.spv";
	const std::string shaderFragmentColor = "/Resources/DefaultShaders/fragColorSh.spv";


	// default meshes (vertices with indices)
	static const std::vector <SGEPosition> defaultRectangleVertices;
	static const std::vector<uint32_t> defaultRectangleIndices;
	static const std::vector <SGEPosition> defaultTriangleVertices;
	static const std::vector<uint32_t> defaultTriangleIndices;

protected:

	std::vector<VkVertexInputBindingDescription> _meshBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> _meshAttributeDescriptions;
	std::vector<VkDescriptorSetLayoutBinding> _additionalDataLayouts;

public:
	bool init();

	struct MeshInstanceData {
		glm::mat4 model{1};
		glm::vec4 color{1, 0, 0, 0};
		glm::vec2 meshToTextureDelta{1};
		glm::vec2 meshStart{0,0};
		glm::vec2 textureStart{0};
	};

	void setTextured(bool isTextured) { _textured = isTextured; }

    Mesh(const std::string name, const std::vector<SGEPosition> vertices, const std::vector<uint32_t> indices, const bool filled = true);
	Mesh(const std::string name, void* vertices, const std::vector<uint32_t> indices,
												std::vector<VkVertexInputBindingDescription> bindDescr,
												std::vector<VkVertexInputAttributeDescription> attrDescr,
												std::vector<VkDescriptorSetLayoutBinding> layoutBind,
												const bool filled = true);

	static Mesh getDefaultRectangleMesh(const std::string name, const bool filld = true);
	static Mesh getDefaultTriangleMesh(const std::string name, const bool filld = true);

	glm::vec2 getTextureBindPoint();
};