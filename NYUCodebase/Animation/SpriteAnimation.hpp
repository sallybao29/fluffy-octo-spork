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
    SpriteAnimation(GLuint textureID, const std::vector<int>& data,
                    size_t sheetWidth, size_t sheetHeight, size_t spriteSize,
                    LoopConvention loopStyle = LOOP_NONE);
    
    ~SpriteAnimation();
    const SheetSprite* GetCurrentFrame() const;
    void NextFrame();
    
private:
    std::vector<SheetSprite> frames;
    GLuint textureID;
    int frameIndex;
    size_t maxFrames;
    
    bool loop;
    LoopConvention loopStyle;
    int deltaFrame;
    
    Timer timer;
    float framesPerSecond;
};

#endif
