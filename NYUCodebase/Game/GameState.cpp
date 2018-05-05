#include <unordered_set>
#include <sstream>
#include "GameState.hpp"
#include "Flyer.hpp"
#include "Floater.hpp"

#define ACCELERATION 20.0f
#define VELOCITY_X 0.7f
#define JUMP_VELOCITY 4.5f
#define FRICTION 0.6f
#define GRAVITY 4.9f

#define COLLIDE_X 0
#define COLLIDE_Y 1

#define DELTA 0.00001f

#define KEY 101
#define LOCK 128
#define DOOR 127

#define FLUID_RED 234
#define FLUID_BROWN 235

std::unordered_set<unsigned int> solidTiles =
{
    // Red world
    0, 1, 2, 3, 4, 5, 6, 7, 8, 22, 27, 28, 29, 30, 44, 49, 50, 51, 52, 193,
    // Yellow world
    66, 67, 68, 69, 70, 71, 72, 73, 74, 88, 93, 94, 95, 96, 110, 115, 116, 117, 118, 128, 192, 232, 254,
};

/*----------------------------------- Initialization/Setup ------------------------------------------*/
GameState::GameState(ShaderProgram* program) : shader(program), level(1), map(nullptr), lives (5) {}

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
}

void GameState::PlaceEntity(std::string type, float x, float y) {
    float entityX = x * map->tileSize + map->tileSize / 2;
    float entityY = y * -map->tileSize - map->tileSize / 2;
    
    // Player entity
    if (type == "playerBlue") {
        // Create entity at position (entityX, entityY)
        Entity* entity = new Entity(entityX, entityY, Rectangle(0.3, 0.3));
        entities.push_back(entity);
        
        player = entity;
        player->currentAction = ACTION_WALKING;
        player->entityType = ENTITY_PLAYER;
        player->isStatic = false;
        
        // Walking
        player->AddAnimation(ACTION_WALKING, "playerBlue_walk", 3, LOOP_REVERSE, 3);
        player->animations[ACTION_WALKING]->SetSpeed(20);
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
    else if (type == "enemyFlying") {
        // Create entity at position (entityX, entityY)
        Flyer* entity = new Flyer(entityX, entityY, 1.5f);
        // Offset gravity so it can fly
        entity->acceleration.y = GRAVITY;
        entity->isStatic = false;
        entities.push_back(entity);

        entity->AddAnimation(ACTION_FLYING, "enemyFlying", 3.5, LOOP_REVERSE, 3);
        entity->animations[ACTION_FLYING]->SetSpeed(30);
    }
    // Floating enemy
    else if (type == "enemyFloating") {
        // Create entity at position (entityX, entityY)
        Floater* entity = new Floater(entityX, entityY, 1.6f, 0.4f, map->tileSize * 5);
        // Offset gravity so it can float
        entity->acceleration.y = GRAVITY;
        entity->isStatic = true;
        entities.push_back(entity);

        entity->AddAnimation(ACTION_DEFENDING, "enemyFloating_3", 3.5, LOOP_NONE);
        entity->AddAnimation(ACTION_ATTACKING, "enemyFloating_1", 3.5, LOOP_NONE);
    }
    // Walking enemy
    else if (type == "enemyWalking") {
        // Create entity at position (entityX, entityY)
        Entity* entity = new Entity(entityX, entityY, Rectangle(0.3, 0.3));
        entity->velocity.x = VELOCITY_X;
        entity->isStatic = false;
        entities.push_back(entity);
        
        entity->entityType = ENTITY_WALKING;
        entity->currentAction = ACTION_WALKING;
        entity->AddAnimation(ACTION_WALKING, type, 3.5, LOOP_REVERSE, 3);
        entity->animations[ACTION_WALKING]->SetSpeed(30);
    }
    // Block
    else if (type == "blockBrown") {
        Entity* entity = new Entity(entityX, entityY, Rectangle(0.3, 0.3));
        //blocks.push_back(entity);
        entities.push_back(entity);
        entity->entityType = ENTITY_BLOCK;
        entity->currentAction = ACTION_NONE;
        entity->isStatic = true;
        entity->AddAnimation(ACTION_NONE, type, 2.5, LOOP_NONE);
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
    
    // Create entities
    for (size_t i = 0; i < entities.size(); i++) {
        delete entities[i];
    }
    entities.clear();
    
    for (int i = 0; i < map->entities.size(); i++) {
        PlaceEntity(map->entities[i].type, map->entities[i].x, map->entities[i].y);
    }
    
    // Ensure that the player has been created
    if (player == nullptr) {
        assert(false);
    }
    
    // Reset animation time for transition to next level
    animationTime = 0.0f;
    
    // Reset keys to 0
    keyCount = 0;
}

void GameState::Reset() {
    level = 1;
    lives = 5;
    LoadLevel();
}

/*----------------------------------- Collision Resolution ------------------------------------------*/
bool GameState::ResolveCollisionY(Entity& entity, int x, int y, float size) {
    // Only resolve collision if tile is solid
    if (x < 0 || y < 0 ||
        x > map->mapWidth || y > map->mapHeight ||
        map->mapData[y][x] == 0 ||
        solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) return false;
    
    if (entity.entityType == ENTITY_PLAYER) {
        // Check if player unlocks lock
        if (map->mapData[y][x] - 1 == LOCK && keyCount) {
            map->mapData[y][x] = 0;
            keyCount--;
        }
    }
    
    // Move tile coordinates to center of tile
    float centerY = -y * size - size / 2;
    return entity.CollidesWithY(centerY, size);
}

bool GameState::ResolveCollisionX(Entity& entity, int x, int y, float size) {
    // Only resolve collision if tile is solid
    if (x < 0 || y < 0 ||
        x > map->mapWidth || y > map->mapHeight ||
        map->mapData[y][x] == 0 ||
        solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) return false;
    
    if (entity.entityType == ENTITY_PLAYER) {
        // Check if player unlocks lock
        if (map->mapData[y][x] - 1 == LOCK && keyCount) {
            map->mapData[y][x] = 0;
            keyCount--;
        }
    }
    
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

/*------------------------------------ Processing Input -------------------------------------------*/
void GameState::ProcessInput() {
    player->acceleration.y = 0.0f;
    player->acceleration.x = 0.0f;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            // Can only jump if ground below is solid
            // Defense form cannot move
            if (player->collidedBottom && player->currentAction != ACTION_DEFENDING) {
                if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                    player->velocity.y = JUMP_VELOCITY;
                    player->currentAction = ACTION_JUMPING;
                    // Reset the jumping animation
                    player->animations[ACTION_JUMPING]->Reset();
                    timer.start();
                }
            }
            if (event.key.keysym.scancode == SDL_SCANCODE_N) {
                // Skip to next level
                level++;
                LoadLevel();
            }
            if (event.key.keysym.scancode == SDL_SCANCODE_R) {
                // Reset entire game
                Reset();
            }
        }
    }
    // Walking
    // Defense form cannot move
    if (player->collidedBottom && player->currentAction != ACTION_DEFENDING) {
        if (keys[SDL_SCANCODE_RIGHT]) {
            player->velocity.x = VELOCITY_X;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            player->velocity.x = -VELOCITY_X;
        }
        else {
            player->velocity.x = 0.0f;
        }
        if (fabs(player->velocity.x) > 0 && keys[SDL_SCANCODE_A]) {
            player->acceleration.x = player->velocity.x > 0 ? ACCELERATION : -ACCELERATION;
        }
    }
    // If jumping, allow left/right velocity to be set
    else if (timer.isRunning()) {
        if (keys[SDL_SCANCODE_RIGHT]) {
            player->velocity.x = 2.0f * VELOCITY_X;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            player->velocity.x = -2.0f * VELOCITY_X;
        }
        // End of jump
        if (timer.isOver(0.3f)) {
            player->velocity.y = 0.0f;
            timer.reset();
        }
    }
    // Defending
    if (keys[SDL_SCANCODE_D]) {
        if (player->currentAction != ACTION_DEFENDING)
            player->previousAction = player->currentAction;
        player->currentAction = ACTION_DEFENDING;
    }
    else if (player->currentAction == ACTION_DEFENDING) {
        player->currentAction = ACTION_WALKING;
    }
}

