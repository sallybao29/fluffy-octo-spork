#include "ShaderProgram.h"
#include "SheetSprite.hpp"

SheetSprite::SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size)
: textureID(textureID), u(u), v(v), width(width), height(height), size(size), reversed(false) {}

SheetSprite::SheetSprite(unsigned int textureID, int index, int spriteCountX, int spriteCountY, float size)
: textureID(textureID), size(size), reversed(false) {
    u = (float)(index % spriteCountX) / (float) spriteCountX;
    v = ((float)index / spriteCountX) / (float) spriteCountY;
    width = 1.0/(float)spriteCountX;
    height = 1.0/(float)spriteCountY;
}

void SheetSprite::Render(ShaderProgram& program) const {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat texCoords[] = {
        u, v + height,
        u + width, v,
        u, v,
        u + width, v,
        u, v + height,
        u + width, v + height
    };
    
    float width = this->width;
    if (reversed) {
        width *= -1;
    }
    float vertices[] = {
        -size * width, -size * height,
        size * width, size * height,
        -size * width, size * height,
        size * width, size * height,
        -size * width, -size * height,
        size * width, -size * height
    };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}
