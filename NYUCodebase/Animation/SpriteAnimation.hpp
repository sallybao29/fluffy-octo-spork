#ifndef SpriteAnimation_hpp
#define SpriteAnimation_hpp

#include <stdio.h>
#include <SDL_opengl.h>
#include <vector>
#include "SheetSprite.hpp"

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
    /*!
     * @discussion Creates a SpriteAnimation and generates frames from data read in chunks of 4
     * @param textureID The id of the texture that the sprites originate from
     * @param data The data used to generate the frames for the animation. Each frame requires 4 values:
     * x, y, width, and height of the sprite on the texture
     * @param sheetWidth The width of the sprite sheet
     * @param sheetHeight The height of the sprite sheet
     * @param spriteSize The size to make the sprites
     * @param loopStyle How the animation should loop
     */
    SpriteAnimation(GLuint textureID, const std::vector<float>& data,
                    size_t sheetWidth, size_t sheetHeight, float spriteSize,
                    LoopConvention loopStyle = LOOP_NONE);
    
    /*!
     * @discussion Returns a pointer to the sprite that represents the current animation frame
     * @return A pointer to the current frame
     */
    SheetSprite* GetCurrentFrame();
    
    /*!
     * @discussion Moves the animation to the next frame if enough time has passed. The amount of time necessary
     * to advance a frame varies depending on the frame rate
     * @param elapsed The amount of time that has elapsed
     */
    void NextFrame(float elapsed);
    
    /*!
     * @discussion Sets the animation speed
     * @param fps The number of frames the animation should run per second
     */
    void SetSpeed(float fps);
    
    /*!
     * @discussion Stops the animation on its current frame
     */
    void Stop();
    
    /*!
     * @discussion Resets the animation to its first frame
     */
    void Reset();
    
private:
    /// The frames that make up the animation
    std::vector<SheetSprite> frames;
    /// The id of the texture that the animation derives its frames from
    GLuint textureID;
    /// The index of the current frame
    int frameIndex;
    /// The total number of frames in the animation
    size_t maxFrames;

    /// The way the animation loops
    LoopConvention loopStyle;
    /// The number of frames to change by
    int deltaFrame;
    /// Accumulates the amount of elapsed time until the frame frequency is reached
    float accumulator;
    /// The number of frames the animation should run per second
    float framesPerSecond;
};

#endif
