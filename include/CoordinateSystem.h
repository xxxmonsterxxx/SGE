#pragma once

#include "GameObjectBase.h"

class CoordinateSystem : public GameObjectBase {

public:
    
    CoordinateSystem(glm::vec3 startPos, glm::vec3 startRot=glm::vec3{0,0,0});

    glm::mat4 getTransMatr();
};