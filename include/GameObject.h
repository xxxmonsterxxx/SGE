#pragma once

#include "Mesh.h"
#include "GameObjectBase.h"

class SGE;

class GameObject : public GameObjectBase {

friend class SGE;

public:
    GameObject(const std::string name, Mesh& mesh, const std::string& texture);
	GameObject(const std::string name, Mesh& mesh, const unsigned char* texture, uint32_t textureWidth, uint32_t textureHeight);
    GameObject(const std::string name, Mesh& mesh);

	void setTextureMapping(glm::vec2 deltaTexture, glm::vec2 meshStart, glm::vec2 textureStart);
	void setColor(SGEColor newColor);

private:
	std::string _texturePath = "/Resources/Textures/empty.jpg";
	
	const unsigned char* _texturePixels = nullptr;
	const uint32_t _textureWidth = 0;
	const uint32_t _textureHeight = 0;

    SGEColor _color;

    Mesh& _mesh;

    bool init(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer);
};