/*--------------------------------------- Updating ------------------------------------------------*/
void GameState::UpdatePhysics(Entity& entity, float elapsed) {
    // Reset all contact flags
    entity.Update(elapsed);
    
    // Apply friction
    entity.velocity.x = lerp(entity.velocity.x, 0.0f, elapsed * FRICTION);
    entity.velocity.y = lerp(entity.velocity.y, 0.0f, elapsed * FRICTION);
    
    // Apply acceleration
    entity.velocity.x += entity.acceleration.x * elapsed;
    entity.velocity.y += entity.acceleration.y * elapsed;
    
    // Apply gravity
    entity.velocity.y -= GRAVITY * elapsed;
    
    // Apply y-axis velocity
    entity.position.y += entity.velocity.y * elapsed;
    CollideWithMap(entity, COLLIDE_Y);
    
    // Apply x-axis velocity
    entity.position.x += entity.velocity.x * elapsed;
    CollideWithMap(entity, COLLIDE_X);
}

void GameState::UpdateAnimation(Entity& entity, float elapsed) {
    float frameSpeed = elapsed;
    switch (entity.entityType) {
        case ENTITY_PLAYER:
            // If the player has just landed after jumping, change their animation back to walking
            if (player->collidedBottom && player->currentAction == ACTION_JUMPING) {
                player->currentAction = ACTION_WALKING;
            }
            // If player is walking
            if (player->currentAction == ACTION_WALKING) {
                if (player->velocity.x == 0 || !player->collidedBottom)
                    player->animations[player->currentAction]->Reset();
                else
                    // Speed of animation frame update is proportional to player's x velocity
                    frameSpeed = fabs(player->velocity.x) * elapsed;
            }
            else if (player->currentAction == ACTION_JUMPING) {
                // Speed of animation frame update is proportional to player's y velocity
                frameSpeed = fabs(player->velocity.y) * elapsed;
            }
            break;
        case ENTITY_WALKING:
            frameSpeed = fabs(entity.velocity.x) * frameSpeed;
            break;
        case ENTITY_FLYING:
            frameSpeed = 0.10 * frameSpeed;
            break;
        default:
            break;
    }
    
    // Advance the animation frame
    entity.animations[entity.currentAction]->NextFrame(frameSpeed);
    // Retrieve the sprite for the current frame
    SheetSprite* frame = entity.animations[entity.currentAction]->GetCurrentFrame();
    if (frame != entity.sprite) {
        entity.SetSprite(frame);
    }
}

