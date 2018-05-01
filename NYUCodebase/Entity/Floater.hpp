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
    
    /*!
     * @discussion Checks if the bullet hits an entity and deactivates the bullet upon collision
     * @param entity The entity to collide with
     * @return Whether the collision occurred
     */
    bool CollidesWith(Entity& entity);
    
    /*!
     * @discussion Checks if the bullet hits a solid tile in the map and deactivates the bullet upon collision
     * @param map The map
     * @param solidTiles A reference to a set of tile values that are considered solid
     * @return Whether the collision occurred
     */
    bool CollideWithMap(const FlareMap& map, const std::unordered_set<unsigned int>& solidTiles);
    
    SheetSprite* sprite;
    Shape* shape;
    Vector3 position;
    Vector3 velocity;
    bool active;
};


class Floater : public Entity {
public:
    /*!
     * @discussion Initializes a floater at the given position with the specified range, shooting speed,
     * and bullet size
     * @param x The x coordinate of the position
     * @param y The y coordinate of the position
     * @param range The floater's detection range
     * @param shootSpeed The number of times the floater shoots per second
     * @param bulletSize The size of the bullet
     */
    Floater(float x, float y, float range, float shootSpeed, float bulletSize = 0.1f);
    
    /*!
     * @discussion Shoots a single bullet from the given position
     * @param x The x coordinate of the position
     * @param y The y coordinate of the position
     * @param velX The x velocity of the bullet
     * @param velY The y velocity of the bullet
     */
    void ShootBullet(float x, float y, float velX, float velY);
    
    /*!
     * @discussion Shoots four bullets, each from a corner of the floaters
     */
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
