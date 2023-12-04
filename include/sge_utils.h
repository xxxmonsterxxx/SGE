#pragma once

#include <SGR/SGR.h>

#include <string>

using SGEPosition = glm::vec3;
using SGEColor = glm::vec3;

const std::vector<SgrVertex> pointsToSGRVertex(const std::vector<SGEPosition> points);


glm::vec3 rotateVector(glm::vec3 v, glm::vec3 angle);