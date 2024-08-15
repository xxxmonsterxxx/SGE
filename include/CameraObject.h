#pragma once

#include "GameObjectBase.h"

class CameraObject : public GameObjectBase {

private:
    std::vector<GameObjectBase*>& _presentedObjects;

    SGEPosition _defaultPosition;
    SGEPosition _position;

    glm::vec3 _rotation;

public:
    CameraObject(std::vector<GameObjectBase*>& presentedObjects) : _presentedObjects(presentedObjects) { ; }

    void setDefaultPos(SGEPosition pos);
    void move(SGEPosition dPos) override;

    void rotate(glm::vec3 dAngle) override;
    glm::vec3 getRotation();

    void reset(); // reset to default position and orientation
};