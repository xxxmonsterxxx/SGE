#include "SGE.h"


SGR SGE::renderer = SGR();
SGE* SGE::instance = nullptr;
std::string SGE::execPath = getExecutablePath();
std::string SGE::resourcesPath = execPath;

const uint16_t maximumInstanceNumber = 1000;

SGE::SGE(){ }

SGE& SGE::get() {
	if (!instance)
		instance = new SGE();

	return *instance;
}

void SGE::staticUpdateRenderData()
{
	SGE& sge = get();
	sge.updateRenderData();
}

void SGE::updateRenderData()
{
	for (auto& obj : meshesAndObjects) {
		size_t objCounter = 0;
		for (auto& gObj : obj.gameObjects) {
			uint8_t* objData = (uint8_t*)((uint64_t)obj.instancesData.data + (objCounter++) * obj.instancesData.dynamicAlignment);
			void* generatedData = malloc(obj.instancesData.dynamicAlignment);
			if (generatedData) {
				obj.mesh->generateInstanceData(gObj, generatedData);
				memcpy((void*)objData, generatedData, obj.instancesData.dynamicAlignment);
			}
		}

		renderer.updateInstancesUniformBufferObject(obj.instancesData);
	}

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

bool SGE::initInstancesData()
{
	for (auto& mao : meshesAndObjects) {
		mao.instancesData.instnaceCount = requiredInstanceNumber;
		mao.instancesData.instanceSize = mao.mesh->getInstanceDataSize();
		if (MemoryManager::createDynamicUniformMemory(mao.instancesData) != sgrOK)
			return false;

		SgrErrCode resultCreateBuffer = MemoryManager::get()->createDynamicUniformBuffer(mao.instancesData.ubo, mao.instancesData.dataSize, mao.instancesData.dynamicAlignment);
		if (resultCreateBuffer != sgrOK)
			return false;

	}


	return true;
}

bool SGE::init(uint16_t width, uint16_t height, std::string windowName)
{
	renderer.enableDebugMode();

    SgrErrCode resultSGRInit = renderer.init(width, height, windowName.c_str());
	if (resultSGRInit != sgrOK)
		return false;

	SgrBuffer* viewProjBuffer = initGlobalViewMatrix();
	if (!viewProjBuffer)
		return false;

	viewProjection.view = glm::translate(viewProjection.view, glm::vec3(0, 0, -0.1));
	viewProjection.proj = glm::perspective(45.f, 1.f/1.f, 0.1f, 100.f); // 0.1-min 100-max // axis is inverted

	if (!initInstancesData())
		return false;

	for (size_t i = 0; i < meshesAndObjects.size(); i++) {
		if (!meshesAndObjects[i].mesh->init())
			return false;
		int objCounter = 0;
		for (size_t j = 0; j < meshesAndObjects[i].gameObjects.size(); j++) {
			if (renderer.addObjectInstance(meshesAndObjects[i].gameObjects[j]->_name,meshesAndObjects[i].mesh->_name,(objCounter++)* meshesAndObjects[i].instancesData.dynamicAlignment) != sgrOK)
				return false;

			if (!meshesAndObjects[i].gameObjects[j]->init(viewProjBuffer, meshesAndObjects[i].instancesData.ubo))
				return false;
		}
	}

	fpsInfo = new UIText("FpsInfo", {0.09,0.09}, {50,50}, "FPS: ");
	registerUIObject(*fpsInfo);

	for (auto uiElem : uiObjects)
		renderer.drawUIElement(*uiElem->getUIPtr());

	renderer.setUpdateFunction(staticUpdateRenderData);

	GLFWwindow* window;
	if (renderer.getWindow(window) != sgrOK) {
		return false;
	}

	eventManager.init(window);

	physEng.init(&physObjects);
	physEng.start();

	// this method should be called in the end of all users callback declaration
	renderer.setupUICallback();

	currentFrame = 0;
	lastDrawTime = SgrTime::now();

    return true;
}

bool SGE::addToRender(GameObject& gObj)
{
	if (totalInstanceNumber+1 > requiredInstanceNumber) {
		return false;
	}

	gObj.bindCoordSystem(&cs); // to each game object we bind SGE coordinate system which binded to camera

	for (auto& obj : meshesAndObjects) {
		if(obj.mesh->_name == gObj._mesh._name) {
			obj.gameObjects.push_back(&gObj);
			totalInstanceNumber++;
			return true;
		}
	}

	MeshAndObjects newMesh;
	newMesh.mesh = &gObj._mesh;
	totalInstanceNumber++;
	newMesh.gameObjects.push_back(&gObj);
	meshesAndObjects.push_back(newMesh);
	return true;
}

bool SGE::drawNextFrame()
{
	if (!renderer.isSGRRunning())
		return false;

	physEng.update();

	if (renderer.drawFrame() != sgrOK)
		return false;

	currentFrame++;
	if (getTimeDuration(lastDrawTime, SgrTime::now()) > 1) {
		std::string fpsText = "FPS: " + std::to_string(currentFrame);
		fpsInfo->changeText(fpsText);
		lastDrawTime = SgrTime::now();
		currentFrame = 0;
	}

    return true;
}

void SGE::setViewTransition(glm::vec3 viewTranslate, float angle, glm::vec3 axis)
{
	viewProjection.view = glm::translate(viewProjection.view, viewTranslate);
	viewProjection.view = glm::rotate(viewProjection.view, glm::radians(angle), axis);
}

bool SGE::addToRender(std::vector<GameObject*> gObjects)
{
	for (auto newGObj : gObjects) {
		bool ret = addToRender(*newGObj);
		if (!ret)
			return false;
	}

	return true;
}

bool SGE::addToRender(TextObject& tObj)
{
	return addToRender(tObj.getGameObjectsData());
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

bool SGE::setMaxInstanceNumber(uint16_t number)
{
	if (number > maximumInstanceNumber)
		return false;

	requiredInstanceNumber = number;
	return true;
}

bool SGE::registerGameObject(GameObject& gObj)
{
	gameObjects.push_back(&gObj);
	physObjects.push_back(&gObj);
	return addToRender(gObj);
}

bool SGE::registerGameObject(TextObject& tObj)
{
	gameObjects.push_back(&tObj);
	physObjects.push_back(&tObj);
	return addToRender(tObj);
}

bool SGE::registerUIObject(UIObject& uiObject)
{
	uiObjects.push_back(&uiObject);
	return true;
}

bool SGE::setGameAppLogo(std::string path)
{
	if (renderer.setApplicationLogo(resourcesPath + path) != sgrOK)
 		return false;

	return true;
}

void SGE::setResourcesPath(std::string path)
{
	resourcesPath = path;
}