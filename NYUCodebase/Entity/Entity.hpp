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

/*!
 * @discussion Represents an entity in a game
 * @warning A copy constructor has not been implemented yet, so please don't ever make a copy
 * of an entity.
 */
class Entity {
public:
    Entity();
    
    /*!
     * @discussion Initializes an untextured entity with the given position, shape, and type
     * @param x The x coordinate of the entity's position
     * @param y The y coordinate of the entity's position
     * @param shape The shape of the entity's hit "box"
     * @param type The type of entity
     */
    Entity(float x, float y, const Shape& shape, EntityType type);
    
    /*!
     * @discussion Intializes a textured entity with the given position, sprite, and type
     * @param x The x coordinate of the entity's position
     * @param y The y coordinate of the entity's position
     * @param sprite The sprite to render for the entity
     * @param type The type of entity
     */
    Entity(float x, float y, SheetSprite *sprite, EntityType type);
    
    ~Entity();
    
    /*!
     * @discussion Advances the entity's state by a certain amount of time. Resets contact flags
     * @param elapsed The amount of time that has elapsed since the last update
     */
    void Update(float elapsed);
    
    /*!
     * @discussion Renders the entity after applying all of its transformations. Entities without a
     * sprite will be rendered as plain polygons based on their shape property
     * @param program The shader program to use when rendering
     */
    void Render(ShaderProgram& program);
    
    /*!
     * @discussion Assigns the entity a new sprite and adjusts its size accordingly
     * @param newSprite A pointer to the new sheet sprite
     */
    void SetSprite(SheetSprite* newSprite);
    
    /*!
     * @discussion Sets the color of the entity. For entities that have no texture and are drawn via
     * an untextured shader program
     * @param r A normalized value representing the red component of the color
     * @param g A normalized value representing the green component of the color
     * @param b A normalized value representing the blue component of the color
     * @param a A normalized value representing the transparency of the color
     */
    void SetColor(float r, float g, float b, float a);
    
    /*!
     * @discussion Updates the entity's transformation matrix to reflect the most recent changes to
     * position, scale, and rotation
     */
    void UpdateMatrix();
    
    /*!
     * @discussion Resolves collision between two entities if one occurs
     * @param other The entity being collided against
     * @return Whether the collision occurred
     */
    bool CollidesWith(Entity& other);
    
    /// The sprite to render for the entity
    SheetSprite* sprite = nullptr;
    
    /// The position of the entity in world coordinates
    Vector3 position;
    
    /// The scale factor applied to the entity
    Vector3 scale;
    
    /// The velocity of the entity
    Vector3 velocity;
    
    /// The acceleration of the entity
    Vector3 acceleration;
    
    /// The bounding shape that represents the entity's hit "box"
    Shape* shape;
    
    /// The entity's transformation matrix
    Matrix modelMatrix;
    
    /// The amount that the entity has been rotated by
    float rotation = 0.0f;
    
    /// The type of the entity
    EntityType entityType;
    
    /// The color of the entity if it is an untextured polygon
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    /// Top contact flag
    bool collidedTop;
    /// Bottom contact flag
    bool collidedBottom;
    /// Left contact flag
    bool collidedLeft;
    /// Right contact flag
    bool collidedRight;
};
#endif
