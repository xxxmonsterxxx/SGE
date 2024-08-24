#include "CoordinateSystem.h"

void CoordinateSystem::updateTransMatr()
{
    _transM = _posM * _rotM;
}

void CoordinateSystem::move(glm::vec3 dPos)
{  
    _posM = glm::translate(_posM, dPos);
    updateTransMatr();
}


void CoordinateSystem::rotate(glm::vec3 dAng)
{
    _rotM = glm::rotate(_rotM,glm::radians(dAng.x),{1,0,0});
    _rotM = glm::rotate(_rotM,glm::radians(dAng.y),{0,1,0});
    _rotM = glm::rotate(_rotM,glm::radians(dAng.z),{0,0,1});
    updateTransMatr();
}

CoordinateSystem::CoordinateSystem(glm::vec3 startPos, glm::vec3 startRot)
{
    move(startPos);
    rotate(startRot);
}