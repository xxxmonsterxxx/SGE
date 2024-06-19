#include "SGE.h"

#if __linux__
#include <unistd.h>
#endif

bool EXIT = false;

// right now simple realisation - later to-do event-subscription mechanism
void terminate(int key, int scancode, int action, int mods)
{
	glfwTerminate();
	EXIT = true;
}

int moveDirection = 1;

void changeDirection(int key, int scancode, int action, int mods)
{
	switch (key)
	{
		case GLFW_KEY_UP:
			moveDirection = 1;
			break;

		case GLFW_KEY_RIGHT:
			moveDirection = 4;
			break;

		case GLFW_KEY_DOWN:
			moveDirection = 3;
			break;

		case GLFW_KEY_LEFT:
			moveDirection = 2;
			break;

		default:
			break;
	}
}

glm::vec2 cameraPos;
bool cameraMove = false;

void moveCamera(int button, int action, int mods)
{
	switch (action) {
		case GLFW_PRESS:
			cameraPos = SGE::get().getCursorPos();
			cameraMove = true;
			break;
		case GLFW_RELEASE:
			cameraMove = false;
			break;
	}
}

bool moveoutcamera = false;
bool moveincamera = false;
void changeCamera(int key, int scancode, int action, int mods)
{
	switch (key) {
		case GLFW_KEY_SPACE:
			if (action == GLFW_PRESS)
				moveoutcamera = true;
			else
				moveoutcamera = false;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			if (action == GLFW_PRESS)
				moveincamera = true;
			else
				moveincamera = false;
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
    const std::vector<glm::vec3> rectVertices{
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
	man.scale(2);
	man.setPosition({-1,0,0});
	man.rotate({0,0,0},{0,0,1},90);
	man.rotate({0,0,90});
	

	glm::vec2 deltaTextureMan;
	deltaTextureMan.x = (0.111 - 0) / (0.5 - -0.5);
	deltaTextureMan.y = (0.250 - 0) / (0.5 - -0.5);
	man.setTextureMapping(deltaTextureMan, glm::vec2(-0.5, -0.5), glm::vec2(0,0));

	// default geometry mesh
    GameObject man2("man2",rectMesh, manTex);
    man2.setPosition({0.5,0.5,3});
	deltaTextureMan.x = (1 - 0) / (0.5 - -0.5);
	deltaTextureMan.y = (1 - 0) / (0.5 - -0.5);
	man2.setTextureMapping(deltaTextureMan, glm::vec2(-0.5, -0.5), glm::vec2(0,0));

	Mesh rectangleFromDefault = Mesh::getDefaultRectangleMesh("defaultRect", false);
	GameObject man3("man3",rectangleFromDefault);
    man3.setPosition({2,-2,-1});
	man3.setColor({0,0,1});

	TextObject helloSGE("Simple Game Engine");
	// move text
	helloSGE.setPosition({-1,0,0});
	helloSGE.rotate({0,0,90});
	helloSGE.rotate({0,30,0});
	helloSGE.rotate({0,-30,0});
	helloSGE.setRotation({0,0,90});
	helloSGE.move({0,1,0});
	helloSGE.rotate({0,0,-90});
	helloSGE.setRotation({0,0,0});
	helloSGE.rotate({0,0,0},{0,0,1},90);
	helloSGE.rotate({0,90,0});
	helloSGE.move({-1,0,0});


	// subscribe to events binded with keys pressing/release
	sgeObject.keyEventSubscribe(GLFW_KEY_ESCAPE, GLFW_RELEASE, terminate);
	sgeObject.keyEventSubscribe(GLFW_KEY_UP, GLFW_PRESS, changeDirection);
	sgeObject.keyEventSubscribe(GLFW_KEY_LEFT, GLFW_PRESS, changeDirection);
	sgeObject.keyEventSubscribe(GLFW_KEY_RIGHT, GLFW_PRESS, changeDirection);
	sgeObject.keyEventSubscribe(GLFW_KEY_DOWN, GLFW_PRESS, changeDirection);
	sgeObject.keyEventSubscribe(GLFW_KEY_SPACE, GLFW_PRESS, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_SPACE, GLFW_RELEASE, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_LEFT_SHIFT, GLFW_RELEASE, changeCamera);
	sgeObject.mouseEventSubscribe(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, moveCamera);
	sgeObject.mouseEventSubscribe(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, moveCamera);

	// render objects
	// sgeObject.setMaxInstanceNumber(4);
	bool ret = false;
    ret = sgeObject.addToRender(man);
	if (!ret)
		return 111;
    ret = sgeObject.addToRender(man2);
	if (!ret)
		return 222;
	ret = sgeObject.addToRender(man3);
	if (!ret)
		return 333;
	sgeObject.addToRender(helloSGE);

    if (!sgeObject.init())
        return 11;

	sgeObject.setViewTransition({0,0,-5});
    
	float mancoordanimation = 0;
    while(sgeObject.drawNextFrame()) {
		if (EXIT)
			return 1;

		if (cameraMove) {
			glm::vec2 deltaCamera = sgeObject.getCursorPos() - cameraPos;
			sgeObject.setViewTransition({0,0,0},deltaCamera.x,glm::vec3(0,1,0));
			sgeObject.setViewTransition({0,0,0},-deltaCamera.y,glm::vec3(1,0,0));
			cameraPos = sgeObject.getCursorPos();
		}

		if (moveoutcamera)
			sgeObject.setViewTransition({0,0,-0.05});
		
		if (moveincamera)
			sgeObject.setViewTransition({0,0,0.05});

		deltaTextureMan.x = (0.111 - 0) / (0.5 - -0.5);
		deltaTextureMan.y = (0.250 - 0) / (0.5 - -0.5);
		man.setTextureMapping(deltaTextureMan, glm::vec2(-0.5, -0.5), glm::vec2(mancoordanimation,(moveDirection - 1)*0.25));
		mancoordanimation += 0.111;
		if (mancoordanimation > 1)
			mancoordanimation = 0;


		deltaTextureMan.x = (1 - 0) / (0.5 - -0.5);
		deltaTextureMan.y = (1 - 0) / (0.5 - -0.5);
		man2.setTextureMapping(deltaTextureMan, glm::vec2(-0.5, -0.5), glm::vec2(mancoordanimation,0));

		helloSGE.rotate({0,1,0});
    }

    return 0;
}