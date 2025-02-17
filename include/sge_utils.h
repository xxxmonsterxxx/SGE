#pragma once

#include <SGR.h>

#include <string>

using SGEPosition = glm::vec3;
using SGEColor = glm::vec3;

glm::vec3 getAnglesFromRotationMatrx(glm::mat4 R);

void rotateVector(glm::vec3& v, glm::vec3 axis, float angle);
float wrap180(float ang);
glm::vec3 wrap180(glm::vec3 angles);
void printVec(const char* prefix, glm::vec3 v);