#include <math.h>
#include "ShaderProgram.h"
#include "Entity.hpp"
#include "SatCollision.h"
#include "GameUtilities.hpp"

#define DELTA 0.00001f

Entity::Entity(float x, float y, const Shape& shape, EntityType type)
: position(x, y, 0.0f), scale(1.0f, 1.0f, 1.0f), entityType(type) {
    this->shape = shape.Clone();
}

Entity::Entity(float x, float y, SheetSprite *sprite, EntityType type)
: position(x, y, 0.0f), scale(1.0f, 1.0f, 1.0f), sprite(sprite), entityType(type) {
    
    this->shape = new Rectangle(2 * sprite->width * sprite->size, 2 * sprite->height * sprite->size);
}

Entity::Entity() {}

Entity::~Entity() {
    if (shape != nullptr) {
        delete shape;
        shape = nullptr;
    }
    for (std::pair<EntityAction, SpriteAnimation*> animation : animations) {
        delete animation.second;
    }
}

void Entity::SetSprite(SheetSprite* newSprite) {
    sprite = newSprite;
    shape->SetSize(2 * sprite->width * sprite->size, 2 * sprite->height * sprite->size);
}

void Entity::UpdateMatrix() {
    modelMatrix.Identity();
    modelMatrix.Translate(position.x, position.y, position.z);
    modelMatrix.Rotate(rotation);
    modelMatrix.Scale(scale.x, scale.y, scale.z);
}

void Entity::Render(ShaderProgram& program) {
    UpdateMatrix();
    program.SetModelMatrix(modelMatrix);
    
    if (sprite == nullptr) {
        program.SetColor(color[RED], color[GREEN], color[BLUE], color[ALPHA]);
        DrawShape(program, *shape);
    }
    
    else {
        // Invert sprite direction depending on movement
        if (facingRight && velocity.x < 0) {
            facingRight = false;
        }
        else if (!facingRight && velocity.x > 0) {
            facingRight = true;
        }
        if (entityType == ENTITY_FLYING || entityType == ENTITY_FLOATING) {
            facingRight = true;
        }
        sprite->reversed = !facingRight;
        sprite->Render(program);
    }
}

void Entity::Update(float elapsed) {
    collidedTop = false;
    collidedLeft = false;
    collidedRight = false;
    collidedBottom = false;
}

bool Entity::CollidesWith(Entity& other) {
    std::pair<float, float> penetration;
    
    UpdateMatrix();
    other.UpdateMatrix();
    
    std::vector<std::pair<float, float>> e1Points = ToWorldSpace(modelMatrix, shape->points());
    std::vector<std::pair<float, float>> e2Points = ToWorldSpace(other.modelMatrix, other.shape->points());
    bool collided = CheckSATCollision(e1Points, e2Points, penetration);
    
    if (collided) {
        position.x += penetration.first * 0.5f;
        position.y += penetration.second * 0.5f;
        
        other.position.x -= penetration.first * 0.5f;
        other.position.y -= penetration.second * 0.5f;
    }
    
    return collided;
}

bool Entity::CollidesWithX(float x, float width) {
    collidedLeft = (position.x - shape->size.x / 2 < x + width / 2) &&
                   (position.x - shape->size.x / 2 > x - width / 2);
    collidedRight = (position.x + shape->size.x / 2 < x + width / 2) &&
                    (position.x + shape->size.x / 2 > x - width / 2);
    
    // Adjust by the amount of penetration if there is collision
    if (collidedRight) {
        float penetration = fabs((position.x + shape->size.x / 2) - (x - width / 2));
        position.x -= penetration - DELTA;
        velocity.x = 0;
    }
    else if (collidedLeft) {
        float penetration = fabs((x + width / 2) - (position.x - shape->size.x / 2));
        position.x += penetration + DELTA;
        velocity.x = 0;
    }
    
    return collidedLeft || collidedRight;
}

bool Entity::CollidesWithY(float y, float height) {
    collidedTop = (position.y + shape->size.y / 2 < y + height / 2) &&
                  (position.y + shape->size.y / 2 > y - height / 2);
    collidedBottom = (position.y + shape->size.y / 2 > y + height / 2) &&
                     (position.y - shape->size.y / 2 < y + height / 2);
    
    // Adjust by the amount of penetration if there is collision
    if (collidedTop) {
        float penetration = fabs((position.y + shape->size.y / 2) - (y - height / 2));
        position.y -= penetration - DELTA;
        velocity.y = 0;
    }
    else if (collidedBottom) {
        float penetration = fabs((y + height / 2) - (position.y - shape->size.y / 2));
        position.y += penetration + DELTA;
        velocity.y = 0;
    }
    
    return collidedTop || collidedBottom;
}


void Entity::SetColor(float r, float g, float b, float a) {
    color[RED] = r;
    color[GREEN] = g;
    color[BLUE] = b;
    color[ALPHA] = a;
}

bool Entity::AddAnimation(EntityAction action, const std::string textureName, float spriteSize,
                          LoopConvention loopStyle, int maxFrames) {
    std::vector<float> spriteData;
    
    bool found = textureAtlas.GetSpritesData(textureName, spriteData, maxFrames);
    animations[action] = new SpriteAnimation(textures[OBJECTS], spriteData,
                                            1024, 1024, spriteSize, loopStyle);
    return found;
}

