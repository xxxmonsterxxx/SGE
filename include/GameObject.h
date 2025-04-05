#pragma once

#include "Mesh.h"
#include "GameObjectBase.h"
#include "AnimationSheet.h"
#include "Model.h"
#include <map>

class SGE;

class GameObject : public GameObjectBase {

friend class SGE;

public:
    GameObject(const std::string name, Mesh& mesh, const std::string& texture);
	GameObject(const std::string name, Mesh& mesh, const char* texture);
	GameObject(const std::string name, Mesh& mesh, const unsigned char* texture, uint32_t textureWidth, uint32_t textureHeight);
    GameObject(const std::string name, Mesh& mesh, bool textured = false);
	GameObject(const std::string name, Model& model);

	void setTextureMapping(glm::vec2 deltaTexture, glm::vec2 meshStart, glm::vec2 textureStart);
	void setColor(glm::vec3 newColor);
	void setColor(SGEColor newColor);

	bool addAnimation(const std::string name, AnimationSheet& animSheet, uint8_t animationNumberInVertical);
	bool doAnimation(std::string name, uint8_t speed, int8_t singleFrameMode = -1);

	glm::mat4 getModel() const { return _model; }
	glm::vec2 getDeltaTexture() const { return _deltaTexture; }
	glm::vec2 getMeshStart() const { return _meshStart; }
	glm::vec2 getTextureStart() const { return _textureStart; }
	glm::vec4 getColor() const { return _color; }

private:
	std::vector<std::string> _texturePath{"/Textures/empty.jpg"};
	
	const unsigned char* _texturePixels = nullptr;
	const uint32_t _textureWidth = 0;
	const uint32_t _textureHeight = 0;

    SGEColor _color;

	glm::vec2 _deltaTexture{1,1};
	glm::vec2 _meshStart;
	glm::vec2 _textureStart{0,0};

    Mesh& _mesh;

    bool init(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer);

	bool changeAnimation(std::string newAnimationName);
	std::string _currentAnimation = "";
	std::map<std::string, Animation> _animationList;

protected:

	std::vector<void*> _descriptorSetData;
	std::vector<SgrImage*> _textures;
	bool textureLoading();
	virtual bool descriptorsUpdate(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer);
};