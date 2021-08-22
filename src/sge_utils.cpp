#include "sge_utils.h"

const std::vector<Sgr2DVertex> pointsToSGRVertex(const std::vector<SGEPosition> points, const std::vector<float> colors)
{
    std::vector<Sgr2DVertex> ret;

	for (auto point : points) {
		Sgr2DVertex pConvert;
		pConvert.position = glm::vec2(point.x,point.y);
		pConvert.color = glm::vec3(1.f,0.f,0.f);
		ret.push_back(pConvert);
	}

    return ret;
}