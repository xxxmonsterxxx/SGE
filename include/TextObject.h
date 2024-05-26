#pragma once

#include "GameObject.h"

class TextObject : public GameObjectBase {

private:

	std::string _text;
	std::string _fontPath = "/Resources/Fonts/times new roman.ttf";
	unsigned char* _fontPixels = nullptr;
	unsigned char*  _charsData = nullptr;

	std::vector<GameObject*> _gameObjectsData;

	const int _fontBitmapWidth = 2048; // generated size of bitmap for font
	const int _fontBitmapHeight = 2048;
	const uint16_t _fontHeight = 256; // generated size of font in pixels e.g. quality
	bool loadFontData();

	struct LetterData {
		glm::vec3 position;
		glm::vec2 deltaTexture; // multiplier conversion from mesh to texture
		glm::vec2 startMesh; // left top mesh coords
		glm::vec2 startTexture; // left top texture coords
		std::vector<SGEPosition> mesh;
	};

	const char _startChar = ' ';
	const char _lastChar  = '~';

	LetterData getLetterData(glm::vec2& nextLetter, char letter);

	static Mesh _letterMesh;

	bool _textReady = false;

	// text center (left bot is coords of 0th character)
	SGEPosition _centerPos;

	static int _textId;

public:

	TextObject(std::string text);
	TextObject(std::string text, std::string font);

	std::vector<GameObject*> getGameObjectsData();

	void setPosition(SGEPosition newPosition);
	void setRotation(glm::vec3 angle);
	void rotate(glm::vec3 dAngle);
	void move(glm::vec3 dPos);

};