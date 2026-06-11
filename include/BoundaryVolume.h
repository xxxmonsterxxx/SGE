#pragma once

#include "sge_utils.h"

class BoundaryVolume {

	std::vector<SGEPosition> vertices;
	float max_diagonale = -1;

	bool isCollide2D(std::vector<glm::vec3> AA, std::vector<glm::vec3> BB);
	bool isCollide3D();

public:
	void setVertices(const std::vector<SGEPosition>& _verts);

	float getMaxDiagonale();

	bool isCollide(glm::mat4 model1, BoundaryVolume& obj, glm::mat4 model2);
};