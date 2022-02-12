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