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
    GameState();
    void Initialize(ShaderProgram* program, FlareMap* map);
    void Reset();
    void ProcessInput();
    void Update(float elapsed);
    void Render();
    
private:    
    ShaderProgram* shader;
    Matrix modelMatrix;
    Matrix viewMatrix;

    FlareMap* map;
    
    Entity* player;
    std::vector<Entity> entities;
};

#endif
