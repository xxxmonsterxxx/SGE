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

void GameObject::setPosition(SGEPosition newPosition)
{
	_position = newPosition;
	_instanceData.model = glm::translate(_instanceData.model, glm::vec3(_position.x,_position.y,_position.z));
}

void GameObject::setScale(glm::vec3 newScale)
{
	_scale = newScale; 
	_instanceData.model = glm::scale(_instanceData.model, _scale);
}

void GameObject::setColor(SgeColor newColor)
{
	_color = newColor;
	_instanceData.color.x = _color.x;
	_instanceData.color.y = _color.y;
	_instanceData.color.z = _color.z;
}

void GameObject::setRotation(glm::vec3 axis, float angle)
{
	_instanceData.model = glm::rotate(_instanceData.model, glm::radians(angle), axis);
}
