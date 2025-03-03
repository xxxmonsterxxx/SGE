#include "ModelGameObject.h"

bool ModelGameObject::descriptorsUpdate(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer)
{
	if (!textureLoading())
		return false;

	_descriptorSetData.push_back((void*)(viewProj));
	_descriptorSetData.push_back((void*)(allInstancesBuffer));
	_descriptorSetData.push_back((void*)(_textures.data()));
	return true;
}