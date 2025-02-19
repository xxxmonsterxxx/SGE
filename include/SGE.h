#pragma once

#include <SGR.h>

#include "GameObject.h"
#include "TextObject.h"
#include "EventManager.h"
#include "CameraObject.h"
#include "CoordinateSystem.h"
#include "UIObject.h"

class SGE {

friend class Mesh;
friend class GameObject;
friend class TextObject;

private:

	static SGE* instance;

	SGE();
	
	struct MeshAndObjects {
		Mesh* mesh;
		std::vector<GameObject*> gameObjects;
	};

    static SGR renderer;

	std::vector<GameObjectBase*> gameObjects;
	std::vector<PhysicsObject*> physObjects;
	std::vector<MeshAndObjects> meshesAndObjects;
	std::vector<UIObject*> uiObjects;

    SgrGlobalUniformBufferObject viewProjection;

	const uint16_t defaultInstanceNumber = 100;
	size_t requiredInstanceNumber = defaultInstanceNumber;
	size_t totalInstanceNumber = 0;
	SgrInstancesUniformBufferObject instancesData;

	static std::string execPath;

    bool buffersInit();

	void updateRenderData();
	static void staticUpdateRenderData();

	SgrBuffer* initGlobalViewMatrix();
	bool initInstancesData();

	EventManager& eventManager = EventManager::get();
	CoordinateSystem cs{glm::vec3{0,0,0},glm::vec3{180,0,0}};
	CameraObject camera{&cs};
	PhysicsEngine& physEng = PhysicsEngine::get();

	bool addToRender(GameObject& gObj);
    bool addToRender(std::vector<GameObject*> gObjects);
	bool addToRender(TextObject& tObj);

	uint8_t currentFrame;
	SgrTime_t lastDrawTime;

	UIText* fpsInfo;

public:
	static SGE& get();

    bool init(uint16_t width = 800, uint16_t height = 800, std::string windowName = "SGE");

	bool setMaxInstanceNumber(uint16_t number);

	bool registerGameObject(GameObject& gObj);
	bool registerGameObject(TextObject& tObj);
	bool registerUIObject(UIObject& uiObject);

	size_t getRenderInstanceNumber() { return totalInstanceNumber; }

    void updateViewProj(glm::mat4 newView, glm::mat4 newProj) {;}

	void setViewTransition(glm::vec3 viewTranslate, float angle = 0, glm::vec3 axis = glm::vec3{0,0,1});
    
    bool drawNextFrame();

	bool keyEventSubscribe(int key, int action, sgeKeyEventFunction func) { return eventManager.addKeySubscriber(key, action, func); }
	bool mouseEventSubscribe(int key, int action, sgeMouseEventFunction func) { return eventManager.addMouseSubscriber(key, action, func); }

	glm::vec2 getCursorPos();

	static std::string getExecPath();

	CameraObject& getCameraObject() { return camera; }

	bool setGameAppLogo(std::string path);
};