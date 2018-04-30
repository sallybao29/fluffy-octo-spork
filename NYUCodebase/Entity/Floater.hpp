#ifndef Floater_hpp
#define Floater_hpp

#include <stdio.h>
#include <unordered_set>
#include "Entity.hpp"
#include "Matrix.h"

#define MAX_BULLETS 20
#define BULLET_SPEED 0.5f

class Floater;

class Bullet {
public:
    Bullet(const SheetSprite& sprite);
    ~Bullet();
    
    void Render(ShaderProgram& program);
    void Update(float elapsed);
    bool CollidesWith(Entity& entity);
    void CollideWithMap(const FlareMap& map, const std::unordered_set<unsigned int>& solidTiles);
    
    SheetSprite* sprite;
    Shape* shape;
    Vector3 position;
    Vector3 velocity;
    bool active;
};


class Floater : public Entity {
public:
    Floater(float x, float y, float range, float shootSpeed, float bulletSize = 0.1f);
    
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
};

#endif
