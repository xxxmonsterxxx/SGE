#pragma once

#include "Mesh.h"
#include "PhysicsObject.h"

class CoordinateSystem;

class GameObjectBase : public PhysicsObject {

friend class SGE;

// Local coordinate system is left-handed and positive rotation is clockwise
// X - right; Y - down; Z - forward to us;

	glm::mat4 _customTransform{1};

protected:
	std::string _name;

	glm::mat4 _positionM{1};
	glm::mat4 _rotationM{1};
	glm::mat4 _scaleM{1};

	glm::mat4 _model{1};

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
	virtual void scale(glm::vec3 sc);
    virtual void scale(float sc);

	virtual SGEPosition getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();

	void additionalTransform(glm::mat4 trans); // don't affect on pos rot scale only for drawing model, so be carefully

	void bindCoordSystem(CoordinateSystem* cs);
};