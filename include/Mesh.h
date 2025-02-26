#pragma once

#include "sge_utils.h"

class SGE;
class GameObject;

class Mesh {

friend class SGE;

private:

    const std::string _name;
    const std::vector<SGEPosition> _vertices;
    const std::vector<uint32_t> _indices;
	const bool _filled;
	bool _textured;

	void* verticesData;
	VkDeviceSize verticesDataSize;

	std::string shaderVertex;
	std::string shaderFragment;

	// default shaders
    const std::string defaultShaderVertex = "/DefaultShaders/vertInstanceSh.spv";
    const std::string defaultShaderFragmentTexture = "/DefaultShaders/fragTextureSh.spv";
	const std::string defaultShaderFragmentColor = "/DefaultShaders/fragColorSh.spv";


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

	virtual const size_t getInstanceDataSize() {return sizeof(MeshInstanceData);}

	void useTexture();

    Mesh(const std::string name, const std::vector<SGEPosition> vertices, const std::vector<uint32_t> indices, const bool filled = true);
	Mesh(const std::string name, void* vertices, VkDeviceSize verticesSize, const std::vector<uint32_t> indices,
												std::vector<VkVertexInputBindingDescription> bindDescr,
												std::vector<VkVertexInputAttributeDescription> attrDescr,
												std::vector<VkDescriptorSetLayoutBinding> layoutBind,
												std::string vertexShader,
												std::string fragmentShader,
												const bool filled = true);

	static Mesh getDefaultRectangleMesh(const std::string name, const bool filld = true);
	static Mesh getDefaultTriangleMesh(const std::string name, const bool filld = true);

	glm::vec2 getTextureBindPoint();

protected:
	virtual void* generateInstanceData(GameObject* go);
};