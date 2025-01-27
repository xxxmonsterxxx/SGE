#pragma once

#include "Mesh.h"
#include "GameObjectBase.h"
#include "AnimationSheet.h"
#include <map>

class SGE;

class GameObject : public GameObjectBase {

friend class SGE;

public:
    GameObject(const std::string name, Mesh& mesh, const std::string& texture);
	GameObject(const std::string name, Mesh& mesh, const char* texture);
	GameObject(const std::string name, Mesh& mesh, const unsigned char* texture, uint32_t textureWidth, uint32_t textureHeight);
    GameObject(const std::string name, Mesh& mesh, bool textured = false);

	void setTextureMapping(glm::vec2 deltaTexture, glm::vec2 meshStart, glm::vec2 textureStart);
	void setColor(SGEColor newColor);

	bool addAnimation(const std::string name, AnimationSheet& animSheet, uint8_t animationNumberInVertical);
	bool doAnimation(std::string name, uint8_t speed, int8_t singleFrameMode = -1);

private:
	std::string _texturePath = "/Resources/Textures/empty.jpg";
	
	const unsigned char* _texturePixels = nullptr;
	const uint32_t _textureWidth = 0;
	const uint32_t _textureHeight = 0;

    SGEColor _color;

    Mesh& _mesh;

    bool init(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer);

	bool changeAnimation(std::string newAnimationName);
	std::string _currentAnimation = "";
	std::map<std::string, Animation> _animationList;

	std::vector<void*> _descriptorSetData;
};