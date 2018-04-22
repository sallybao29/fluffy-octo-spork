#ifndef SpriteAnimation_hpp
#define SpriteAnimation_hpp

#include <stdio.h>
#include <SDL_opengl.h>
#include <vector>
#include "SheetSprite.hpp"
#include "Timer.hpp"

/*!
 * @discussion Describes how the animation loops
 */
enum LoopConvention {
    /// Loop from the start once the last frame is reached
    LOOP_REPEAT,
    /// Reverse the loop direction once the first or last frame is reached
    LOOP_REVERSE,
    /// Don't loop 
    LOOP_NONE
};

class SpriteAnimation {
public:
    SpriteAnimation(GLuint textureID, const std::vector<float>& data,
                    size_t sheetWidth, size_t sheetHeight, float spriteSize,
                    LoopConvention loopStyle = LOOP_NONE);
    
    ~SpriteAnimation();
    SheetSprite* GetCurrentFrame();
    void NextFrame(float elapsed);
    void SetSpeed(float fps);
    void Stop();
    
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
