#ifndef Flyer_hpp
#define Flyer_hpp

#include <stdio.h>
#include "Entity.hpp"

/*!
 * @discussion An entity that flies in place but chases targets in range
 */
class Flyer : public Entity {
public:
    Flyer();
    Flyer(float x, float y, float range);
    ~Flyer();
    
    /*!
     * @discussion Checks whether the target is in the flyer's range
     * @param entity The target entity
     * @return Whether the target is in range
     */
    bool InRange(const Entity& entity) const;
    
    /*!
     * @discussion Updates the AI state of the flyer
     * @param target The target entity
     * @param elapsed The amount of time that has elapsed
     */
    void Update(Entity& target, float elapsed);
    
    /*!
     * @discussion Moves the flyer from its current position to the target position
     * @param pos The target position
     */
    void Approach(Vector3 pos);
    
private:
    /// The flyer's base position
    Vector3 origin;
    /// The range of the flyer's detection
    float range;
    /// The speed of the flyer
    float speed;
    /// How long the flyer will hang around if it can't detect the target anymore
    float chaseTime;
    /// The AI state
    AIState state;
};

#endif
