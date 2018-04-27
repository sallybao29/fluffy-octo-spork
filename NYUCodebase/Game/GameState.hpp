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
     * @discussion Checking collision between solids and player
     */
    
    void Collision ();
    
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
    
    ShaderProgram* shader;
    Matrix modelMatrix;
    Matrix viewMatrix;

    Matrix backgroundMatrix;
    
    FlareMap* map;
    
    Entity* player;
    std::vector<Entity*> entities;
    
    int level;
    
    Timer timer;
};

#endif
