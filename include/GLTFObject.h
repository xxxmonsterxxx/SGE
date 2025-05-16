#pragma once

#include "GameObject.h"
#include "GLTFModel.h"

class GLTFObject : public GameObjectBase {

public:

	GLTFObject(const std::string name, GLTFModel& model);

    std::vector<GameObject*> getGameObjectsData();

    void setPosition(SGEPosition newPosition) override;
	void setRotation(glm::vec3 angle) override;
	void rotate(glm::vec3 dAngle) override;
	void rotate(glm::vec3 begin, glm::vec3 end, float angle) override;
	void move(glm::vec3 dPos) override;

	void scale(glm::vec3 sc) override;
	void scale(float sc) override;

	glm::vec3 getPosition() override;

private:

    glm::vec3 _position;

    std::vector<GameObject*> _gameObjectsData;
};