void GameState::CheckForTurn(Entity& entity) {
    int midY = map->worldToTileCoordY(entity.position.y);
    
    if (entity.velocity.x < 0) {
        // Pick a point a little to the left of the entity
        int leftX = map->worldToTileCoordX(entity.position.x - (entity.shape->size.x / 2) - map->tileSize / 5);
        // If there's something solid to the left, turn around
        if (solidTiles.find(map->mapData[midY][leftX] - 1) != solidTiles.end() ||
            entity.collidedLeft) {
            entity.velocity.x = VELOCITY_X * 0.80;
        }
        // Otherwise keep going
        else
            entity.velocity.x = -VELOCITY_X * 0.80;
    }
    else {
        // Pick a point a little to the right of the entity
        int rightX = map->worldToTileCoordX(entity.position.x + (entity.shape->size.x / 2) + map->tileSize / 5);
        // If there's something solid to the right, turn around
        if (solidTiles.find(map->mapData[midY][rightX] - 1) != solidTiles.end() ||
            entity.collidedRight) {
            entity.velocity.x = -VELOCITY_X * 0.80;
        }
        // Otherwise turn around
        else
            entity.velocity.x = VELOCITY_X * 0.80;
    }
}

void GameState::loseLifeReturn () {
    lives --;
    
    //transport player back to beginning
    for (int i = 0 ; i < map -> entities.size(); i++) {
        if (map -> entities [i].type == "playerBlue") {
            player -> position.x = map -> entities[i].x * map->tileSize + map->tileSize / 2;
            player -> position.y = -map -> entities[i].y * map->tileSize - map->tileSize / 2;
            player -> velocity.x = 0.0f;
            break;
        }
    }
    
    // If player loses all lives, game over
    if (lives == 0) {
        mode = STATE_GAME_OVER;
    }
}

