#include "SGE.h"

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

    // triangle
    std::vector<SGEPosition> triVertices{
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{0.5f,0.5f,0.f}
	};
    std::vector<uint16_t> triIndices{0,1,2};
    Mesh triMesh("triangle", triVertices, triIndices);

    // create new object
    std::string manTex = "Textures/manTex.png";
    GameObject man("man",rectMesh, manTex);
    man.setPosition(SGEPosition{0.3,0,0});
	man.setTexPosSize(glm::vec2(0.055,0.125),glm::vec2(0.222,0.5));
	man.setScale(0.3);

    GameObject man2("man2",rectMesh, manTex);
    man2.setPosition(SGEPosition{-0.3,0,0});
	man2.setTexPosSize(glm::vec2(0.055,0.125),glm::vec2(0.222,0.5));
	man2.setScale(0.3);

	GameObject man3("man3",rectMesh2, manTex);
    man3.setPosition(SGEPosition{-0.3,0.3,0});
	man3.setTexPosSize(glm::vec2(0.055,0.125),glm::vec2(0.222,0.5));
	man3.setScale(0.3);

	GameObject man4("man4",rectMesh, manTex);
    man4.setPosition(SGEPosition{0.3,0.3,0});
	man4.setTexPosSize(glm::vec2(0.055,0.125),glm::vec2(0.222,0.5));
	man4.setScale(0.3);

	std::string treeTex = "Textures/tree.png";    
    GameObject tree("tree",triMesh, treeTex);
    SGEPosition treeStartPos = {0.5,0,0};
    tree.setPosition(treeStartPos);
	tree.setScale(0.2);

    // sgeObject.setKeyCallback(handleKeys); // ????

    sgeObject.addToRender(man);
	sgeObject.addToRender(tree);
    sgeObject.addToRender(man2);
	sgeObject.addToRender(man3);
	sgeObject.addToRender(man4);
    // sgeObject.addToRender(ball);

    if (!sgeObject.init())
        return 11;
    
    while(sgeObject.drawNextFrame()) {
		
    }

    return 0;
}