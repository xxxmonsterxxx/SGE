#include "CameraObject.h"

void CameraObject::setDefaultPos(SGEPosition pos)
{
    _defaultPosition = pos;
}

void CameraObject::reset()
{
    rotate(-_rotation);

    move(-_position + _defaultPosition);
}

void CameraObject::move(SGEPosition dPos)
{
    for (auto& go : _presentedObjects) {
        go->move(-dPos);
    }

    rotateVector(dPos, {1,0,0}, _rotation.x);
    rotateVector(dPos, {0,1,0}, _rotation.y);

    _position += dPos;
}

glm::vec3 CameraObject::getRotation()
{
    return _rotation;
}

void CameraObject::rotate(glm::vec3 dAngle)
{
    _rotation += dAngle;
    _rotation = wrap180(_rotation);

    // imagine that camera rotates... so for real all object rotates but camera not, so around Y all object rotates by
    // negative angle value. But Y axis and objects rotate by angle around X axis which is fixed with camera
    glm::vec3 yAxis(0,1,0); // warning... axis in Vulkan forwarded down
    rotateVector(yAxis, {1,0,0}, -_rotation.x); // y axis also with object should be rotated by reverse rotation camera angle

    for (auto& go : _presentedObjects) {
        go->rotate({0,0,0}, {1,0,0}, -dAngle.x);
        go->rotate({0,0,0}, yAxis, -dAngle.y);
    }
}