#include "BoundaryVolume.h"

void BoundaryVolume::setVertices(const std::vector<SGEPosition>& _verts)
{
	if (_verts.size() != 4 && _verts.size() != 8) // 2d or 3d
		return;

	vertices = _verts;
}


bool BoundaryVolume::isCollide2D(std::vector<glm::vec3> AA, std::vector<glm::vec3> BB)
{
	return false;
}


bool BoundaryVolume::isCollide3D()
{
	return true;
}


bool BoundaryVolume::isCollide(glm::mat4 model1, BoundaryVolume& obj, glm::mat4 model2)
{
	if (vertices.empty() || obj.vertices.empty() || vertices.size() != obj.vertices.size())
		return false;

	std::vector<glm::vec3> b1;
	for (auto& p : vertices)
		b1.push_back(model1 * glm::vec4(p, 1));

	std::vector<glm::vec3> b2;
	for (auto& p : obj.vertices)
		b2.push_back(model2 * glm::vec4(p, 1));

	if (vertices.size() == 4)
		return isCollide2D(b1, b2);
	else
		return isCollide3D();

	return false;
}


float BoundaryVolume::getMaxDiagonale()
{
	if (max_diagonale > 0)
		return max_diagonale;

	for (auto& pt1 : vertices) {
		for (auto& pt2 : vertices) {
			float dist = glm::distance(pt1, pt2);
			if (dist > max_diagonale)
				max_diagonale = dist;
		}
	}

	return max_diagonale;
}