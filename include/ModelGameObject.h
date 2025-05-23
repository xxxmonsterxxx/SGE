#pragma once

#include "GameObject.h"

class ModelGameObject : public GameObject {

public:
	
	using GameObject::GameObject;

	ModelGameObject(const std::string name, std::vector<Mesh*>& mesh, std::vector<char*> textures);

protected:

	bool descriptorsUpdate(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer) override;
};