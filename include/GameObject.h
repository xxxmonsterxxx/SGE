#pragma once

#include "sge_utils.h"
#include "Mesh.h"

class SGE;

class GameObject {

friend class SGE;

public:
    GameObject(const std::string name, Mesh& mesh, const std::string& texture = "empty");
    GameObject(const std::string name, Mesh& mesh, const std::vector<float>& colors);

    void setPosition(SGEPosition newPosition);
	void setTexPosSize(glm::vec2 texPos, glm::vec2 texSize);
    void setScale(float newScale);

private:
	std::string _name;

    SGEPosition _position;

    float _scale = 1;

	const std::string _texturePath;

    const std::vector<float> _colors;

    Mesh& _mesh;

	Mesh::MeshInstanceData _instanceData;

    bool init(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer);

};