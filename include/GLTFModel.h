#pragma once

#include "Mesh.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"

class GLTFMesh : public Mesh {

friend class GLTFModel;

private:

	glm::mat4 model{ 1 };

	struct GLTFTexture {
		unsigned char* pixels = nullptr;
		uint32_t height;
		uint32_t width;
	} texture;

public:
	GLTFMesh(const std::string name, void* vertices, VkDeviceSize verticesSize, const std::vector<uint32_t> indices, const bool filled = true);

    struct GLTFVertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 color;
	};

	struct MeshInstanceData {
		glm::mat4 model;
	};

	const size_t getInstanceDataSize() override {return sizeof(MeshInstanceData);}

	glm::mat4 getRecomendedModel() { return model; }
	GLTFTexture getTexture() { return texture; }
	

protected:

	void generateInstanceData(GameObject* go, void* data) override;

	// default shaders
	const std::string vertexSh = "/DefaultShaders/vertGLTF.spv";
	const std::string fragmentSh = "/DefaultShaders/fragGLTF.spv";
};

class GLTFModel {

public:

	struct Primitive {
		std::string name;
		std::vector<GLTFMesh::GLTFVertex> verts;
		std::vector<uint32_t> inds;
		int textureInd;

		glm::vec3 calculateCenter();
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

	GLTFModel(std::string name, std::string modelPath);

    std::vector<GLTFMesh*>* getMeshes() { return &meshes; }

	glm::vec3 getCenter();

private:

    bool loadData(std::string path);
	void loadGLTFNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, GLTFModel::GLTFNode* parent, std::vector<GLTFModel::GLTFNode*>& nodes);
	bool loadImages(tinygltf::Model input, std::vector<GLTFMesh::GLTFTexture>& textures);

	std::vector<GLTFMesh*> meshes;
    std::vector<GLTFMesh::GLTFTexture> textures;

    std::vector<GLTFNode*> nodes;
	std::vector<uint32_t> indices;

	void normalize() { ; }
};