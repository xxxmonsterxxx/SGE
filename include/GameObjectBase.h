#pragma once

#include "Mesh.h"
#include "PhysicsObject.h"

class CoordinateSystem;

class GameObjectBase : public PhysicsObject {

friend class SGE;

// Local coordinate system is left-handed and positive rotation is clockwise
// X - right; Y - down; Z - forward to us;
private:

	Mesh::MeshInstanceData getInstanceData();

protected:
	std::string _name;
	Mesh::MeshInstanceData _instanceData;

	glm::mat4 _positionM{1};
	glm::mat4 _rotationM{1};
	glm::mat4 _scaleM{1};

	CoordinateSystem* _cs = nullptr;

	void updateModel();

public:
	std::string getName() { return _name; }

	// default interface
    virtual void setPosition(SGEPosition newPosition);
	virtual void setRotation(SGEPosition newRotation);
	void setScale(glm::vec3 newScale);

	virtual void move(SGEPosition dPos) override;
	virtual void rotate(glm::vec3 dAngle);
	virtual void rotate(glm::vec3 begin, glm::vec3 end, float angle);
	void scale(glm::vec3 sc);
    void scale(float sc);

	virtual SGEPosition getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();

	void bindCoordSystem(CoordinateSystem* cs);
};