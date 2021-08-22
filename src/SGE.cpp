#include "SGE.h"

SGR SGE::renderer = SGR();
SGE* SGE::instance = nullptr;

SGE::SGE(){ }

void SGE::staticUpdateRenderData()
{
	SGE& sge = get();
	sge.updateRenderData();
}

void SGE::updateRenderData()
{
	size_t objCounter = 0;
	for (auto obj : meshesAndObjects) {
		for (auto gObj : obj.gameObjects) {
			Mesh::MeshInstanceData* currentObject = (Mesh::MeshInstanceData*)((uint64_t)instancesData.data + (objCounter++) * instancesData.dynamicAlignment);
			*currentObject = gObj->_instanceData;
		}
	}

	renderer.updateDynamicUniformBuffer(instancesData);
	renderer.updateUniformBuffer(viewProjection);
}

SgrBuffer* SGE::initGlobalViewMatrix()
{
	SgrBuffer* viewProjBuffer = nullptr;
	if (MemoryManager::get()->createUniformBuffer(viewProjBuffer, sizeof(SgrUniformBufferObject)) != sgrOK)
		return nullptr;
	renderer.setupUniformBuffer(viewProjBuffer);

	return viewProjBuffer;
}

SgrBuffer* SGE::initInstancesData()
{
	instancesData.instnaceCount = defaultInstanceNumber;
	instancesData.instanceSize = sizeof(Mesh::MeshInstanceData);
	if (MemoryManager::createDynamicUniformMemory(instancesData) != sgrOK)
		return nullptr;

	SgrBuffer* instancesDataBuffer = nullptr;
	SgrErrCode resultCreateBuffer = MemoryManager::get()->createDynamicUniformBuffer(instancesDataBuffer, instancesData.dataSize);
	if (resultCreateBuffer != sgrOK)
		return nullptr;

	renderer.setupDynamicUniformBuffer(instancesDataBuffer);

	return instancesDataBuffer;
}

bool SGE::init()
{
    SgrErrCode resultSGRInit = renderer.init();
	if (resultSGRInit != sgrOK)
		return false;

	SgrBuffer* viewProjBuffer = initGlobalViewMatrix();
	if (!viewProjBuffer)
		return false;

	SgrBuffer* instancesDataBuffer = initInstancesData();
	if (!instancesDataBuffer)
		return false;

	int objCounter = 0;
	for (size_t i = 0; i < meshesAndObjects.size(); i++) {
		if (!meshesAndObjects[i].mesh->init())
			return false;
		for (size_t j = 0; j < meshesAndObjects[i].gameObjects.size(); j++) {
			if (renderer.addObjectInstance(meshesAndObjects[i].gameObjects[j]->_name,meshesAndObjects[i].mesh->_name,(objCounter++)*instancesData.dynamicAlignment) != sgrOK)
				return false;

			if (!meshesAndObjects[i].gameObjects[j]->init(viewProjBuffer, instancesDataBuffer))
				return false;
		}
	}

	printf("\nInitialized %d instances to draw.",objCounter);

	renderer.setUpdateFunction(staticUpdateRenderData);

    return true;
}

void SGE::addToRender(GameObject& gObj)
{
	for (auto& obj : meshesAndObjects) {
		if(obj.mesh->_name == gObj._mesh._name) {
			obj.gameObjects.push_back(&gObj);
			return;
		}
	}

	MeshAndObjects newMesh;
	newMesh.mesh = &gObj._mesh;
	newMesh.gameObjects.push_back(&gObj);
	meshesAndObjects.push_back(newMesh);
}

bool SGE::drawNextFrame()
{
	if (!renderer.isSGRRunning())
		return false;

	if (renderer.drawFrame() != sgrOK)
		return false;

    return true;
}