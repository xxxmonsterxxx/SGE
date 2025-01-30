#pragma once

#include "sge_utils.h"

class UIObject {

public:
    UIObject(std::string name, glm::vec2 pos = {0,0}, glm::vec2 size = {0.1,0.1});
    virtual ~UIObject();

    void setPosition(glm::vec2 newPos);
    void setSize(glm::vec2 newSize);
    void visible(bool visible);

    virtual SgrUIElement* getUIPtr();
    
protected:
    std::string _name;
    glm::vec2 _pos;
    glm::vec2 _size;
    bool _visible;

    SgrUIElement* thisElement;
};

class UIButton : public UIObject {

private:
    static void emptyButtonFunc();

public:
    UIButton(std::string name, glm::vec2 pos = {0,0}, glm::vec2 size = {50,50}, std::string buttonText = "Button", void (*func)() = emptyButtonFunc);
};

class UIText : public UIObject {

public:
    UIText(std::string name, glm::vec2 pos = {0,0}, glm::vec2 size = {50,50}, std::string text = "TextBox");

    void changeText(std::string newText);
};