#pragma once

class iGameObject {
protected:

	SGEPosition _position = {0,0,0};
	glm::vec3 _rotation = {0,0,0};

public:

	virtual void setPosition(SGEPosition newPosition) { ; }
	virtual void move(SGEPosition dPos) { ; }
	SGEPosition getPosition() { return {0,0,0}; }

	virtual void setRotation(glm::vec3 angle) { ; }
	virtual void rotate(glm::vec3 dAngle, bool global = false) { ; }
	
	virtual void setTextureMapping(glm::vec2 deltaTexture, glm::vec2 meshStart, glm::vec2 textureStart) { ; }
    virtual void setScale(glm::vec3 newScale) { ; }
	virtual void setColor(SGEColor newColor) { ; }
};