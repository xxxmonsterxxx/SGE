#pragma once
#include <glfw3.h>
#include <map>
#include <vector>

class GameObjectBase;

using sgeKeyEventFunction = void(*)(int, int, int, int);
using sgeMouseEventFunction = void(*)(int, int, int);
using sgePickGameObjectFunction = void(*)(GameObjectBase*);

class EventManager {

private:
	static EventManager* _singleton;

	static void keyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods);
	void keyCallback(int key, int scancode, int action, int mods);

	static void mouseCallbackStatic(GLFWwindow* window, int button, int action, int mods);
	void mouseCallback(int button, int action, int mods);

	GLFWwindow* _window;

	std::map<int, std::map<int, std::vector<sgeKeyEventFunction>>> keySubscribers;
	std::map<int, std::map<int, std::vector<sgeMouseEventFunction>>> mouseSubscribers;
	std::vector<sgePickGameObjectFunction> pickGOSubscribers;


	void pickObjFunc();

public:

	static EventManager& get();

	void init(GLFWwindow* window);

	bool addKeySubscriber(int key, int action, sgeKeyEventFunction func);
	bool addMouseSubscriber(int key, int action, sgeMouseEventFunction func);
	bool addPickObjectSubscriber(sgePickGameObjectFunction func);
};