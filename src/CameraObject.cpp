#include "CameraObject.h"

void CameraObject::setDefaultPos(SGEPosition pos)
{
    _defaultPosition = pos;
}

void CameraObject::reset()
{
    move(-_rotation, (-_position + _defaultPosition));
}

void CameraObject::move(glm::vec3 deltaLookAngle, SGEPosition dPos)
{
    _rotation += deltaLookAngle;
    _rotation = wrap180(_rotation);

    // imagine that camera rotates... so for real all object rotates but camera not, so around Y all object rotates by
    // negative angle value. But Y axis and objects rotate by angle around X axis which is fixed with camera
    glm::vec3 yAxis(0,1,0); // warning... axis in Vulkan forwarded down
    rotateVector(yAxis, {1,0,0}, -_rotation.x); // y axis also with object should be rotated by reverse rotation camera angle

    _cs->rotate({0,0,0}, {1,0,0}, -deltaLookAngle.x);
    _cs->rotate({0,0,0}, yAxis, -deltaLookAngle.y);
    _cs->move(-dPos);

    rotateVector(dPos, {1,0,0}, _rotation.x);
    rotateVector(dPos, {0,1,0}, _rotation.y);

    _position += dPos;
}

SGEPosition CameraObject::getPosition()
{
    return _position;
}

glm::vec3 CameraObject::getRotation()
{
    return _rotation;
}