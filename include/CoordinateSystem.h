#pragma once

#include "sge_utils.h"

class CoordinateSystem {

public:
    
    CoordinateSystem(glm::vec3 startPos, glm::vec3 startRot=glm::vec3{0,0,0});
    void move(glm::vec3 dPos);
    void rotate(glm::vec3 dAng);
    glm::mat4 getTransMatr() { return _transM; }

private:

    glm::mat4 _transM{1.f};
    glm::mat4 _rotM{1.f};
    glm::mat4 _posM{1.f};

    void updateTransMatr();
};