#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include "Vector3.hpp"
#include "Shape.hpp"
#include "SheetSprite.hpp"

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

class SheetSprite;
class ShaderProgram;
class Matrix;

/*!
 * @discussion A list of entity types
 */
enum EntityType {
    /// A player entity
    ENTITY_PLAYER = 10,
    /// An enemy entity
    ENTITY_ENEMY = 20
};

class Entity {
public:
    Entity();
    Entity(float x, float y, Shape& shape, EntityType type);
    Entity(float x, float y, SheetSprite *sprite, EntityType type);
    ~Entity();
    
    void Update(float elapsed);
    void Render(ShaderProgram& program);
    void SetSprite(SheetSprite* newSprite);
    void SetColor(float r, float g, float b, float a);
    void UpdateMatrix();
    bool CollidesWith(Entity& other);
    
    SheetSprite* sprite = nullptr;
    
    Vector3 position;
    Vector3 scale;
    Vector3 velocity;
    Vector3 acceleration;
    Shape* shape;
    
    Matrix modelMatrix;
    
    float rotation = 0.0f;
    
    EntityType entityType;
    
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
};
#endif
