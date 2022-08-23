#include "SGE.h"
#include <unistd.h>

enum SGEKeys {
	up,
	down,
	left,
	right
};

void handleKeys(int key, int type)
{
    switch(key) {
        case up:
			break;
    }
}

int main()
{
    SGE& sgeObject = SGE::get();

    // steps
	// 1. Create MESH
	// 2. Create GameObject
	// 3. Change parameters of GameObjects
	// 4. Render GameObject

    // we can create own geometry mesh
    const std::vector<SGEPosition> rectVertices{
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{0.5f,0.5f,0.f},
		{-0.5f,0.5f,0.f}
	};
    const std::vector<uint16_t> rectIndices{0,1,2,2,3,0};
	Mesh rectMesh("rectangle", rectVertices, rectIndices);

    // create new object
    std::string manTex = "/Resources/Textures/manTex.png";
    GameObject man("man",rectMesh, manTex);
	// change gameobject parameters
    man.setPosition(SGEPosition{-2,-2,-1});
	glm::vec2 deltaTextureMan;
	deltaTextureMan.x = (0.111 - 0) / (0.5 - -0.5);
	deltaTextureMan.y = (0.250 - 0) / (0.5 - -0.5);
	man.setTextureMapping(deltaTextureMan, glm::vec2(-0.5, -0.5), glm::vec2(0,0));
	// man.setRotation(glm::vec3(0,0,1),90);

	// default geometry mesh
    GameObject man2("man2",rectMesh, manTex);
    man2.setPosition(SGEPosition{0.5,0.5,3});
	deltaTextureMan.x = (1 - 0) / (0.5 - -0.5);
	deltaTextureMan.y = (1 - 0) / (0.5 - -0.5);
	man2.setTextureMapping(deltaTextureMan, glm::vec2(-0.5, -0.5), glm::vec2(0,0));

	Mesh rectangleFromDefault = Mesh::getDefaultRectangleMesh("defaultRect", false);
	GameObject man3("man3",rectangleFromDefault);
    man3.setPosition(SGEPosition{2,-2,-1});
	man3.setColor(SgeColor{0,0,1});

	TextObject helloSGE("Simple Game Engine");
	helloSGE.setPosition(SGEPosition{-0.75,0,0});
	helloSGE.setScale(glm::vec3{1,1,1});

    // sgeObject.setKeyCallback(handlesKeys); // ????

	// render objects
    sgeObject.addToRender(man);
    sgeObject.addToRender(man2);
	sgeObject.addToRender(man3);
	sgeObject.addToRender(helloSGE);

    if (!sgeObject.init())
        return 11;

	sgeObject.setViewTransition({0,0,-5});
    
	float mancoordanimation = 0;
	float animationfire = 0.024;
    while(sgeObject.drawNextFrame()) {
		helloSGE.setRotation(glm::vec3{0,1,0}, 0.3); // move text

		deltaTextureMan.x = (0.111 - 0) / (0.5 - -0.5);
		deltaTextureMan.y = (0.250 - 0) / (0.5 - -0.5);
		man.setTextureMapping(deltaTextureMan, glm::vec2(-0.5, -0.5), glm::vec2(mancoordanimation,0));
		mancoordanimation += 0.111;
		if (mancoordanimation > 1)
			mancoordanimation = 0;


		deltaTextureMan.x = (1 - 0) / (0.5 - -0.5);
		deltaTextureMan.y = (1 - 0) / (0.5 - -0.5);
		man2.setTextureMapping(deltaTextureMan, glm::vec2(-0.5, -0.5), glm::vec2(mancoordanimation,0));
    }

    return 0;
}