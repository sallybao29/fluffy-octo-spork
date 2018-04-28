#include "Flyer.hpp"

#define ACTIVE_TIME_MAX 5.0f

Flyer::Flyer() {}

Flyer::Flyer(float x, float y, float range)
: Entity(x, y, Rectangle(0.0f, 0.0f), ENTITY_FLYING), range(range), origin(x, y, 0.0f) {
    state = STATE_IDLE;
    speed = 0.7f;
    chaseTime = 0.0f;
    currentAction = ACTION_FLYING;
}

Flyer::~Flyer() {}

bool Flyer::InRange(const Entity& entity) const {
    return distance(position, entity.position) < range;
}

void Flyer::Update(Entity& target, float elapsed) {
    switch (state) {
        case STATE_IDLE:
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            
            if (InRange(target)) {
                state = STATE_CHASING;
            }
            break;
        case STATE_CHASING:
            velocity.x = position.x - target.position.x > 0 ? -speed : speed;
            velocity.y = position.y - target.position.y > 0 ? -speed : speed;
            
            if (!InRange(target)) {
                state = STATE_RETURNING;
            }
            else if (target.currentAction == ACTION_DEFENDING) {
                if (chaseTime > ACTIVE_TIME_MAX) {
                    state = STATE_RETURNING;
                    chaseTime = 0.0f;
                }
                else
                    chaseTime += elapsed;
            }
            break;
        case STATE_RETURNING:
            velocity.x = position.x - origin.x > 0 ? -speed : speed;
            velocity.y = position.y - origin.y > 0 ? -speed : speed;
            
            if (InRange(target) && target.currentAction != ACTION_DEFENDING) {
                state = STATE_CHASING;
            }
            else if (distance(position, origin) < 0.0001f) {
                state = STATE_IDLE;
            }
            break;
        default:
            break;
    }
}
