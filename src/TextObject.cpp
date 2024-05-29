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

	// convert from [-1 ; 1] to coord system of stbtt
	float pixelToCoordW = 2.f / _fontBitmapWidth;
	float pixelToCoordH = 2.f / _fontBitmapHeight;

	stbtt_aligned_quad charData;
	stbtt_GetBakedQuad((stbtt_bakedchar*)_charsData, _fontBitmapWidth, _fontBitmapHeight, letter - _startChar, &nextLetter.x, &nextLetter.y, &charData, 1);

	// got letter rectangle Mesh coordinates
	SGEPosition tL{charData.x0*pixelToCoordW,charData.y0*pixelToCoordH,0};
	SGEPosition tR{charData.x1*pixelToCoordW,charData.y0*pixelToCoordH,0};
	SGEPosition bR{charData.x1*pixelToCoordW,charData.y1*pixelToCoordH,0};
	SGEPosition bL{charData.x0*pixelToCoordW,charData.y1*pixelToCoordH,0};

	// offset to move mesh left bot in zero
	ret.position = bL;
	tL = tL - bL;
	tR = tR - bL;
	bR = bR - bL;
	bL = bL - bL;

	ret.mesh.push_back(tL);
	ret.mesh.push_back(tR);
	ret.mesh.push_back(bR);
	ret.mesh.push_back(bL);

	// calculate texture mapping data
	ret.startTexture = glm::vec2(charData.s0,charData.t0);
	ret.startMesh = tL;
	ret.deltaTexture.x = (charData.s1 - charData.s0) / ((charData.x1 - charData.x0)*pixelToCoordW);
	ret.deltaTexture.y = (charData.t1 - charData.t0) / ((charData.y1 - charData.y0)*pixelToCoordH);

	return ret;
}

TextObject::TextObject(std::string text) :
	_text(text)
{
	_fontPath = SGE::execPath + _fontPath;
	if (!loadFontData())
		return;

	glm::vec2 nextLetter(0,0);

	SGEPosition leftBot;
	SGEPosition rightTop;

	for (uint8_t i = 0; i < text.length(); i++) {
		char letter = text[i];
		std::string GOLetterName = "letter_" + std::to_string(_textId)+std::string("_")+std::to_string(i);
		LetterData LD = getLetterData(nextLetter, letter);
		Mesh* _letterMesh = new Mesh(GOLetterName, LD.mesh, std::vector<uint16_t>{ 0,1,2,2,3,0 });
		
		GameObject* letterGO = new GameObject(GOLetterName, *_letterMesh, _fontPixels, _fontBitmapWidth, _fontBitmapHeight);
		letterGO->setPosition(LD.position);
		letterGO->setTextureMapping(LD.deltaTexture, LD.startMesh, LD.startTexture);
		_gameObjectsData.push_back(letterGO);

		if (i == 0) {
			leftBot = LD.mesh[0] + LD.position;
		}

		if (i == text.length() - 1) {
			rightTop = LD.mesh[1] + LD.position;
		}
	}

	// calculate coordinate of text center
	_centerPos.x = leftBot.x + (rightTop.x - leftBot.x) / 2;
	_centerPos.y = leftBot.y + (rightTop.y - leftBot.y) / 2;
	_centerPos.z = 0.f;

	setPosition({0,0,0});

	_textReady = true;
	_textId++;
}

std::vector<GameObject*> TextObject::getGameObjectsData()
{
	return _gameObjectsData; 
}

void TextObject::move(glm::vec3 dPos)
{
	setPosition(_centerPos + dPos);
}

void TextObject::setPosition(SGEPosition newPosition)
{
	SGEPosition delta = newPosition - _centerPos;
	for (size_t i = 0; i < _gameObjectsData.size(); i++)
		_gameObjectsData[i]->move(delta);
	_centerPos = newPosition;
}

void TextObject::rotate(glm::vec3 dAngle)
{	
	glm::vec3 axis = _centerPos;
	axis.x += 1;
	rotate(_centerPos, axis, dAngle.x);

	axis = _centerPos;
	axis.y += 1;
	rotate(_centerPos, axis, dAngle.y);
	
	axis = _centerPos;
	axis.z += 1;
	rotate(_centerPos, axis, dAngle.z);
}

void TextObject::setRotation(glm::vec3 angle)
{
	glm::vec3 backRot = -getRotation();
	glm::vec3 tempRot = backRot;
	tempRot.x = 0; tempRot.y = 0;
	rotate(tempRot);
	tempRot = backRot;
	tempRot.x = 0; tempRot.z = 0;
	rotate(tempRot);
	tempRot = backRot;
	tempRot.y = 0; tempRot.z = 0;
	rotate(tempRot);

	rotate(angle);
}

void TextObject::rotate(glm::vec3 begin, glm::vec3 end, float angle)
{
	for (size_t i = 0; i < _gameObjectsData.size(); i++) {
		_gameObjectsData[i]->rotate(begin, end, angle);
	}

	glm::vec4 trans(_centerPos.x-begin.x,_centerPos.y-begin.y,_centerPos.z-begin.z,1);
	glm::mat4 newP(1);
	newP = glm::rotate(newP, glm::radians(angle), end-begin);
	trans = newP * trans;
	_centerPos = {trans.x,trans.y,trans.z};
	_centerPos += begin;

	glm::vec3 axis = end - begin;
	// rotate axis projection on XY around Z to connect with X axis then we got axis on XZ.
	float alpha = -glm::degrees(atan2f(axis.y,axis.x));
	// rotate axis in XZ arround Y to connect with Z
	glm::vec2 axisxy{axis.x,axis.y};
	float beta = glm::degrees(atan2f(glm::length(axisxy),axis.z));

	glm::mat4 newR(1);
	newR = glm::rotate(newR, glm::radians(-alpha),{0,0,1});
	newR = glm::rotate(newR, glm::radians(beta),  {0,1,0});
	newR = glm::rotate(newR, glm::radians(angle), {0,0,1});
	newR = glm::rotate(newR, glm::radians(-beta), {0,1,0});
	newR = glm::rotate(newR, glm::radians(alpha), {0,0,1});
	_rotationM = newR * _rotationM;
}

glm::vec3 TextObject::getPosition()
{
	return _centerPos;
}