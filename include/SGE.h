#pragma once

#include <SGR/SGR.h>

#include "GameObject.h"

class SGE {

friend class Mesh;
friend class GameObject;

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

    bool buffersInit();

	void updateRenderData();
	static void staticUpdateRenderData();

	SgrBuffer* initGlobalViewMatrix();
	SgrBuffer* initInstancesData();

public:
	static SGE& get() { 
		if (!instance)
			instance = new SGE();

		return *instance;
	}

    bool init(uint16_t width = 800, uint16_t height = 800, std::string windowName = "SGE");

    void addToRender(GameObject& gObj);
    void addToRender(std::vector<GameObject*> gObjects);

	void setKeyCallback(void(*callback)(int,int)) {;}

    void updateViewProj(glm::mat4 newView, glm::mat4 newProj) {;}

	void setViewTransition(glm::vec3 viewTranslate, float angle = 0, glm::vec3 axis = glm::vec3{0,0,1});
    
    bool drawNextFrame();
};