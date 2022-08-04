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
    std::string manTex = "Resources/Textures/manTex.png";
    GameObject man("man",rectMesh, manTex);
	// change gameobject parameters
    man.setPosition(SGEPosition{0.6,0.5,-2});
	man.setTexPosSize(glm::vec2(0.055,0.125),glm::vec2(0.222,0.5));
	man.setRotation(glm::vec3(0,0,1),90);

	// default geometry mesh
	Mesh rectangleFromDefault = Mesh::getDefaultRectangleMesh("defaultRect");
    GameObject man2("man2",rectangleFromDefault, manTex);
    man2.setPosition(SGEPosition{-0.3,0,-2});

	GameObject man3("man3",rectMesh);
    man3.setPosition(SGEPosition{0,0,-10});
	man3.setColor(SgeColor{0,0,1});


    // sgeObject.setKeyCallback(handleKeys); // ????

	// render objects
    sgeObject.addToRender(man);
    sgeObject.addToRender(man2);
	sgeObject.addToRender(man3);

    if (!sgeObject.init())
        return 11;

	sgeObject.setViewTransition({0,0,-5});
    
    while(sgeObject.drawNextFrame()) {
    }

    return 0;
}