#include "TextObject.h"
#include "SGE.h"

#include <filesystem>

#define STB_TRUETYPE_IMPLEMENTATION
	#include "../modules/stb/stb_truetype.h"

Mesh TextObject::_letterMesh = Mesh::getDefaultRectangleMesh("letterMesh");
int  TextObject::_textId = 0;

bool TextObject::loadFontData()
{
	auto fontFileSize = std::filesystem::file_size(_fontPath);
	if (fontFileSize < 1)
		return false;

	unsigned char* fontData = new unsigned char [fontFileSize];

	std::ifstream fontFile(_fontPath, std::ios::binary);
	fontFile.read((char*)fontData, fontFileSize);
	fontFile.close();

	_fontPixels = new unsigned char [_fontBitmapWidth*_fontBitmapHeight];
	const int charNumberToBake = _lastChar - _startChar;
	_charsData = new unsigned char [charNumberToBake*sizeof(stbtt_bakedchar)];

	int result = stbtt_BakeFontBitmap(fontData, 0, _fontHeight, _fontPixels, _fontBitmapWidth, _fontBitmapHeight, _startChar, charNumberToBake, (stbtt_bakedchar*)_charsData);

	delete[] fontData;

	return true;
}

TextObject::LetterData TextObject::getLetterData(glm::vec2& nextLetter, char letter)
{
	LetterData ret;

	// all data for font is in pixels but we need pure coordinates [-1 ; 1]
	float pixelToCoordW = 2.f / _fontBitmapWidth;
	float pixelToCoordH = 2.f / _fontBitmapHeight;

	ret.position.x = nextLetter.x;
	ret.position.y = nextLetter.y;

	nextLetter.x = (nextLetter.x + 1.f) / pixelToCoordW;
	nextLetter.y = (nextLetter.y + 1.f) / pixelToCoordH;

	stbtt_aligned_quad charData;
	stbtt_GetBakedQuad((stbtt_bakedchar*)_charsData, _fontBitmapWidth, _fontBitmapHeight, letter - _startChar, &nextLetter.x, &nextLetter.y, &charData, 1);

	nextLetter.x = nextLetter.x * pixelToCoordW - 1.f;
	nextLetter.y = nextLetter.y * pixelToCoordH - 1.f;

	ret.startTexture = glm::vec2(charData.s0,charData.t0);
	ret.startMesh = glm::vec2(charData.x0*pixelToCoordW, charData.y0*pixelToCoordH);
	ret.deltaTexture.x = (charData.s1 - charData.s0) / ((charData.x1 - charData.x0)*pixelToCoordW);
	ret.deltaTexture.y = (charData.t1 - charData.t0) / ((charData.y1 - charData.y0)*pixelToCoordH);

	SGEPosition tL{charData.x0*pixelToCoordW,charData.y0*pixelToCoordH,0};
	SGEPosition tR{charData.x1*pixelToCoordW,charData.y0*pixelToCoordH,0};
	SGEPosition bR{charData.x1*pixelToCoordW,charData.y1*pixelToCoordH,0};
	SGEPosition bL{charData.x0*pixelToCoordW,charData.y1*pixelToCoordH,0};
	ret.mesh.push_back(tL);
	ret.mesh.push_back(tR);
	ret.mesh.push_back(bR);
	ret.mesh.push_back(bL);

	return ret;
}

TextObject::TextObject(std::string text) :
	_text(text)
{
	_fontPath = SGE::execPath + _fontPath;
	if (!loadFontData())
		return;

	glm::vec2 nextLetter(-1,0);
	SGEPosition firstOffset;
	bool offsetSetted = false;
	for (uint8_t i = 0; i < text.length(); i++) {
		char letter = text[i];
		std::string GOLetterName = "letter_" + std::to_string(_textId)+std::string("_")+std::to_string(i);
		LetterData LD = getLetterData(nextLetter, letter);
		Mesh* _letterMesh = new Mesh(GOLetterName, LD.mesh, std::vector<uint16_t>{ 0,1,2,2,3,0 });

		if (!offsetSetted) {
			firstOffset.x = -LD.mesh[0].x;
			firstOffset.y = -LD.mesh[0].y;
			firstOffset.z = 0;
			offsetSetted = true;
		}
		
		GameObject* letterGO = new GameObject(GOLetterName, *_letterMesh, _fontPixels, _fontBitmapWidth, _fontBitmapHeight);
		letterGO->setPosition(firstOffset);
		letterGO->setTextureMapping(LD.deltaTexture, LD.startMesh, LD.startTexture);
		_gameObjectsData.push_back(letterGO);
	}

	_textReady = true;
	_textId++;
}

std::vector<GameObject*> TextObject::getGameObjectsData()
{
	return _gameObjectsData; 
}

void TextObject::setPosition(SGEPosition newPosition)
{
	for (size_t i = 0; i < _gameObjectsData.size(); i++)
		_gameObjectsData[i]->setPosition(newPosition);
}

void TextObject::setScale(glm::vec3 newScale)
{
	for (size_t i = 0; i < _gameObjectsData.size(); i++)
		_gameObjectsData[i]->setScale(newScale);
}

void TextObject::setRotation(glm::vec3 axis, float angle)
{
	for (size_t i = 0; i < _gameObjectsData.size(); i++)
		_gameObjectsData[i]->setRotation(axis, angle);
}