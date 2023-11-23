#include "SGE.h"

SGR SGE::renderer = SGR();
SGE* SGE::instance = nullptr;
std::string SGE::execPath = getExecutablePath();

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
	
	renderer.updateInstancesUniformBufferObject(instancesData);
	renderer.updateGlobalUniformBufferObject(viewProjection);
}

SgrBuffer* SGE::initGlobalViewMatrix()
{
	SgrBuffer* viewProjBuffer = nullptr;
	if (MemoryManager::get()->createUniformBuffer(viewProjBuffer, sizeof(SgrGlobalUniformBufferObject)) != sgrOK)
		return nullptr;
	renderer.setupGlobalUniformBufferObject(viewProjBuffer);

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

	renderer.setupInstancesUniformBufferObject(instancesDataBuffer);

	return instancesDataBuffer;
}

bool SGE::init(uint16_t width, uint16_t height, std::string windowName)
{
    SgrErrCode resultSGRInit = renderer.init(width, height, windowName.c_str());
	if (resultSGRInit != sgrOK)
		return false;

	SgrBuffer* viewProjBuffer = initGlobalViewMatrix();
	if (!viewProjBuffer)
		return false;

	viewProjection.view = glm::translate(viewProjection.view, glm::vec3(0, 0, -0.1));
	viewProjection.proj = glm::perspective(45.f, 1.f/1.f, 0.1f, 100.f); // 0.1-min 100-max // axis is inverted

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

	renderer.setUpdateFunction(staticUpdateRenderData);

	GLFWwindow* window;
	if (renderer.getWindow(window) != sgrOK) {
		return false;
	}

	eventManager.init(window);

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

void SGE::setViewTransition(glm::vec3 viewTranslate, float angle, glm::vec3 axis)
{
	viewProjection.view = glm::translate(viewProjection.view, viewTranslate);
	viewProjection.view = glm::rotate(viewProjection.view, glm::radians(angle), axis);
}

void SGE::addToRender(std::vector<GameObject*> gObjects)
{
	for (auto newGObj : gObjects)
		addToRender(*newGObj);
}

void SGE::addToRender(TextObject& tObj)
{
	addToRender(tObj.getGameObjectsData());
}

glm::vec2 SGE::getCursorPos()
{
	GLFWwindow* window;
	renderer.getWindow(window);
	double x,y;
	glfwGetCursorPos(window, &x, &y);

	return glm::vec2(x,y);
}

std::string SGE::getExecPath()
{
	return execPath;
}