#include "sge_utils.h"

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

void rotateVector(glm::vec3& v, glm::vec3 axis, float angle)
{
	glm::vec4 v4(v.x,v.y,v.z,1);
	glm::mat4 rot(1);
	
	rot = glm::rotate(rot, glm::radians(angle), axis);
	v4 = rot * v4;

	v.x = v4.x;
	v.y = v4.y;
	v.z = v4.z;
}

float wrap360(float ang)
{
	float ret = ang - (int)(ang / 360)*360;
	if (ret < 0)
		ret += 360;
	return ret;
}

float wrap180(float ang)
{
	float ang360 = wrap360(ang);
	if (ang360 > 180)
		return ang360 - 360;
	
	return ang360;
}

glm::vec3 wrap180(glm::vec3 angles)
{
	return {wrap180(angles.x),wrap180(angles.y),wrap180(angles.z)};
}

void printVec(const char* prefix, glm::vec3 v)
{
	printf("\n%s : %f %f %f",prefix,v.x,v.y,v.z);
}