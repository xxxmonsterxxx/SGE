#include "GameObject.h"
#include "SGE.h"

GameObject::GameObject(std::string name, Mesh& mesh, const std::string& texture) :
        _mesh(mesh),
		_texturePath(texture) {
			_name = name;
			_mesh.setTextured(true);
		}

GameObject::GameObject(const std::string name, Mesh& mesh, const char* texture) : 
			GameObject(name, mesh, std::string(texture)) { ; }

GameObject::GameObject(const std::string name, Mesh& mesh, bool textured) :
        _mesh(mesh) {
			_name = name;
			_mesh.setTextured(textured);
		}

GameObject::GameObject(const std::string name, Mesh& mesh, const unsigned char* texture, const uint32_t textureWidth, const uint32_t textureHeight) :
        _mesh(mesh),
		_texturePixels(texture),
		_textureWidth(textureWidth),
		_textureHeight(textureHeight)
{
	_name = name;
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
		
	_descriptorSetData.push_back((void*)(viewProj));
	_descriptorSetData.push_back((void*)(_texture));
	_descriptorSetData.push_back((void*)(allInstancesBuffer));

	if (SGE::renderer.writeDescriptorSets(_name, _descriptorSetData) != sgrOK)
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

void GameObject::setColor(SGEColor newColor)
{
	_color = newColor;
	_instanceData.color.x = _color.x;
	_instanceData.color.y = _color.y;
	_instanceData.color.z = _color.z;
}

bool GameObject::addAnimation(const std::string name, AnimationSheet& animSheet, uint8_t animationNumberInVertical)
{
	Animation newAnim;
	if (animSheet.getAnimation(newAnim, animationNumberInVertical)) {
		_animationList[name] = newAnim;

		if (_currentAnimation == "")
			doAnimation(name, 60);

		return true;
	}

	return false;
}

bool GameObject::changeAnimation(std::string newAnimationName)
{
	if (_animationList.find(newAnimationName) == _animationList.end())
		return false;

	if (_descriptorSetData[1] != ((void*)(_animationList[newAnimationName].animPixels))) {
		_descriptorSetData[1] = ((void*)(_animationList[newAnimationName].animPixels));
		if (SGE::renderer.writeDescriptorSets(_name, _descriptorSetData) != sgrOK)
			return false;
	}

	return true;
}

bool GameObject::doAnimation(std::string name, uint8_t speed, int8_t singleFrameMode)
{
	if (_currentAnimation != name) {
		if (changeAnimation(name))
			_currentAnimation = name;
	}

	Animation& curr = _animationList[_currentAnimation];

	curr.speed = speed;
	glm::vec2 frameOffset{0};
	frameOffset.x = curr.frameSize.x;
	if (singleFrameMode > -1 && singleFrameMode <= curr.length)
		curr.frame = singleFrameMode;
	setTextureMapping(curr.frameSize, _mesh.getTextureBindPoint(), curr.startCoord + (float)curr.frame * frameOffset);
	curr.frameCounter++;
	if (curr.frameCounter > curr.speed / curr.length) {
		curr.frame++;
		curr.frameCounter = 0;
		if (curr.frame == curr.length) {
			curr.frame = 0;
			return true;
		}
	}

	return false;
}