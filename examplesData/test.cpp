#include "SGE.h"

#if __linux__
#include <unistd.h>
#endif

bool EXIT = false;

// right now simple realisation - later to-do event-subscription mechanism
void terminate(int key, int scancode, int action, int mods)
{
	EXIT = true;
}

void exitFunction()
{
	terminate(0,0,0,0);
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

int soldierAction = 1;

void soldierMove(int key, int scancode, int action, int mods)
{
	switch (key)
	{
		case GLFW_KEY_I:
			soldierAction = 1;
			break;

		case GLFW_KEY_L:
			soldierAction = 2;
			break;

		case GLFW_KEY_ENTER:
			soldierAction = 3;
			break;

		case GLFW_KEY_K:
			soldierAction = 4;
			break;

		default:
			break;
	}
}

glm::vec2 cameraPos;
bool cameraRotate = false;

void moveCamera(int button, int action, int mods)
{
	switch (action) {
		case GLFW_PRESS:
			cameraPos = SGE::get().getCursorPos();
			cameraRotate = true;
			break;
		case GLFW_RELEASE:
			cameraRotate = false;
			break;
	}
}

int moveCameraZ = 0; // -1 - Forward, 1 - Backward
int moveCameraX = 0; // -1 - Left, 1 - Right
bool cameraReset = false;
void changeCamera(int key, int scancode, int action, int mods)
{
	switch (key) {
		case GLFW_KEY_S:
			if (action == GLFW_PRESS)
				moveCameraZ = 1;
			else
				moveCameraZ = 0;
			break;
		case GLFW_KEY_W:
			if (action == GLFW_PRESS)
				moveCameraZ = -1;
			else
				moveCameraZ = 0;
			break;
		case GLFW_KEY_A:
			if (action == GLFW_PRESS)
				moveCameraX = -1;
			else
				moveCameraX = 0;
			break;
		case GLFW_KEY_D:
			if (action == GLFW_PRESS)
				moveCameraX = 1;
			else
				moveCameraX = 0;
			break;
		case GLFW_KEY_SPACE:
			if (action == GLFW_PRESS)
				cameraReset = true;
	}
}

int main()
{
    SGE& sgeObject = SGE::get();

#if __APPLE__ && !NDBUG
	sgeObject.setResourcesPath(SGE::getExecPath()+"/../Resources");
#else
	sgeObject.setResourcesPath(SGE::getExecPath()+"/Resources");
#endif

    // steps
	// 1. Create MESH
	// 2. Create GameObject
	// 3. Change parameters of GameObjects
	// 4. Render GameObject

	// since ver 0.5.0 coordinate system of SGE is: Y - up, Z - forward
    // we can create own geometry mesh
    const std::vector<glm::vec3> rectVertices{
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{0.5f,0.5f,0.f},
		{-0.5f,0.5f,0.f}
	};
    const std::vector<uint32_t> rectIndices{0,1,2,2,3,0};
	Mesh rectMesh("rectangle", rectVertices, rectIndices);

    // create new object
    GameObject man("man",rectMesh,true);
	// change gameobject parameters
	man.scale(2);
	man.rotate({180,0,0});
	AnimationSheet torchMan("TorchMan", "/Textures/manTex.png", 9, 4);

	// default geometry mesh
    GameObject man2("soldier",rectMesh,true);
	man2.scale(3);
	man2.move({2,0.5,0});
	man2.rotate({180,0,0});
	AnimationSheet soldIdle("Soldier idle", "/Textures/Soldier/Idle.png", 7, 1);
	AnimationSheet soldRun("Soldier run", "/Textures/Soldier/Run.png", 8, 1);
	AnimationSheet soldShot("Soldier shot", "/Textures/Soldier/Shot_2.png", 4, 1);
	AnimationSheet soldRech("Soldier recharge", "/Textures/Soldier/Recharge.png", 13, 1);

	Mesh rectangleFromDefault = Mesh::getDefaultRectangleMesh("defaultRect", false);
	GameObject man3("man3",rectangleFromDefault,false);
    man3.setPosition({1.5,-1.5,-1});
	man3.setColor({0,0,1});

	TextObject helloSGE("Simple Game Engine");
	// move text
	helloSGE.setPosition({-1.5,0,0});
	helloSGE.rotate({0,0,-90});
	helloSGE.setRotation({0,0,0});
	helloSGE.rotate({0,0,0},{0,0,1},90);
	helloSGE.move({-1,0,0});

	Model halcon("Halcon", "/3d_models/Halcon_Milenario", Model::ModelType::OBJ);
	ModelGameObject ship("Ship", halcon);
	ship.move({-1.5,0,0});
	ship.rotate({-10,0,0});

	Model audi("Audi", "/3d_models/Audi_S5_Sportback", Model::ModelType::OBJ);
	ModelGameObject car("Car", audi);
	car.move({1,1,0});
	car.rotate({-20,0,0});


	// subscribe to events binded with keys pressing/release
	sgeObject.keyEventSubscribe(GLFW_KEY_ESCAPE, GLFW_RELEASE, terminate);
	sgeObject.keyEventSubscribe(GLFW_KEY_UP, GLFW_PRESS, changeDirection);
	sgeObject.keyEventSubscribe(GLFW_KEY_LEFT, GLFW_PRESS, changeDirection);
	sgeObject.keyEventSubscribe(GLFW_KEY_RIGHT, GLFW_PRESS, changeDirection);
	sgeObject.keyEventSubscribe(GLFW_KEY_DOWN, GLFW_PRESS, changeDirection);
	sgeObject.keyEventSubscribe(GLFW_KEY_W, GLFW_PRESS, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_W, GLFW_RELEASE, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_S, GLFW_PRESS, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_S, GLFW_RELEASE, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_A, GLFW_PRESS, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_A, GLFW_RELEASE, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_D, GLFW_PRESS, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_D, GLFW_RELEASE, changeCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_SPACE, GLFW_PRESS, changeCamera);
	sgeObject.mouseEventSubscribe(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, moveCamera);
	sgeObject.mouseEventSubscribe(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, moveCamera);
	sgeObject.keyEventSubscribe(GLFW_KEY_I, GLFW_PRESS, soldierMove);
	sgeObject.keyEventSubscribe(GLFW_KEY_L, GLFW_PRESS, soldierMove);
	sgeObject.keyEventSubscribe(GLFW_KEY_ENTER, GLFW_PRESS, soldierMove);
	sgeObject.keyEventSubscribe(GLFW_KEY_K, GLFW_PRESS, soldierMove);

	// render objects
	// sgeObject.setMaxInstanceNumber(4);
    if (!sgeObject.registerGameObject(man))
		return 111;
	if (!sgeObject.registerGameObject(man2))
		return 222;
	if (!sgeObject.registerGameObject(man3))
		return 333;
	if (!sgeObject.registerGameObject(helloSGE))
		return 444;
	if (!sgeObject.registerGameObject(ship))
		return 555;
	if (!sgeObject.registerGameObject(car))
		return 666;

	UIButton exitButt("Exit",{0.8,0.1},{50,20}, "Exit!", exitFunction);
	UIText sgetext("Sgetext",{0.09,0.3},{50,50},"Welcome to SGE example application!");

	sgeObject.registerUIObject(exitButt);
	sgeObject.registerUIObject(sgetext);

    if (!sgeObject.init())
        return 11;

#if !__linux__
	sgeObject.setGameAppLogo("/Logo/Logo.png");
#endif

	man.addAnimation("Walk forward",torchMan, 1);
	man.addAnimation("Walk left", 	torchMan, 2);
	man.addAnimation("Walk back", 	torchMan, 3);
	man.addAnimation("Walk right", 	torchMan, 4);

	man2.addAnimation("Idle", soldIdle, 1);
	man2.addAnimation("Run", soldRun, 1);
	man2.addAnimation("Shot", soldShot, 1);
	man2.addAnimation("Recharge", soldRech, 1);


	sgeObject.getCameraObject().setDefaultPos({0,0,5});
	sgeObject.getCameraObject().reset();
    
    while(sgeObject.drawNextFrame()) {
		if (EXIT)
			break;

		// camera processing
		glm::vec2 deltaMouse(0.f);
		if (cameraRotate) {
			deltaMouse = sgeObject.getCursorPos() - cameraPos;
			cameraPos = sgeObject.getCursorPos();
		}

		if (cameraReset) {
			cameraReset = false;
			sgeObject.getCameraObject().reset();
		}

		if (moveCameraZ || moveCameraX || glm::length(deltaMouse) > 0) {
			glm::vec3 rotateAngles(deltaMouse.y,-deltaMouse.x,0);
			sgeObject.getCameraObject().move(rotateAngles,{moveCameraX * 0.05, 0, moveCameraZ * 0.05});
		}

		if (moveDirection == 1) {
			man.velocity={0,0,0.3};
			man.doAnimation("Walk forward", 60);
		} else if (moveDirection == 2) {
			man.doAnimation("Walk left", 60);
			man.velocity={-0.3,0,0};
		} else if (moveDirection == 3) {
			man.doAnimation("Walk back", 60);
			man.velocity={0,0,-0.3};
		} else if (moveDirection == 4) {
			man.doAnimation("Walk right", 60);
			man.velocity={0.3,0,0};
		}


		if (soldierAction == 1) {
			man2.doAnimation("Idle",60);
			man2.velocity = {0,0,0};
		} else if (soldierAction == 2) {
			man2.doAnimation("Run",40);
			man2.velocity = {0.6,0,0};
		} else if (soldierAction == 3) {
			man2.doAnimation("Shot",10);
			man2.velocity = {0,0,0};
		} else if (soldierAction == 4) {
			bool finish = man2.doAnimation("Recharge",60);
			if (finish)
				soldierAction = 3;
			man2.velocity = {0,0,0};
		}

		helloSGE.rotate({0,1,0});
		ship.rotate({0,0.1,0});
		car.rotate({ 0,0.1,0 });
    }

    return 0;
}

#if _WIN64
#include "Windows.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	return main();
}

#endif