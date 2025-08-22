#include "GLTFObject.h"


GLTFObject::GLTFObject(const std::string name, GLTFModel& model) : _position(0)
{
    for (auto& mesh : *model.getMeshes()) {
        GameObject* g = new GameObject(name + "_" + mesh->getName(), *mesh, mesh->getTexture().pixels, mesh->getTexture().width, mesh->getTexture().height);
		g->additionalTransform(mesh->getRecomendedModel());
        _gameObjectsData.push_back(g);
    }

    //_position = model.getCenter();
}

std::vector<GameObject*> GLTFObject::getGameObjectsData()
{
    return _gameObjectsData;
}

void GLTFObject::setPosition(SGEPosition newPosition)
{
	SGEPosition delta = newPosition - _position;
	for (size_t i = 0; i < _gameObjectsData.size(); i++)
		_gameObjectsData[i]->move(delta);
	_position = newPosition;
}

void GLTFObject::move(glm::vec3 dPos)
{
	setPosition(_position + dPos);
}

void GLTFObject::rotate(glm::vec3 begin, glm::vec3 end, float angle)
{
	 for (size_t i = 0; i < _gameObjectsData.size(); i++) {
	 	_gameObjectsData[i]->rotate(begin, end, angle);
	 }

	 glm::vec4 trans(_position.x-begin.x,_position.y-begin.y,_position.z-begin.z,1);
	 glm::mat4 newP(1);
	 newP = glm::rotate(newP, glm::radians(angle), end-begin);
	 trans = newP * trans;
	 _position = {trans.x,trans.y,trans.z};
	 _position += begin;

	 glm::vec3 axis = end - begin;
	 // rotate axis projection on XY around Z to connect with X axis then we got axis on XZ.
	 float alpha = -glm::degrees(atan2f(axis.y,axis.x));
	 // rotate axis in XZ arround Y to connect with Z
	 glm::vec2 axisxy{axis.x,axis.y};
	 float beta = glm::degrees(atan2f(glm::length(axisxy),axis.z));

	 glm::mat4 newR(1);
	 newR = glm::rotate(newR, glm::radians(-alpha),{0,0,1});
	 newR = glm::rotate(newR, glm::radians(beta),  {0,1,0});
	 newR = glm::rotate(newR, glm::radians(angle), {0,0,1});
	 newR = glm::rotate(newR, glm::radians(-beta), {0,1,0});
	 newR = glm::rotate(newR, glm::radians(alpha), {0,0,1});
	 _rotationM = newR * _rotationM;
}

void GLTFObject::rotate(glm::vec3 dAngle)
{	
	 glm::vec3 axis = _position;
	 axis.x += 1;
	 rotate(_position, axis, dAngle.x);

	 axis = _position;
	 axis.y += 1;
	 rotate(_position, axis, dAngle.y);
	
	 axis = _position;
	 axis.z += 1;
	 rotate(_position, axis, dAngle.z);
}

void GLTFObject::setRotation(glm::vec3 angle)
{
	// glm::vec3 backRot = -getRotation();
	// glm::vec3 tempRot = backRot;
	// tempRot.x = 0; tempRot.y = 0;
	// rotate(tempRot);
	// tempRot = backRot;
	// tempRot.x = 0; tempRot.z = 0;
	// rotate(tempRot);
	// tempRot = backRot;
	// tempRot.y = 0; tempRot.z = 0;
	// rotate(tempRot);

	// rotate(angle);
}

void GLTFObject::scale(glm::vec3 sc)
{
	for (size_t i = 0; i < _gameObjectsData.size(); i++) {
		_gameObjectsData[i]->scale(sc);
	}
}

void GLTFObject::scale(float sc)
{
	scale({ sc,sc,sc });
}

glm::vec3 GLTFObject::getPosition()
{
	return _position;
}