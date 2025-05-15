#include "GameObject.h"
#include "SGE.h"

GameObject::GameObject(std::string name, Mesh& mesh, const std::string& texture) :
        _mesh(mesh) {
			_name = name;
			_mesh.useTexture();
			_texturePath.push_back(texture);

			if (_mesh.isCorrect()) {
				_isMeshCorrect = true;
				_meshStart = _mesh.getTextureBindPoint();
			}
		}

GameObject::GameObject(const std::string name, Mesh& mesh, const char* texture) : 
			GameObject(name, mesh, std::string(texture)) { ; }

GameObject::GameObject(const std::string name, Mesh& mesh, bool textured) :
        _mesh(mesh) {
			_name = name;
			if (_mesh.isCorrect()) {
				_isMeshCorrect = true;
				if (textured) _mesh.useTexture();
			}
		}

GameObject::GameObject(const std::string name, Mesh& mesh, const unsigned char* texture, const uint32_t textureWidth, const uint32_t textureHeight) :
        _mesh(mesh),
		_texturePixels(texture),
		_textureWidth(textureWidth),
		_textureHeight(textureHeight)
{
	_name = name;
	if (_mesh.isCorrect()) {
		_isMeshCorrect = true;
		_mesh.useTexture();
	}
}

GameObject::GameObject(const std::string name, Model& model) : _mesh(model.getMesh()), _texturePath(model.getTextures())
{
	_name = name;
	if (_mesh.isCorrect()) {
		_isMeshCorrect = true;
	}
}

bool GameObject::textureLoading()
{
	if (_texturePixels) {
		SgrImage* newTexture = nullptr;
		if (TextureManager::createFontTextureImage((void*)_texturePixels, _textureWidth, _textureHeight, newTexture) == sgrOK)
			_textures.push_back(newTexture);
		else
			return false;
	}
	else {
		for (auto& p : _texturePath) {
			SgrImage* newTexture = nullptr;
			if (TextureManager::createTextureImage(SGE::resourcesPath + p, newTexture) == sgrOK)
				_textures.push_back(newTexture);
			else
				return false;
		}
	}

	if (_textures.empty()) {
		char emptyPixel = 0;
		SgrImage* emptyTexture = nullptr;
		TextureManager::createTextureImage((void*)&emptyPixel, 1, 1, emptyTexture);
		_textures.push_back(emptyTexture);
	}

	return true;
}

bool GameObject::descriptorsUpdate(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer)
{
	if (!textureLoading())
		return false;

	_descriptorSetData.push_back((void*)(viewProj));
	_descriptorSetData.push_back((void*)(_textures.data()));
	_descriptorSetData.push_back((void*)(allInstancesBuffer));
	return true;
}

bool GameObject::init(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer)
{
	if (!_isMeshCorrect)
		return false;
		
	if (!descriptorsUpdate(viewProj, allInstancesBuffer))
		return false;
	
	if (SGE::renderer.writeDescriptorSets(_name, _descriptorSetData) != sgrOK)
		return false;

	if (SGE::renderer.drawObject(_name) != sgrOK)
		return false;

	return true;
}

void GameObject::setTextureMapping(glm::vec2 deltaTexture, glm::vec2 meshStart, glm::vec2 textureStart)
{
	_deltaTexture = deltaTexture;
	_meshStart = meshStart;
	_textureStart = textureStart;
}

void GameObject::setColor(glm::vec3 newColor)
{
	_color = glm::vec4{newColor.x,newColor.y,newColor.z,1};
}

void GameObject::setColor(SGEColor newColor)
{
	_color = newColor;
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

	// we need if unused forward f.e. emptyTexture destroy this image
	if (_descriptorSetData[1] != ((void*)(&_animationList[newAnimationName].animPixels))) {
		_descriptorSetData[1] = ((void*)(&_animationList[newAnimationName].animPixels));
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