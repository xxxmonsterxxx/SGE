#pragma once

#include "Mesh.h"

class ObjModelMesh : public Mesh {

public:
	ObjModelMesh(const std::string name, void* vertices, VkDeviceSize verticesSize, const std::vector<uint32_t> indices, uint8_t texturesCount, const bool filled = true);

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

	struct MeshInstanceData {
		glm::mat4 model;
	};

	const size_t getInstanceDataSize() override {return sizeof(MeshInstanceData);}

protected:

	void generateInstanceData(GameObject* go, void* data) override;

	// default shaders
	const std::string vertexSh = "/DefaultShaders/vertObjModel.spv";
	const std::string fragmentSh = "/DefaultShaders/fragObjModel.spv";
};

class ObjModel {

public:

	ObjModel(std::string name, std::string modelPath);

	Mesh& getMesh() { return *mesh; }
	std::vector<std::string> getTextures() { return textures; }


private:

	std::vector<ObjModelMesh::ObjModelVertex> verts;
	std::vector<uint32_t> indices;
	std::vector<std::string> textures;

	ObjModelMesh* mesh = nullptr;

	bool loadData(std::string path);

	void normalize();
};