#ifndef SheetSprite_hpp
#define SheetSprite_hpp

#include <stdio.h>

class ShaderProgram;

/*!
 * @discussion Consolidates uniform and non-uniform sprites into a single class that stores information
 * on how to draw them
 */
class SheetSprite {
public:
    /*!
     * @discussion Initializes a non-uniform sprite
     * @param textureID The id of the sprite's texture
     * @param u The normalized u coordinate of the sprite on the texture
     * @param v The normalized v coordinate of the sprite on the texture
     * @param width The normalized width of the sprite
     * @param height The normalized height of the sprite
     * @param aspect The original width-to-height ratio of the sprite on the texture
     * @param size The scale factor to apply to the normalized sprite size
     */
    SheetSprite(unsigned int textureID, float u, float v, float width, float height, float aspect, float size);
    
    /*!
     * @discussion Initializes a uniform sprite
     * @param textureID The id of the sprite's texture
     * @param index The index of the sprite on the texture
     * @param spriteCountX The number of columns that the texture contains
     * @param spriteCountY The number of rows that the texture contains
     * @param aspect The original width-to-height ratio of the sprite on the texture
     * @param size The scale factor to apply to the normalized sprite size
     */
    SheetSprite(unsigned int textureID, int index, int spriteCountX, int spriteCountY, float aspect, float size);
    
    /*!
     * @abstract Renders the sprite
     * @discussion Renders the portion of the texture that contains the sprite
     * @param program The shader program to use when rendering
     */
    void Render(ShaderProgram& program) const;
    
    /// The id of the sprite's texture
    unsigned int textureID;

    /// The normalized u coordinate of the sprite on the texture
    float u;
    /// The normalized v coordinate of the sprite on the texture
    float v;
    
    /// The normalized width of the sprite
    float width;
    /// The normalized height of the sprite
    float height;
    
    /// The original width-to-height ratio of the sprite on the texture
    float aspect;
    /// The scale factor to apply to the normalized sprite size
    float size;
    
    /// Whether the sprite should be flipped (reflected across the y-axis)
    bool reversed;
};

#endif
