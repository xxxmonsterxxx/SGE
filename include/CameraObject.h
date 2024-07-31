#pragma once

#include "GameObjectBase.h"

class CameraObject : public GameObjectBase {

private:
    std::vector<GameObjectBase*>& _presentedObjects;

public:
    CameraObject(std::vector<GameObjectBase*>& presentedObjects) : _presentedObjects(presentedObjects) { ; }

    void move(SGEPosition dPos) override;
};