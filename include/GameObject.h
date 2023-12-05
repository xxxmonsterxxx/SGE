#pragma once

#include "Mesh.h"
#include "iGameObject.h"

class SGE;

class GameObject : iGameObject {

friend class SGE;

public:
    GameObject(const std::string name, Mesh& mesh, const std::string& texture);
	GameObject(const std::string name, Mesh& mesh, const unsigned char* texture, uint32_t textureWidth, uint32_t textureHeight);
    GameObject(const std::string name, Mesh& mesh);

    void setPosition(SGEPosition newPosition) override;
	void move(SGEPosition dPos) override;
	SGEPosition getPosition();

	void setRotation(glm::vec3 angle) override;
	void rotate(glm::vec3 dAngle, bool global = false) override;

	void setTextureMapping(glm::vec2 deltaTexture, glm::vec2 meshStart, glm::vec2 textureStart) override;
    void setScale(glm::vec3 newScale) override;
	void setScale(float newScale);
	void setColor(SGEColor newColor) override;

private:
	std::string _name;

    glm::vec3 _scale = glm::vec3(1);

	std::string _texturePath = "/Resources/Textures/empty.jpg";
	
	const unsigned char* _texturePixels = nullptr;
	const uint32_t _textureWidth = 0;
	const uint32_t _textureHeight = 0;

    SGEColor _color;

    Mesh& _mesh;

	Mesh::MeshInstanceData _instanceData;

    bool init(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer);

	void updateModel();
};