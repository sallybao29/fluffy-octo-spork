#ifndef GameState_hpp
#define GameState_hpp

#include <stdio.h>
#include "Vector3.hpp"
#include "ShaderProgram.h"
#include "FlareMap.hpp"
#include "Entity.hpp"
#include "GameUtilities.hpp"
#include "SheetSprite.hpp"
#include "Timer.hpp"

class GameState {
public:
    GameState(ShaderProgram* program);
    ~GameState();
    
    /*!
     * @discussion Initializes the game level
     */
    void Initialize();
    
    /*!
     * @discussion Loads a map from a file based on the current level
     */
    void LoadLevel();
    
    /*!
     * @discussion Resets the game to level one
     */
    void Reset();
    
    /*!
     * @discussion Processes user input
     */
    void ProcessInput();
    
    /*!
     * @discussion Updates the game state by an elapsed amount of time. New entity positions are determined
     * and collisions are resolved here.
     * @param elapsed The amount of time that has passed
     */
    void Update(float elapsed);
    
    /*!
     * @discussion Renders the tile map and all objects in the game
     */
    void Render();
    
private:
    /*!
     * @abstract Creates an entity of the given type at the given position
     * @discussion Creates an entity based on its type and loads all of its animation frames
     * @param type The type of the entity parsed from a FlareMap
     * @param x The entity's x position on the map (in tiles)
     * @param y The entity's y position on the map (in tiles)
     */
    void PlaceEntity(std::string type, float x, float y);
    /*!
     * @discussion Checks for and resolves collision in either the x or y direction for an entity
     * @param entity The entity colliding against the map
     * @param direction The direction of collision
     */
    void CollideWithMap(Entity& entity, int direction);
    
    /*!
     * @discussion Resolves collision in the y direction for an entity colliding against a given tile
     * @param entity The entity colliding against the map
     * @param x The x coordinate of the tile (in tile coordinates)
     * @param y The y coordinate of the tile (in tile coordinates)
     * @param size The size of the tile
     * @return Whether the collision was resolved
     */
    bool ResolveCollisionY(Entity& entity, int x, int y, float size);
    
    /*!
     * @discussion Resolves collision in the x direction for an entity colliding against a given tile
     * @param entity The entity colliding against the map
     * @param x The x coordinate of the tile (in tile coordinates)
     * @param y The y coordinate of the tile (in tile coordinates)
     * @param size The size of the tile
     * @return Whether the collision was resolved
     */
    bool ResolveCollisionX(Entity& entity, int x, int y, float size);
    
    /*!
     * @discussion Advances the physics of the world by the elapsed amount of time and resolves
     * collisions for entities
     * @param entity The entity to update
     * @param elapsed The amount of time that has elapsed
     */
    void UpdatePhysics(Entity& entity, float elapsed);
    
    /*!
     * @discussion Updates the animation for the entity
     * @param entity The entity to update
     * @param elapsed The amount of time that has elapsed
     */
    void UpdateAnimation(Entity& entity, float elapsed);
    
    /*!
     * @discussion Draws the background of the level
     * @param viewX The x coordinate to draw the background at
     * @param viewY The y coordinate to draw the background at
     */
    void RenderBackground(float viewX, float viewY);
    
    void CheckForTurn(Entity& entity);
    
    /*!
     * @discussion Checks if any events are triggered by the value of the current tile the player is standing at.
     */
    void CheckTileEvent();
    
    /*!
     *@discussion Returns player back to starting position upon losing a life
     */
    void loseLifeReturn ();
    
    void ResolveEntityCollisions();
    
    void ResolveEntityCollision(Entity& one, Entity& two);
    
    ShaderProgram* shader;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    FlareMap* map;
    
    Entity* player;
    std::vector<Entity*> entities;
    
    int level;
    
    int lives;
    
    int keyCount = 0;
    
    Timer timer;
    
    float animationTime = 0.0f;
    float animationEnd = 2.0f;
};

#endif
