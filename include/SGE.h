#pragma once

#include <SGR/SGR.h>

#include "GameObject.h"
#include "TextObject.h"
#include "EventManager.h"

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

	std::vector<MeshAndObjects> meshesAndObjects;

    SgrGlobalUniformBufferObject viewProjection;

	const uint16_t defaultInstanceNumber = 100;
	SgrInstancesUniformBufferObject instancesData;

	static std::string execPath;

    bool buffersInit();

	void updateRenderData();
	static void staticUpdateRenderData();

	SgrBuffer* initGlobalViewMatrix();
	SgrBuffer* initInstancesData();

	EventManager& eventManager = EventManager::get();

public:
	static SGE& get() { 
		if (!instance)
			instance = new SGE();

		return *instance;
	}

    bool init(uint16_t width = 800, uint16_t height = 800, std::string windowName = "SGE");

    void addToRender(GameObject& gObj);
    void addToRender(std::vector<GameObject*> gObjects);
	void addToRender(TextObject& tObj);

    void updateViewProj(glm::mat4 newView, glm::mat4 newProj) {;}

	void setViewTransition(glm::vec3 viewTranslate, float angle = 0, glm::vec3 axis = glm::vec3{0,0,1});
    
    bool drawNextFrame();

	bool keyEventSubscribe(int key, int action, sgeKeyEventFunction func) { return eventManager.addKeySubscriber(key, action, func); }
	bool mouseEventSubscribe(int key, int action, sgeMouseEventFunction func) { return eventManager.addMouseSubscriber(key, action, func); }

	glm::vec2 getCursorPos();

	std::string getExecPath();
};