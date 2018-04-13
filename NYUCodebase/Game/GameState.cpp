#include <unordered_set>
#include "GameState.hpp"

#define ACCELERATION 0.7f
#define FRICTION 0.6f
#define GRAVITY 1.5f

#define DELTA 0.00001f

GameState::GameState() {}

void GameState::Initialize(ShaderProgram* program, FlareMap* map) {
    this->shader = program;
    this->map = map;
}

void GameState::Reset() {
}

void GameState::ProcessInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
}

void GameState::Update(float elapsed) {
}

void GameState::Render() {
    modelMatrix.Identity();
    shader->SetModelMatrix(modelMatrix);

    // Draw map
    map->Render(*shader);
}


