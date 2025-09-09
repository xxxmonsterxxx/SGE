#pragma once

#include "GameObject.h"
#include "ObjModel.h"

class ObjModelObject : public GameObject {

public:

	ObjModelObject(const std::string name, ObjModel& model);

protected:

	bool descriptorsUpdate(SgrBuffer* viewProj, SgrBuffer* allInstancesBuffer) override;
};