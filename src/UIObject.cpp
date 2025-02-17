#include "UIObject.h"

UIObject::UIObject(std::string name, glm::vec2 pos, glm::vec2 size) : _name(name), _pos(pos), _size(size), _visible(true) { ; }

UIObject::~UIObject()
{
    //delete thisElement;
}

SgrUIElement* UIObject::getUIPtr()
{
    return thisElement;
}

void UIObject::setPosition(glm::vec2 newPos)
{
    thisElement->_position = newPos;
    _pos = newPos;
}

void UIObject::setSize(glm::vec2 newSize)
{
    thisElement->_size = newSize;
    _size = newSize;
}

void UIObject::visible(bool visible)
{
    _visible = visible;
    thisElement->_visible = _visible;
}

void UIButton::emptyButtonFunc() { ; } // Empty button function!

UIButton::UIButton(std::string name, glm::vec2 pos, glm::vec2 size, std::string buttonText, void (*func)()) : UIObject(name, pos, size)
{
    thisElement = new SgrUIButton(name, {pos.x, pos.y}, func, buttonText);
    thisElement->_size = size;
}

UIText::UIText(std::string name, glm::vec2 pos, glm::vec2 size, std::string text) : UIObject(name, pos, size)
{
    thisElement = new SgrUIText(name, {pos.x, pos.y}, text);
}

void UIText::changeText(std::string newText)
{
    ((SgrUIText*)(thisElement))->changeText(newText);
}
