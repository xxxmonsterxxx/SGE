#pragma once

#include "utils.h"
#include <SGR.h>

struct Animation {
    glm::vec2 startCoord{0,0};
    glm::vec2 frameSize{1,1};
    uint8_t frame{0};
    uint8_t frameCounter{0};
    uint8_t length{1};
    uint8_t speed{1}; // in frames
    const SgrImage* animPixels;
};

class AnimationSheet {

private:

    std::string _name;
    glm::vec2 _deltaTexture;
    SgrImage* _animationImage = nullptr;
    const uint8_t _length;
    std::string _filePath;

    bool initImageData();

public:

    AnimationSheet(std::string name, std::string filePath, uint8_t horizontal, uint8_t vertical);

    bool getAnimation(Animation& anim, uint8_t vertical);
};