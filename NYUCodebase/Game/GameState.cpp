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
    // Ensure that the player has been created
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
    if (strcmp(type.data(), "player") == 0) {
        player = entity;
        player->currentAction = ACTION_WALKING;
        player->entityType = ENTITY_PLAYER;
        
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
    // Flying enemy
    else if (strcmp(type.data(), "enemyFlying") == 0) {
        entity->entityType = ENTITY_FLYING;
        entity->currentAction = ACTION_FLYING;
        entity->AddAnimation(ACTION_FLYING, type, 3.5, LOOP_REVERSE, 3);
    }
    // Floating enemy
    else if (strcmp(type.data(), "enemyFloating") == 0) {
        entity->entityType = ENTITY_FLOATING;
        entity->currentAction = ACTION_DEFENDING;
        entity->AddAnimation(ACTION_DEFENDING, "enemyFloating_3", 3.5, LOOP_NONE);
        entity->AddAnimation(ACTION_ATTACKING, "enemyFloating_1", 3.5, LOOP_NONE);
    }
    // Walking enemy
    else if (strcmp(type.data(), "enemyWalking") == 0) {
        entity->entityType = ENTITY_WALKING;
        entity->currentAction = ACTION_WALKING;
        entity->AddAnimation(ACTION_WALKING, type, 3.5, LOOP_REVERSE, 3);
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
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i]->Update(elapsed);
        
        switch (entities[i]->entityType) {
            case ENTITY_WALKING:
                if (player->currentAction == ACTION_DEFENDING) {
                    entities[i]->velocity.x = 0.0f;
                    continue;
                }
                if (player->position.x > entities[i]->position.x) {
                    entities[i]->velocity.x = 0.2f;
                }
                else {
                    entities[i]->velocity.x = -0.2f;
                }
                break;
            default:
                break;
        }
    }
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
    
    // Draw entities
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i]->Render(*shader);
    }
}


