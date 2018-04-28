#include <unordered_set>
#include <sstream>
#include "GameState.hpp"

#define ACCELERATION 0.7f
#define MAX_VELOCITY_X 3.0f
#define JUMP_VELOCITY 3.5f
#define FRICTION 0.6f
#define GRAVITY 4.9f

#define COLLIDE_X 0
#define COLLIDE_Y 1

#define DELTA 0.00001f

std::unordered_set<unsigned int> solidTiles =
{
    // Red world
    0, 1, 2, 3, 4, 5, 6, 7, 8, 22, 27, 28, 29, 30, 44, 49, 50, 51, 52, 234, 256
};

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
    
    // Create entity at position (entityX, entityY)
    Entity* entity = new Entity(entityX, entityY, Rectangle(0.3, 0.3));
    entities.push_back(entity);
    
    // Player entity
    if (strcmp(type.data(), "playerBlue") == 0) {
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
        entity->AddAnimation(ACTION_FLYING, "enemyFlying", 3.5, LOOP_REVERSE, 3);
        entity->animations[ACTION_FLYING]->SetSpeed(30);
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
        entity->animations[ACTION_WALKING]->SetSpeed(30);
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
    player->acceleration.x = 0;
    player->acceleration.y = 0;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            // Can only jump if ground below is solid
            if (player->collidedBottom) {
                if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                    player->velocity.y = JUMP_VELOCITY;
                    timer.start();
                }
            }
        }
    }
    // Can only accelerate when standing on something
    if (player->collidedBottom) {
        if (keys[SDL_SCANCODE_RIGHT] && player->velocity.x < MAX_VELOCITY_X) {
            player->acceleration.x = ACCELERATION;
        }
        else if (keys[SDL_SCANCODE_LEFT] && player->velocity.x > -MAX_VELOCITY_X) {
            player->acceleration.x = -ACCELERATION;
        }
    }
    // If jumping, allow left/right velocity to be set
    else if (timer.isRunning()) {
        if (keys[SDL_SCANCODE_RIGHT]) {
            player->velocity.x = 1.0f;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            player->velocity.x = -1.0f;
        }
        // End of jump
        if (timer.isOver(0.3f)) {
            player->velocity.y = 0.0f;
            timer.reset();
        }
    }
}

bool GameState::ResolveCollisionY(Entity& entity, int x, int y, float size) {
    // Only resolve collision if tile is solid
    if (x < 0 || y < 0 ||
        map->mapData[y][x] == 0 ||
        solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) return false;
    
    // Move tile coordinates to center of tile
    float centerY = -y * size - size / 2;
    return entity.CollidesWithY(centerY, size);
}

bool GameState::ResolveCollisionX(Entity& entity, int x, int y, float size) {
    // Only resolve collision if tile is solid
    if (x < 0 || y < 0 ||
        map->mapData[y][x] == 0 ||
        solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) return false;
    
    // Move tile coordinates to center of tile
    float centerX = x * size + size / 2;
    return entity.CollidesWithX(centerX, size);
}

void GameState::CollideWithMap(Entity& entity, int direction) {
    if (direction == COLLIDE_Y) {
        // Sample 2 points along the width of the entity;
        int midX1 = map->worldToTileCoordX(entity.position.x - entity.shape->size.x / 4);
        int midX2 = map->worldToTileCoordX(entity.position.x + entity.shape->size.x / 4);
        
        if (entity.velocity.y > 0) {
            int topY = map->worldToTileCoordY(entity.position.y + entity.shape->size.y / 2);
            if (!ResolveCollisionY(entity, midX1, topY, map->tileSize))
                ResolveCollisionY(entity, midX2, topY, map->tileSize);
        }
        else {
            int botY = map->worldToTileCoordY(entity.position.y - entity.shape->size.y / 2);
            if (!ResolveCollisionY(entity, midX1, botY, map->tileSize))
                ResolveCollisionY(entity, midX2, botY, map->tileSize);
        }
    }
    else if (direction == COLLIDE_X) {
        if (entity.velocity.x > 0) {
            int rightX, rightY;
            map->worldToTileCoordinates(entity.position.x + entity.shape->size.x / 2, entity.position.y, rightX, rightY);
            ResolveCollisionX(entity, rightX, rightY, map->tileSize);
        }
        else {
            int leftX, leftY;
            map->worldToTileCoordinates(entity.position.x - entity.shape->size.x / 2, entity.position.y, leftX, leftY);
            ResolveCollisionX(entity, leftX, leftY, map->tileSize);
        }
    }
}

void GameState::Update(float elapsed) {
    for (int i = 0; i < entities.size(); i++) {
        Entity* entity = entities[i];
        
        // Reset all contact flags
        entity->Update(elapsed);
        
        // Apply friction
        entity->velocity.x = lerp(entity->velocity.x, 0.0f, elapsed * FRICTION);
        entity->velocity.y = lerp(entity->velocity.y, 0.0f, elapsed * FRICTION);
        
        // Apply acceleration
        entity->velocity.x += entity->acceleration.x * elapsed;
        entity->velocity.y += entity->acceleration.y * elapsed;
        
        // Apply gravity
        entity->velocity.y -= GRAVITY * elapsed;
        
        // Apply y-axis velocity
        entity->position.y += entity->velocity.y * elapsed;
        CollideWithMap(*entity, COLLIDE_Y);
        
        // Apply x-axis velocity
        entity->position.x += entity->velocity.x * elapsed;
        CollideWithMap(*entity, COLLIDE_X);
        
        SheetSprite* frame = entity->animations[entity->currentAction]->GetCurrentFrame();
        if (frame != entity->sprite) {
            entity->SetSprite(frame);
        }
    }
    
    // Keep player in bounds of map
    if (player->position.x - player->shape->size.x / 2 < 0) {
        player->position.x = player->shape->size.x / 2 + DELTA;
    }
    else if (player->position.x + player->shape->size.x / 2 > map->mapWidth * map->tileSize) {
        player->position.x = map->mapWidth * map->tileSize - player->shape->size.x / 2 - DELTA;
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
    
    // Draw background
    modelMatrix.Identity();
    modelMatrix.Scale (map->mapWidth, map->mapHeight, 1.0f);
    shader -> SetModelMatrix(modelMatrix);
    std::stringstream stream;
    stream << "background_" << level ;
    DrawTexture(*shader, textures[stream.str()]);
    
    // Draw background tiles
    modelMatrix.Identity();
    modelMatrix.SetPosition (viewX, viewY, 0.0f);
    modelMatrix.Scale (projection.x * 2, projection.y *2  , 1.0f);
    shader -> SetModelMatrix( modelMatrix);
    stream.str ("");
    stream << "tiles_" << level ;
    DrawTexture(*shader, textures[stream.str()]);
    
    // Draw background hills
    modelMatrix.Identity ();
    modelMatrix.SetPosition (viewX + projection.x * 0.25, viewY + projection.y * 0.375 , 0.0f);
    modelMatrix.Scale (projection.x * 2.5, projection.y * 2.5, 1.0f);
    shader -> SetModelMatrix(modelMatrix);
    stream.str ("");
    stream << "hills_" << level ;
    DrawTexture(*shader, textures[stream.str()]);
    
    // Draw map
    modelMatrix.Identity ();
    shader -> SetModelMatrix(modelMatrix);
    map->Render(*shader);
    
    // Draw entities
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i]->Render(*shader);
    }
}



