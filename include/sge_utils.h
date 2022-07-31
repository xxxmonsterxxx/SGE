#pragma once

#include <SGR/SGR.h>

#include <string>

struct SGEPosition {
    float x,y,z;
};

using SgeColor = glm::vec3;

const std::vector<SgrVertex> pointsToSGRVertex(const std::vector<SGEPosition> points);