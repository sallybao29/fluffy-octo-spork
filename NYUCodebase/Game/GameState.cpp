#include <unordered_set>
#include <sstream>
#include "GameState.hpp"

#define ACCELERATION 0.7f
#define FRICTION 0.6f
#define GRAVITY 1.5f

#define DELTA 0.00001f

GameState::GameState(ShaderProgram* program) : shader(program), level(1), map(nullptr) {}

GameState::~GameState() {
    if (map != nullptr) {
        delete map;
        map = nullptr;
    }
    for (size_t i = 0; i < entities.size(); i++) {
        delete entities[i];
    }
    entities.clear();
}

void GameState::Initialize() {
    LoadLevel();
    
    // Create entities
    for (int i = 0; i < map->entities.size(); i++) {
        PlaceEntity(map->entities[i].type, map->entities[i].x, map->entities[i].y);
    }
    
    if (player == nullptr) {
        assert(false);
    }
}

void GameState::PlaceEntity(std::string type, float x, float y) {
    float entityX = x * map->tileSize + map->tileSize / 2;
    float entityY = y * -map->tileSize - map->tileSize / 2;
    
    Entity* entity = new Entity(entityX, entityY, Rectangle(0.3, 0.3));
    entities.push_back(entity);
    
    // Player entity
    if (strcmp(type.data(), "Player") == 0) {
        player = entities.back();
        player->previousAction = ACTION_WALKING;
        player->currentAction = ACTION_WALKING;
        player->entityType = ENTITY_PLAYER;
        
        // Add animations
        // Walking
        player->AddAnimation(ACTION_WALKING, "playerBlue_walk", 3, LOOP_REVERSE, 3);
        player->animations[ACTION_WALKING]->SetSpeed(25);
        // Defending
        player->AddAnimation(ACTION_DEFENDING, "playerBlue_roll", 3, LOOP_NONE);
        // Swimming
        player->AddAnimation(ACTION_SWIMMING, "playerBlue_swim", 3, LOOP_REPEAT);
        player->animations[ACTION_SWIMMING]->SetSpeed(25);
        // Jumping
        player->AddAnimation(ACTION_JUMPING, "playerBlue_up", 3, LOOP_ONCE);
        player->animations[ACTION_JUMPING]->SetSpeed(30);
    }
}

void GameState::LoadLevel() {
    if (map != nullptr) {
        delete map;
    }
    // Get the map for the level
    std::stringstream stream;
    stream << RESOURCE_FOLDER"Resources/Levels/level_" << level << ".txt";
    
    // Load a new map
    map = new FlareMap(0.3f);
    map->Load(stream.str());
    map->SetSpriteSheet(textures[TILES], 22, 12);
}

void GameState::Reset() {
    if (level == 1) {
        // reset code here, don't need to reload the map
    }
    else {
        level = 1;
        LoadLevel();
    }
}

void GameState::ProcessInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                player->previousAction = player->currentAction;
                player->currentAction = ACTION_JUMPING;
                player->velocity.y = 1.0f;
            }
        }
        else if (event.type == SDL_KEYUP) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                player->currentAction = player->previousAction;
                player->previousAction = ACTION_DEFENDING;
            }
        }
    }
    
    if (keys[SDL_SCANCODE_RIGHT]) {
        player->velocity.x = 0.3f;
    }
    else if (keys[SDL_SCANCODE_LEFT]) {
        player->velocity.x = -0.3f;
    }
    else {
        player->velocity.x = 0.0f;
    }
    if (keys[SDL_SCANCODE_SPACE]) {
        if (player->currentAction != ACTION_DEFENDING) {
            player->previousAction = player->currentAction;
            player->currentAction = ACTION_DEFENDING;
        }
    }
}

void GameState::Update(float elapsed) {
    player->Update(elapsed);
}

void GameState::Render() {
    modelMatrix.Identity();
    shader->SetModelMatrix(modelMatrix);
    
    // Calculate bounds of scrolling
    float viewX = player->position.x;
    float viewY = -(map->mapHeight * map->tileSize - projection.y);
    
    // Bound left scrolling to left end of the map
    if (viewX - projection.x <= 0) {
        viewX = projection.x;
    }
    
    // Bound right scrolling to right end of the map
    if (viewX + projection.x >= map->mapWidth * map->tileSize) {
        viewX = map->mapWidth * map->tileSize - projection.x;
    }
    
    // Set view matrix to follow player
    viewMatrix.Identity();
    viewMatrix.Translate(-viewX, -viewY, 0.0f);
    shader->SetViewMatrix(viewMatrix);

    // Draw map
    map->Render(*shader);
    
    /*
    // Draw entities
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i]->Render(*shader);
    }*/
    player->Render(*shader);
}


