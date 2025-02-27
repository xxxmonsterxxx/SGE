#pragma once

#include "Mesh.h"

class ModelMesh : public Mesh {

public:
	using Mesh::Mesh;
	struct MeshInstanceData {
		glm::mat4 model;
	};

	const size_t getInstanceDataSize() override {return sizeof(MeshInstanceData);}

protected:

	void generateInstanceData(GameObject* go, void* data) override;
};

class Model {

private:

	ModelMesh* mesh;

private:
	bool loadObjData(std::string path);
	bool loadGLTFData(std::string path);

	void normalize();

	std::vector<VkVertexInputAttributeDescription> attributeDescription;
	std::vector<VkVertexInputBindingDescription> bindingDescription;
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

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

		bool operator==(const ObjModelVertex& other) const
		{
			return vertex == other.vertex;
		}
	};

	struct ObjModelInstanceData {
		glm::mat4 model;
	};

	Model(std::string name, std::string modelPath, ModelType type);

	Mesh& getMesh() { return *mesh; }
	std::vector<std::string> getTextures() { return textures; }

private:

	std::vector<ObjModelVertex> verts;
	std::vector<uint32_t> indices;
	std::vector<std::string> textures;

	std::string defaultVertexShader = "/DefaultShaders/vertObjModel.spv";
	std::string defaultFragmentShader = "/DefaultShaders/fragObjModel.spv";

};