#include "SpriteAnimation.hpp"

SpriteAnimation::SpriteAnimation(GLuint textureID, const std::vector<int>& data,
                                 size_t sheetWidth, size_t sheetHeight, size_t spriteSize,
                                 LoopConvention loopStyle) : textureID(textureID), loopStyle(loopStyle) {
    
    loop = loopStyle != LOOP_NONE;
    frameIndex = 0;
    deltaFrame = 1;
    maxFrames = data.size() / 4;
    frames.reserve(maxFrames);
    
    for (size_t i = 0; i < data.size(); i += 4) {
        float u = (float) data[i] / sheetWidth;
        float v = (float) data[i+1] / sheetHeight;
        float width = (float) data[i+2] / sheetWidth;
        float height = (float) data[i+3] / sheetHeight;
        float aspect = (float) data[i+2] / (float) data[i+3];
        
        frames.emplace_back(textureID, u, v, width, height, aspect, spriteSize);
    }
}

const SheetSprite* SpriteAnimation::GetCurrentFrame() const {
    return &frames[frameIndex];
}

void SpriteAnimation::NextFrame() {
    switch (loopStyle) {
        case LOOP_NONE:
            break;
        case LOOP_REPEAT:
            frameIndex = (frameIndex + deltaFrame) % maxFrames;
            break;
        case LOOP_REVERSE:
            frameIndex += deltaFrame;
            if (frameIndex < 0) {
                frameIndex = (int) maxFrames;
                deltaFrame *= -1;
            }
            else if (frameIndex > maxFrames) {
                frameIndex = 0;
                deltaFrame *= -1;
            }
            break;
        default:
            break;
    }
}
