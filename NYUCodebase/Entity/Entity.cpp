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
    
    this->shape = new Rectangle(sprite->aspect * sprite->size, sprite->size);
}

Entity::Entity() {}

Entity::~Entity() {
    if (shape != nullptr) {
        delete shape;
        shape = nullptr;
    }/*
    if (sprite != nullptr) {
        delete sprite;
        sprite = nullptr;
    }*/
}

void Entity::SetSprite(SheetSprite* newSprite) {
    sprite = newSprite;
    shape->SetSize(sprite->aspect * sprite->size, sprite->size);
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
        if ((sprite->reversed && velocity.x > 0) ||
            (!sprite->reversed && velocity.x < 0)) {
            sprite->reversed = !sprite->reversed;
        }
        sprite->Render(program);
    }
}

void Entity::Update(float elapsed) {
    collidedTop = false;
    collidedLeft = false;
    collidedRight = false;
    collidedBottom = false;
    
    position.x += velocity.x * elapsed;
    position.y += velocity.y * elapsed;
    
    SheetSprite* frame = animation->GetCurrentFrame();
    if (frame != sprite) {
         SetSprite(frame);
    }

    if (fabs(velocity.x) == 0) {
        animation->Stop();
    }
    else {
        animation->NextFrame(fabs(velocity.x) * elapsed);
    }
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

void Entity::SetColor(float r, float g, float b, float a) {
    color[RED] = r;
    color[GREEN] = g;
    color[BLUE] = b;
    color[ALPHA] = a;
}

