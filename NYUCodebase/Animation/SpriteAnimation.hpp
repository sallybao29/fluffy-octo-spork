#ifndef SpriteAnimation_hpp
#define SpriteAnimation_hpp

#include <stdio.h>
#include <SDL_opengl.h>
#include <vector>
#include "SheetSprite.hpp"
#include "Timer.hpp"

/*!
 * @discussion Describes how the animation runs
 */
enum LoopConvention {
    /// Loop from the start once the last frame is reached
    LOOP_REPEAT,
    /// Reverse the loop direction once the first or last frame is reached
    LOOP_REVERSE,
    /// Don't run. For single still sprites
    LOOP_NONE,
    /// Run once and stop
    LOOP_ONCE
};

class SpriteAnimation {
public:
    SpriteAnimation(GLuint textureID, const std::vector<float>& data,
                    size_t sheetWidth, size_t sheetHeight, float spriteSize,
                    LoopConvention loopStyle = LOOP_NONE);
    
    SheetSprite* GetCurrentFrame();
    void NextFrame(float elapsed);
    void SetSpeed(float fps);
    void Stop();
    void Reset();
    
private:
    std::vector<SheetSprite> frames;
    GLuint textureID;
    int frameIndex;
    size_t maxFrames;

    LoopConvention loopStyle;
    int deltaFrame;
    
    float accumulator;
    float framesPerSecond;
};

#endif
