#pragma once

#include "sge_utils.h"
#include "Mesh.h"

class SGE;

class GameObject {

friend class SGE;

public:
    GameObject(const std::string name, Mesh& mesh, const std::string& texture);
    GameObject(const std::string name, Mesh& mesh);

    void setPosition(SGEPosition newPosition);
	void setRotation(glm::vec3 axis, float angle);
	void setTexPosSize(glm::vec2 texPos, glm::vec2 texSize);
    void setScale(glm::vec3 newScale);
	void setColor(SgeColor newColor);

private:
	std::string _name;

    SGEPosition _position;

    glm::vec3 _scale = glm::vec3(1);

	std::string _texturePath = "/Resources/Textures/empty.jpg";

    SgeColor _color;

    Mesh& _mesh;

	Mesh::MeshInstanceData _instanceData;

    bool init(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer);

};