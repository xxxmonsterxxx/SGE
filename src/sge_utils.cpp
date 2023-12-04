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

glm::vec3 rotateVector(glm::vec3 v, glm::vec3 angle)
{
	glm::mat4 rot(1.f);
	rot = glm::rotate(rot,glm::radians(angle.x),{1,0,0});
	rot = glm::rotate(rot,glm::radians(angle.y),{0,1,0});
	rot = glm::rotate(rot,glm::radians(angle.z),{0,0,1});

	glm::vec4 pos = {v.x,v.y,v.z,1.f};
	pos = rot*pos;
	v.x = pos.x;
	v.y = pos.y;
	v.z = pos.z;

	return v;
}