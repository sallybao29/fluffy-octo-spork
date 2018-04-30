#ifndef Floater_hpp
#define Floater_hpp

#include <stdio.h>
#include "Entity.hpp"
#include "Matrix.h"

#define MAX_BULLETS 20
#define BULLET_SPEED 1.0f

class Floater;

class Bullet {
public:
    Bullet(const SheetSprite& sprite, float size = 0.1f);
    ~Bullet();
    
    void Render(ShaderProgram& program, Matrix& matrix);
    void Update(float elapsed);
    bool CollidesWith(Entity& entity);
    
    SheetSprite* sprite;
    Shape* shape;
    Vector3 position;
    Vector3 velocity;
    Floater* parent;
    bool active;
};


class Floater : public Entity {
public:
    Floater(float x, float y, float range, float shootSpeed);
    
    void ShootBullet(float x, float y, float velX, float velY);
    void Shoot();
    
    /*!
     * @discussion Updates the AI state of the flyer
     * @param target The target entity
     * @param elapsed The amount of time that has elapsed
     */
    void Update(Entity& target, float elapsed);
    
    void Render(ShaderProgram& program) override;
    
    std::vector<Bullet> bullets;
    int bulletIndex;
    
    float range;
    float shootSpeed;
    float secondsSinceShot;
    float bulletSize;
    
    AIState state;
    
    Matrix matrix;
};

#endif
