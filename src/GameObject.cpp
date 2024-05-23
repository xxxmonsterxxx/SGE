#include "GameObject.h"
#include "SGE.h"

GameObject::GameObject(std::string name, Mesh& mesh, const std::string& texture) :
		_name(name),
        _mesh(mesh),
		_texturePath(texture) {
			_mesh.setTextured(true);
		}

GameObject::GameObject(const std::string name, Mesh& mesh) :
		_name(name),
        _mesh(mesh) {
			_mesh.setTextured(false);
		}

GameObject::GameObject(const std::string name, Mesh& mesh, const unsigned char* texture, const uint32_t textureWidth, const uint32_t textureHeight) :
		_name(name),
        _mesh(mesh),
		_texturePixels(texture),
		_textureWidth(textureWidth),
		_textureHeight(textureHeight)
{
	_mesh.setTextured(true);
}

bool GameObject::init(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer)
{
	SgrImage* _texture = nullptr;
	SgrErrCode resultCreateTextureImage;
	if (_texturePixels)
		resultCreateTextureImage = TextureManager::createFontTextureImage((void*)_texturePixels, _textureWidth, _textureHeight , _texture);
	else
		resultCreateTextureImage = TextureManager::createTextureImage(SGE::execPath + _texturePath, _texture);

	if (resultCreateTextureImage != sgrOK)
		return false;
		
	std::vector<void*> instanceData;
	instanceData.push_back((void*)(viewProj));
	instanceData.push_back((void*)(_texture));
	instanceData.push_back((void*)(allInstancesBuffer));

	if (SGE::renderer.writeDescriptorSets(_name, instanceData) != sgrOK)
		return false;

	if (SGE::renderer.drawObject(_name) != sgrOK)
		return false;

	return true;
}

void GameObject::setTextureMapping(glm::vec2 deltaTexture, glm::vec2 meshStart, glm::vec2 textureStart)
{
	_instanceData.meshToTextureDelta = deltaTexture;
	_instanceData.meshStart = meshStart;
	_instanceData.textureStart = textureStart;
}

SGEPosition GameObject::getPosition()
{
	return _position;
}

void GameObject::setPosition(SGEPosition newPosition)
{
	_position = newPosition;
	updateModel();
}

void GameObject::move(SGEPosition dPos)
{
	_position += dPos;
	setPosition(_position);
}

void GameObject::setScale(glm::vec3 newScale)
{
	_scale = newScale;
	updateModel();
}

void GameObject::setScale(float newScale)
{
	setScale(glm::vec3(newScale));
}

void GameObject::setColor(SGEColor newColor)
{
	_color = newColor;
	_instanceData.color.x = _color.x;
	_instanceData.color.y = _color.y;
	_instanceData.color.z = _color.z;
}

void GameObject::setRotation(glm::vec3 angle)
{
	_rotation = angle;

	updateModel();
}

void GameObject::rotate(glm::vec3 dAngle, bool global)
{
	_rotation += dAngle;

	if (global) {
		_position = rotateVector(_position, dAngle);
	}

	setRotation(_rotation);
}

void GameObject::rotate(glm::vec3 begin, glm::vec3 end, float angle)
{
	// move axis to origin
	move(-begin);

	// calculate axis vector
	glm::vec3 axis = end - begin;

	// rotate axis projection on XY around Z to connect with X axis then we got axis on XZ.
	float alpha = atan2f(axis.y,axis.x);
	rotate({0,0,-alpha},true);

	// rotate axis in XZ arround Y to connect with Z
	glm::vec2 axisxy(axis.x,axis.y);
	float beta = atan2f(axisxy.length(),axis.z);
	rotate({0,beta,0},true);

	// now we can rotate around Z
	rotate({0,0,angle},true);

	// backward rotating
	rotate({0,-beta,0},true);
	rotate({0,0,-alpha},true);

	// move axis back to begin
	move(begin);
}

void GameObject::updateModel()
{
	glm::mat4 model(1.f);

	model = glm::translate(model, _position);

	model = glm::rotate(model, glm::radians(_rotation.x), {1,0,0});
	model = glm::rotate(model, glm::radians(_rotation.y), {0,1,0});
	model = glm::rotate(model, glm::radians(_rotation.z), {0,0,1});

	model = glm::scale(model, _scale);

	_instanceData.model = model;
}