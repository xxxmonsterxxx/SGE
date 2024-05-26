#include "sge_utils.h"

const std::vector<SgrVertex> pointsToSGRVertex(const std::vector<SGEPosition> points)
{
    std::vector<SgrVertex> ret;

	auto i = 0;
	for (auto point : points) {
		SgrVertex pConvert;
		pConvert = glm::vec3(point.x,point.y,point.z);
		i++;
		ret.push_back(pConvert);
	}

    return ret;
}

glm::vec3 getAnglesFromRotationMatrx(glm::mat4 R)
{
	glm::vec3 ret;

	ret.y = asinf(-R[0][2]);
	ret.x = atan2f(R[1][2], R[2][2]);
	ret.z = atan2f(R[0][1], R[0][0]);

	ret.x = glm::degrees(ret.x);
	ret.y = glm::degrees(ret.y);
	ret.z = glm::degrees(ret.z);

	return ret;
}