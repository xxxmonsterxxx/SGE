#pragma once

class iGameObject {
public:

	virtual void setPosition(SGEPosition newPosition) { ; }
	virtual void setRotation(glm::vec3 axis, float angle) { ; }
	virtual void setTextureMapping(glm::vec2 deltaTexture, glm::vec2 meshStart, glm::vec2 textureStart) { ; }
    virtual void setScale(glm::vec3 newScale) { ; }
	virtual void setColor(SgeColor newColor) { ; }
};