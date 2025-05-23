#pragma once

#include "Mesh.h"

class ModelMesh : public Mesh {

public:
	using Mesh::Mesh;

	ModelMesh(const std::string name, void* vertices, VkDeviceSize verticesSize, const std::vector<uint32_t> indices,
		std::vector<VkVertexInputBindingDescription> bindDescr,
		std::vector<VkVertexInputAttributeDescription> attrDescr,
		std::vector<VkDescriptorSetLayoutBinding> layoutBind,
		std::string vertexShader,
		std::string fragmentShader,
		const bool filled = true);

	struct MeshInstanceData {
		glm::mat4 model;
	};

	const size_t getInstanceDataSize() override {return sizeof(MeshInstanceData);}

protected:

	void generateInstanceData(GameObject* go, void* data) override;
};

class Model {

public:
	enum ModelType {
		OBJ,
		GLTF
	};

	struct ObjModelVertex {
		glm::vec3 pos;
		glm::vec4 color;
		int texInd;
		glm::vec2 texCoord;

		bool operator==(const ObjModelVertex& other) const
		{
			return pos == other.pos;
		}
	};

	struct ObjModelInstanceData {
		glm::mat4 model;
	};

	struct GLTFVertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 color;
	};

	struct Primitive {
		std::string name;
		std::vector<GLTFVertex> verts;
		std::vector<uint32_t> inds;
		int textureInd;
	};

	struct GLTFNode {
		std::string name;
		GLTFNode* parent;
		std::vector<GLTFNode*> children;
		std::vector<Primitive> primitives;
		glm::mat4 matrix;
		~GLTFNode() {
			for (auto& child : children) {
				delete child;
			}
		}
	};

	Model(std::string name, std::string modelPath, ModelType type);

	Mesh& getMesh() { return *mesh; }
	std::vector<std::string> getTextures() { return textures; }


private:

	ModelMesh* mesh = nullptr;

	bool loadObjData(std::string path);
	bool loadGLTFData(std::string path);

	void normalizeOBJ();

	std::vector<VkVertexInputAttributeDescription> attributeDescription;
	std::vector<VkVertexInputBindingDescription> bindingDescription;
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

	std::vector<ObjModelVertex> verts;
	std::vector<uint32_t> indices;
	std::vector<std::string> textures;

	std::vector<GLTFNode*> GLTFNodes;
	std::vector<SgrImage*> GLTFTextures;

	// default shaders
	std::string objVertexShader = "/DefaultShaders/vertObjModel.spv";
	std::string objFragmentShader = "/DefaultShaders/fragObjModel.spv";

	std::string gltfVertexShader = "/DefaultShaders/vertGLTF.spv";
	std::string gltfFragmentShader = "/DefaultShaders/fragGLTF.spv";

};