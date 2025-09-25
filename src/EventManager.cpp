#include "SGE.h"

#include "EventManager.h"

EventManager* EventManager::_singleton = nullptr;

EventManager& EventManager::get()
{
	if (_singleton) {
		return *_singleton;
	} else {
		_singleton = new EventManager();
		return *_singleton;
	}
}

void EventManager::init(GLFWwindow* window)
{
	_window = window;
	glfwSetKeyCallback(_window, EventManager::keyCallbackStatic);
	glfwSetMouseButtonCallback(_window, EventManager::mouseCallbackStatic);
}

void EventManager::keyCallback(int key, int scancode, int action, int mods)
{
	auto itr = keySubscribers.find(key);
	if (itr == keySubscribers.end()) {
		return;
	}

	auto itr2 = keySubscribers[key].find(action);
	if (itr2 == keySubscribers[key].end()) {
		return;
	}

	for (auto func : keySubscribers[key][action]) {
		func(key, scancode, action, mods);
	}
}

void EventManager::keyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	EventManager::get().keyCallback(key, scancode, action, mods);
}

bool EventManager::addKeySubscriber(int key, int action, sgeKeyEventFunction func)
{
	keySubscribers[key][action].push_back(func);
	return true;
}

void EventManager::mouseCallbackStatic(GLFWwindow* window, int button, int action, int mods)
{
	EventManager::get().mouseCallback(button, action, mods);
}


void EventManager::mouseCallback(int button, int action, int mods)
{
	auto itr = mouseSubscribers.find(button);
	if (itr == mouseSubscribers.end()) {
		return;
	}

	auto itr2 = mouseSubscribers[button].find(action);
	if (itr2 == mouseSubscribers[button].end()) {
		return;
	}

	for (auto func : mouseSubscribers[button][action]) {
		func(button, action, mods);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickObjFunc();
	}
}

bool EventManager::addMouseSubscriber(int button, int action, sgeMouseEventFunction func)
{
	mouseSubscribers[button][action].push_back(func);
	return true;
}

bool EventManager::addPickObjectSubscriber(sgePickGameObjectFunction func)
{
	pickGOSubscribers.push_back(func);
	return true;
}

void EventManager::pickObjFunc()
{
	glm::vec2 cpos = SGE::getCursorPos();

	for (auto go : SGE::get().gameObjects) {
		if (go->checkPointInside(cpos)) {
			for (auto func : pickGOSubscribers) {
				func(go);		
			}
			return;
		}
	}
}