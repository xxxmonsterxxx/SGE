#include "AnimationSheet.h"
#include "SGE.h"

AnimationSheet::AnimationSheet(std::string name, std::string filePath, uint8_t horizontal, uint8_t vertical)
                                 : _name(name), _length(horizontal), _filePath(filePath)
{
    _deltaTexture.x = 1.f / horizontal;
    _deltaTexture.y = 1.f / vertical;
}

bool AnimationSheet::initImageData()
{
    std::string path = SGE::getResourcesPath() + _filePath;
    SgrErrCode res = TextureManager::createTextureImage(SGE::getResourcesPath() + _filePath, _animationImage);
    if (res != sgrOK)
        return false;
    
    return true;
}

bool AnimationSheet::getAnimation(Animation& anim, uint8_t vertical)
{
    if (!_animationImage) {
        if (!initImageData())
            return false;
    }

    anim.animPixels = _animationImage;
    anim.frameSize = _deltaTexture;
    anim.length = _length;
    anim.startCoord.x = 0.f;
    anim.startCoord.y = (vertical - 1) * _deltaTexture.y;
    return true;
}

