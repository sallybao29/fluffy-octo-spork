#include "SpriteAnimation.hpp"

SpriteAnimation::SpriteAnimation(GLuint textureID, const std::vector<float>& data,
                                 size_t sheetWidth, size_t sheetHeight, float spriteSize,
                                 LoopConvention loopStyle) : textureID(textureID), loopStyle(loopStyle) {
    
    framesPerSecond = 3;
    frameIndex = 0;
    deltaFrame = 1;
    accumulator = 0.0f;
    maxFrames = data.size() / 4;
    frames.reserve(maxFrames);
    
    for (size_t i = 0; i < data.size(); i += 4) {
        float u =  data[i] / (float) sheetWidth;
        float v = data[i+1] / (float) sheetHeight;
        float width = data[i+2] / (float) sheetWidth;
        float height = data[i+3] / (float) sheetHeight;
        
        frames.emplace_back(textureID, u, v, width, height, spriteSize);
    }
}

SheetSprite* SpriteAnimation::GetCurrentFrame() {
    return &frames[frameIndex];
}

void SpriteAnimation::SetSpeed(float fps) {
    framesPerSecond = fps;
}

void SpriteAnimation::Stop() {
    accumulator = 0;
}

void SpriteAnimation::Reset() {
    accumulator = 0;
    frameIndex = 0;
}

void SpriteAnimation::NextFrame(float elapsed) {
    if (loopStyle == LOOP_NONE) return;
    
    accumulator += elapsed;
    
    if (accumulator > 1 / (float) framesPerSecond) {
        switch (loopStyle) {
            case LOOP_ONCE:
                if (frameIndex < maxFrames - 1) {
                    frameIndex += deltaFrame;
                }
                break;
            case LOOP_REPEAT:
                frameIndex = (frameIndex + deltaFrame) % maxFrames;
                break;
            case LOOP_REVERSE:
                frameIndex += deltaFrame;
                if (frameIndex < 0) {
                    frameIndex = (int) maxFrames - 1;
                    deltaFrame *= -1;
                }
                else if (frameIndex == maxFrames) {
                    frameIndex = 0;
                    deltaFrame *= -1;
                }
                break;
            default:
                break;
        }
        accumulator -= (1 / (float) framesPerSecond);
    }
}
