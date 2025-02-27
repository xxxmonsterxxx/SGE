#include "CoordinateSystem.h"

glm::mat4 CoordinateSystem::getTransMatr()
{
    return _model;
}

CoordinateSystem::CoordinateSystem(glm::vec3 startPos, glm::vec3 startRot)
{
    move(startPos);
    rotate(startRot);
}