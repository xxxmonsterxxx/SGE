#include "CameraObject.h"

void CameraObject::move(SGEPosition dPos)
{
    for (auto& go : _presentedObjects) {
            go->move(-dPos);
    }
}