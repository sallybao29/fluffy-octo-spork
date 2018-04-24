#ifndef GameState_hpp
#define GameState_hpp

#include <stdio.h>
#include "Vector3.hpp"
#include "ShaderProgram.h"
#include "FlareMap.hpp"
#include "Entity.hpp"
#include "GameUtilities.hpp"
#include "SheetSprite.hpp"

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
    
    ShaderProgram* shader;
    Matrix modelMatrix;
    Matrix viewMatrix;

    FlareMap* map;
    
    Entity* player;
    std::vector<Entity*> entities;
    
    int level;
};

#endif