void GameState::Update(float elapsed) {
    for (size_t i = 0; i < entities.size(); i++) {
        Entity* entity = entities[i];
        switch (entity->entityType) {
            case ENTITY_PLAYER:
                // Enable wall bounce in ball form
                if (!player->collidedBottom && player->currentAction == ACTION_DEFENDING) {
                    if (player->collidedRight) {
                        player->velocity.x = -VELOCITY_X * 2.0f;
                        player->velocity.y = JUMP_VELOCITY * 0.75f;
                    }
                    else if (player->collidedLeft) {
                        player->velocity.x = VELOCITY_X * 2.0f;
                        player->velocity.y = JUMP_VELOCITY * 0.75f;
                    }
                }
                // Ground bounce in ball form
                else if (player->collidedBottom &&
                         player->currentAction == ACTION_DEFENDING &&
                         player->previousAction == ACTION_JUMPING) {
                    player->velocity.y = JUMP_VELOCITY * 1.2f;
                }
                break;
            case ENTITY_FLYING:
                ((Flyer*)entity)->Update(*player, elapsed);
                
                break;
            case ENTITY_FLOATING:
                ((Floater*)entity)->Update(*player, elapsed);
                // Remove bullets that hit solid objects on the map
                for (Bullet& bullet : ((Floater*)entity)->bullets) {
                    bullet.CollideWithMap(*map, solidTiles);
                }
                break;
            case ENTITY_WALKING:
                CheckForTurn(*entity);
                break;
            default:
                break;
        }
        UpdatePhysics(*entity, elapsed);
        UpdateAnimation(*entity, elapsed);
    }

    ResolveEntityCollisions();
    
    CheckTileEvent();
    
    // Keep player in bounds of map
    if (player->position.x - player->shape->size.x / 2 < 0) {
        player->position.x = player->shape->size.x / 2 + DELTA;
    }
    else if (player->position.x + player->shape->size.x / 2 > map->mapWidth * map->tileSize) {
        player->position.x = map->mapWidth * map->tileSize - player->shape->size.x / 2 - DELTA;
    }
    
    // Update animation (tweening)
    animationTime += elapsed;
    float alpha = mapValue(animationTime, 0.0f, animationEnd, 0.0f, 1.0f);
    shader->SetAlpha(lerp(0.0f, 1.0f, alpha));
}

void GameState::CheckTileEvent() {
    // Get player's tile coordinates
    int tileX, tileY, tileVal;
    map->worldToTileCoordinates(player->position.x, player->position.y, tileX, tileY);
    // Clamp tile value to one if indices are out of bounds
    if (tileX < 0 || tileY < 0 ||
        tileX > map->mapWidth || tileY > map->mapHeight ||
        map->mapData[tileY][tileX] == 0) tileVal = 0;
    else
        tileVal = map->mapData[tileY][tileX] - 1;
    
    // Collect key
    if (tileVal == KEY) {
        map->mapData[tileY][tileX] = 0;
        keyCount++;
    }
    // Check if player unlocks door to next level
    else if (keyCount && tileVal == DOOR) {
        if (level == 3) {
            mode = STATE_GAME_WON;
            return;
        }
        level++;
        LoadLevel();
    }
    // Check if player falls into fluid
    else if (tileVal == FLUID_RED || tileVal == FLUID_BROWN) {
        loseLifeReturn();
    }
}

void GameState::ResolveEntityCollision(Entity& one, Entity& two) {
    // Don't compare if one and two are the same object
    if (&one == &two) return;
    // Let dynamic entities of the same type ignore each other
    if (!one.isStatic && !two.isStatic && one.entityType == two.entityType) return;
    
    std::pair<float, float> penetration;
    bool collided = one.CollidesWith(two, penetration);
    if (!collided) return;
    
    // Static on static entity collision
    if (one.isStatic && two.isStatic) {
        // Adjust the top-most one by the full penetration
        if (penetration.second > 0.0)
            one.position.y += penetration.second;
        else
            two.position.y -= penetration.second;
    }
    // Dynamic on static entity collision
    else if (!one.isStatic && two.isStatic) {
        // Adjust the dynamic one by the full penetration
        one.position.x += penetration.first;
        one.position.y += penetration.second;
    }
    // Static on dynamic entity collision
    else if (one.isStatic && !two.isStatic) {
        // Adjust the dynamic one by the full penetration
        two.position.x -= penetration.first;
        two.position.y -= penetration.second;
    }
    // Dynamic on dynamic entity collision
    else if (!one.isStatic && !two.isStatic) {
        // Adjust the both by the half the penetration
        one.position.x += penetration.first * 0.5f;
        one.position.y += penetration.second * 0.5f;
        
        two.position.x -= penetration.first * 0.5f;
        two.position.y -= penetration.second * 0.5f;
        
        if (one.entityType == ENTITY_PLAYER || two.entityType == ENTITY_PLAYER) {
            if (player->currentAction != ACTION_DEFENDING) {
                loseLifeReturn();
            }
        }
    }
}

