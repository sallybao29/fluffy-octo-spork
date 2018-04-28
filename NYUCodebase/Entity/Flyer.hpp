#ifndef Flyer_hpp
#define Flyer_hpp

#include <stdio.h>
#include "Entity.hpp"

class Flyer : public Entity {
public:
    Flyer();
    Flyer(float x, float y, float range);
    ~Flyer();
    
    bool InRange(const Entity& entity) const;
    void Update(Entity& target, float elapsed);
    
private:
    Vector3 origin;
    float range;
    float speed;
    float chaseTime;
    AIState state;
};

#endif
