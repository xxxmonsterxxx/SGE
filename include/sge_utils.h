#pragma once

#include <SGR.h>

#include <string>

struct SGEPosition {
    float x,y,z;
};

const std::vector<Sgr2DVertex> pointsToSGRVertex(const std::vector<SGEPosition> points, const std::vector<float> colors = std::vector<float>{1,1,1});