void GameState::ResolveEntityCollisions() {
    for (size_t i = 0; i < entities.size(); i++) {
        Entity& entityOne = *entities[i];
        for (size_t j = i; j < entities.size(); j++) {
            Entity& entityTwo = *entities[j];
            ResolveEntityCollision(entityOne, entityTwo);
        }
    }
    
    // Check player bullet collision
    for (size_t i = 0; i < entities.size(); i++) {
        Entity* entity = entities[i];
        if (entity->entityType == ENTITY_FLOATING) {
            Floater* floater = (Floater*)entity;
            // Check collision with bullets
            for (Bullet& bullet : floater->bullets) {
                bool collided = bullet.CollidesWith(*player);
                if (collided) {
                    if (player->currentAction != ACTION_DEFENDING) {
                        lives--;
                        if (!lives)
                            mode = STATE_GAME_OVER;
                        return;
                    }
                }
            }
        }
    }
}

/*------------------------------------------- Rendering ----------------------------------------------*/
void GameState::Render() {
    // Calculate bounds of scrolling
    float viewX = player->position.x;
    float viewY = player->position.y;
    
    // Bound left scrolling to left end of the map
    if (viewX - projection.x <= 0) {
        viewX = projection.x;
    }
    
    // Bound right scrolling to right end of the map
    if (viewX >= map->mapWidth * map->tileSize - projection.x) {
        viewX = map->mapWidth * map->tileSize - projection.x;
    }
    
    if (viewY <= -(map->mapHeight * map->tileSize - projection.y)) {
        viewY = -(map->mapHeight * map->tileSize - projection.y);
    }
    
    // Set view matrix to follow player
    viewMatrix.Identity();
    viewMatrix.Translate(-viewX, -viewY, 0.0f);
    shader->SetViewMatrix(viewMatrix);
    
    // Draw background
    RenderBackground(viewX, -(map->mapHeight * map->tileSize - projection.y));
    
    // Draw map
    modelMatrix.Identity();
    shader->SetModelMatrix(modelMatrix);
    map->Render(*shader);
    
    // Draw entities
    for (size_t i = 0; i < entities.size(); i++) {
        Entity& entity = *entities[i];
        // Apply squashing and squeezing based on magnitude of y velocity
        entity.scale.y = mapValue(fabs(entity.velocity.y), 0.0f, 5.0f, 1.0f, 1.2f);
        entity.scale.x = mapValue(fabs(entity.velocity.y), 5.0f, 0.0f, 0.8f, 1.0f);
        entity.Render(*shader);
    }
}

void GameState::RenderBackground(float viewX, float viewY) {
    // Draw background
    modelMatrix.Identity();
    modelMatrix.Scale(map->mapWidth, map->mapHeight, 1.0f);
    shader->SetModelMatrix(modelMatrix);
    std::stringstream stream;
    stream << "background_" << level;
    DrawTexture(*shader, textures[stream.str()]);
    
    // Draw background tiles
    modelMatrix.Identity();
    modelMatrix.SetPosition(viewX, viewY, 0.0f);
    modelMatrix.Scale(projection.x * 2, projection.y * 2, 1.0f);
    shader->SetModelMatrix(modelMatrix);
    stream.str ("");
    stream << "tiles_" << level;
    DrawTexture(*shader, textures[stream.str()]);
    
    // Draw background hills
    modelMatrix.Identity();
    modelMatrix.SetPosition(viewX + projection.x * 0.25, viewY + projection.y * 0.375 , 0.0f);
    modelMatrix.Scale(projection.x * 2.5, projection.y * 2.5, 1.0f);
    shader->SetModelMatrix(modelMatrix);
    stream.str ("");
    stream << "hills_" << level;
    DrawTexture(*shader, textures[stream.str()]);
}

