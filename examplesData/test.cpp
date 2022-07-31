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

    // create new mesh

    // rectangle
    const std::vector<SGEPosition> rectVertices{
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{0.5f,0.5f,0.f},
		{-0.5f,0.5f,0.f}
	};
    const std::vector<uint16_t> rectIndices{0,1,2,2,3,0};
    Mesh rectMesh("rectangle", rectVertices, rectIndices);
	Mesh rectMesh2("rectangle2", rectVertices, rectIndices);
	Mesh rectMesh3("rectangle3", rectVertices, rectIndices);

    // triangle
    std::vector<SGEPosition> triVertices{
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{0.5f,0.5f,0.f}
	};
    std::vector<uint16_t> triIndices{0,1,2};
    Mesh triMesh("triangle", triVertices, triIndices, false);

    // create new object
    std::string manTex = "Resources/Textures/manTex.png";
    GameObject man("man",rectMesh, manTex);
    man.setPosition(SGEPosition{0.6,0.5,-2});
	man.setTexPosSize(glm::vec2(0.055,0.125),glm::vec2(0.222,0.5));
	man.setRotation(glm::vec3(0,0,1),90);

    GameObject man2("man2",rectMesh2, manTex);
    man2.setPosition(SGEPosition{-0.3,0,-2});

	GameObject man3("man3",rectMesh3);
    man3.setPosition(SGEPosition{0,0,-10});
	man3.setColor(SgeColor{0,0,1});


    // sgeObject.setKeyCallback(handleKeys); // ????

    sgeObject.addToRender(man);
    sgeObject.addToRender(man2);
	sgeObject.addToRender(man3);

    if (!sgeObject.init())
        return 11;

	sgeObject.setViewTransition({0,0,-0.1},10);
    
    while(sgeObject.drawNextFrame()) {
    }

    return 0;
}