#ifndef Floater_hpp
#define Floater_hpp

#include <stdio.h>
#include "Entity.hpp"

#define MAX_BULLETS 20

class Bullet {
public:
    Bullet();
private:
    Shape* shape;
    Vector3 position;
    Vector3 velocity;
};


class Floater : public Entity {
public:
    Floater(float x, float y, float range);
    
    void ShootBullet();
    /*!
     * @discussion Updates the AI state of the flyer
     * @param target The target entity
     * @param elapsed The amount of time that has elapsed
     */
    void Update(Entity& target, float elapsed);
    
    void Render(ShaderProgram& program) override;
private:
    Bullet bullets[MAX_BULLETS];
    int bulletIndex;
    
    float range;
    float shootSpeed;
    float secondsSinceShot;
};

#endif
