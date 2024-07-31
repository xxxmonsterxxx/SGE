#pragma once

#include "Mesh.h"

class GameObjectBase {

// Coordinate system is left-handed and positive rotation is clockwise

protected:
	std::string _name;
	Mesh::MeshInstanceData _instanceData;

	glm::mat4 _positionM{1};
	glm::mat4 _rotationM{1};
	glm::mat4 _scaleM{1};

	void updateModel();

public:
	std::string getName() { return _name; }

	// default interface
    virtual void setPosition(SGEPosition newPosition);
	virtual void setRotation(SGEPosition newRotation);
	void setScale(glm::vec3 newScale);

	virtual void move(SGEPosition dPos);
	virtual void rotate(glm::vec3 dAngle);
	virtual void rotate(glm::vec3 begin, glm::vec3 end, float angle);
	void scale(glm::vec3 sc);
    void scale(float sc);

	virtual SGEPosition getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();
};