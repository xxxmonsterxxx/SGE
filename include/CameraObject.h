#pragma once

#include "CoordinateSystem.h"

class CameraObject {

private:

    SGEPosition _defaultPosition{0};
    SGEPosition _position{0};

    glm::vec3 _rotation{0};

    CoordinateSystem* _cs;
    std::string _name;

public:
    CameraObject(CoordinateSystem* cs) : _cs(cs), _name("Camera") { ; }

    void setDefaultPos(SGEPosition pos);
    void reset(); // reset to default position and orientation

    void move(glm::vec3 deltaLookAngle, SGEPosition dPos);

    SGEPosition getPosition();
    glm::vec3 getRotation();
};