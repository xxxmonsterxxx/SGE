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

bool GameObject::init(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer)
{
	SgrImage* _texture = nullptr;
	SgrErrCode resultCreateTextureImage = TextureManager::createTextureImage(_texturePath, _texture);
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

void GameObject::setTexPosSize(glm::vec2 texPos, glm::vec2 texSize)
{
	_instanceData.texCoord = texPos;
	_instanceData.texSize = texSize;
}

void GameObject::setPosition(SGEPosition newPosition)
{
	_position = newPosition;
	_instanceData.model = glm::translate(_instanceData.model, glm::vec3(_position.x,_position.y,_position.z));
}

void GameObject::setScale(float newScale)
{
	_scale = newScale; 
	_instanceData.model = glm::scale(_instanceData.model, glm::vec3(_scale,_scale,_scale));
}

void GameObject::setColor(SgeColor newColor)
{
	_color = newColor;
	_instanceData.color = _color;
}

void GameObject::setRotation(glm::vec3 axis, float angle)
{
	_instanceData.model = glm::rotate(_instanceData.model, glm::radians(angle), axis);
}
