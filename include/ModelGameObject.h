#pragma once

#include "GameObject.h"

class ModelGameObject : public GameObject {

public:
	
	using GameObject::GameObject;

protected:

	bool descriptorsUpdate(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer) override;
};