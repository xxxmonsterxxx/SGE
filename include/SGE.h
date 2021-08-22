#pragma once

#include <SGR.h>

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

    SgrUniformBufferObject viewProjection;

	const uint16_t defaultInstanceNumber = 100;
	SgrDynamicUniformBufferObject instancesData;

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

    bool init();

    void addToRender(GameObject& gObj);
    void addToRender(std::vector<GameObject*> gObjects);

	void setKeyCallback(void(*callback)(int,int)) {;}

    void updateViewProj(glm::mat4 newView, glm::mat4 newProj) {;}
    
    bool drawNextFrame();
};