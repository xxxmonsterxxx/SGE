#include "GameObjectBase.h"
#include "CoordinateSystem.h"

Mesh::MeshInstanceData GameObjectBase::getInstanceData()
{
	updateModel();
	return _instanceData; 
}

void GameObjectBase::updateModel()
{
	// WARNING all matrcies transformation actions should be applied in reverse order
	// so this code should be readed from down to up
	glm::mat4 model(1.f);

	if (_cs) { // if this object was binded to certain coordinate system
		model = model * _cs->getTransMatr();
	}

	// operations in coodinate system of this object
	model = model * _positionM; // tranlate
	model = model * _rotationM; // local rotations
	model = model * _scaleM; // local scale

	_instanceData.model = model;
}

SGEPosition GameObjectBase::getPosition()
{
	return {_positionM[3][0],_positionM[3][1],_positionM[3][2]};
}

SGEPosition GameObjectBase::getRotation()
{
	return getAnglesFromRotationMatrx(_rotationM);
}

void GameObjectBase::setPosition(SGEPosition newPosition)
{
	glm::mat4 newPosM(1);
	newPosM = glm::translate(newPosM, newPosition);
	_positionM = newPosM;
	updateModel();
}

void GameObjectBase::move(SGEPosition dPos)
{
	_positionM = glm::translate(_positionM, dPos);
	updateModel();
}

void GameObjectBase::rotate(glm::vec3 begin, glm::vec3 end, float angle)
{
	// move axis to origin
	move(-begin);

	// calculate axis vector
	glm::vec3 axis = end - begin;

	// rotate axis projection on XY around Z to connect with X axis then we got axis on XZ.
	float alpha = -glm::degrees(atan2f(axis.y,axis.x));
	// rotate axis in XZ arround Y to connect with Z
	glm::vec2 axisxy{axis.x,axis.y};
	float beta = glm::degrees(atan2f(glm::length(axisxy),axis.z));

	// order: Z(-alpha),Y(beta),Z(angle),Y(-beta),Z(alpha)

	glm::mat4 newR(1);
    glm::mat4 newP = _positionM;
	newR = glm::rotate(newR, glm::radians(-alpha),{0,0,1});
	newR = glm::rotate(newR, glm::radians(beta),  {0,1,0});
	newR = glm::rotate(newR, glm::radians(angle), {0,0,1});
	newR = glm::rotate(newR, glm::radians(-beta), {0,1,0});
	newR = glm::rotate(newR, glm::radians(alpha), {0,0,1});
	_rotationM = newR * _rotationM;

	newP = newR * _positionM;
    setPosition({newP[3][0],newP[3][1],newP[3][2]}); // i don't know why, but position in PosMatr its last row not last column?????

	// move axis back to begin
	move(begin);
}

void GameObjectBase::rotate(glm::vec3 dAngle)
{
	glm::vec3 axis = getPosition();
	axis.x += 1;
	rotate(getPosition(), axis, dAngle.x);

	axis = getPosition();
	axis.y += 1;
	rotate(getPosition(), axis, dAngle.y);

	axis = getPosition();
	axis.z += 1;
	rotate(getPosition(), axis, dAngle.z);
}

void GameObjectBase::setRotation(glm::vec3 angle)
{
	_rotationM = glm::mat4(1);
	rotate(angle);
}

void GameObjectBase::setScale(glm::vec3 newScale)
{
	glm::mat4 scaleM(1);
	scaleM = glm::scale(scaleM, newScale);
	_scaleM = scaleM;
	updateModel();
}

void GameObjectBase::scale(glm::vec3 sc)
{
	_scaleM = glm::scale(_scaleM, sc);
	updateModel();
}

void GameObjectBase::scale(float sc)
{
	setScale(glm::vec3(sc));
}