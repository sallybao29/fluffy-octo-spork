#include <unordered_set>
#include <sstream>
#include "GameState.hpp"
#include "Flyer.hpp"
#include "Floater.hpp"

#define ACCELERATION 15.0f
#define VELOCITY_X 0.5f
#define JUMP_VELOCITY 4.5f
#define FRICTION 0.6f
#define GRAVITY 4.9f

#define COLLIDE_X 0
#define COLLIDE_Y 1

#define DELTA 0.00001f

std::unordered_set<unsigned int> solidTiles =
{
    // Red world
    0, 1, 2, 3, 4, 5, 6, 7, 8, 22, 27, 28, 29, 30, 44, 49, 50, 51, 52, 234, 256,
    // Green world
    66, 67, 68, 69, 70, 71, 72, 73, 74, 88, 93, 94, 95, 96, 110, 115, 116, 117, 118, 232, 254
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
        entities.push_back(entity);

        entity->AddAnimation(ACTION_FLYING, "enemyFlying", 3.5, LOOP_REVERSE, 3);
        entity->animations[ACTION_FLYING]->SetSpeed(30);
    }
    // Floating enemy
    else if (type == "enemyFloating") {
        // Create entity at position (entityX, entityY)
        Floater* entity = new Floater(entityX, entityY, 1.5f, 0.3f, map->tileSize * 5);
        // Offset gravity so it can float
        entity->acceleration.y = GRAVITY;
        entities.push_back(entity);

        entity->AddAnimation(ACTION_DEFENDING, "enemyFloating_3", 3.5, LOOP_NONE);
        entity->AddAnimation(ACTION_ATTACKING, "enemyFloating_1", 3.5, LOOP_NONE);
    }
    // Walking enemy
    else if (type == "enemyWalking") {
        // Create entity at position (entityX, entityY)
        Entity* entity = new Entity(entityX, entityY, Rectangle(0.3, 0.3));
        entity->velocity.x = VELOCITY_X;
        entities.push_back(entity);
        
        entity->entityType = ENTITY_WALKING;
        entity->currentAction = ACTION_WALKING;
        entity->AddAnimation(ACTION_WALKING, type, 3.5, LOOP_REVERSE, 3);
        entity->animations[ACTION_WALKING]->SetSpeed(30);
    }
    else if (type == "enemySwimming") {
        // Create entity at position (entityX, entityY)
        Entity* entity = new Entity(entityX, entityY, Rectangle(0.3, 0.3));
        entity->velocity.x = VELOCITY_X;
        entities.push_back(entity);
        
        entity->entityType = ENTITY_SWIMMING;
        entity->currentAction = ACTION_SWIMMING;
        entity->AddAnimation(ACTION_SWIMMING, type, 3.5, LOOP_REVERSE, 3);
        entity->animations[ACTION_SWIMMING]->SetSpeed(30);
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
            else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
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
            player->velocity.x = 2.5f * VELOCITY_X;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            player->velocity.x = -2.5f * VELOCITY_X;
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
        case ENTITY_SWIMMING:
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
    int boty = map->worldToTileCoordY(entity.position.y - entity.shape->size.y / 2 - map->tileSize / 4);
    
    if (entity.velocity.x < 0) {
        int leftx = map -> worldToTileCoordX(entity.position.x - (entity.shape->size.x / 2) - map->tileSize / 4);
        
        if (map -> mapData [boty][leftx] == 0 ||
            solidTiles.find(map->mapData[boty][leftx] - 1) == solidTiles.end() ||
            entity.collidedLeft) {
            entity.velocity.x = VELOCITY_X * 0.75;
        }
        else {
            entity.velocity.x = -VELOCITY_X * 0.75;
        }
    
    }
    else {
        int rightx = map -> worldToTileCoordX(entity.position.x + (entity.shape->size.x / 2) + map->tileSize / 4);
         if (map -> mapData [boty][rightx] == 0 ||
             solidTiles.find(map->mapData[boty][rightx] - 1) == solidTiles.end() || entity.collidedRight) {
             entity.velocity.x = -VELOCITY_X * 0.75;
         }
         else {
             entity.velocity.x = VELOCITY_X * 0.75;
        }
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
                // Flawed: Only bounces if a jump was initiated
                // If player transitions from defense form to walking mid-jump, ground bounce will fail
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
    
    // Check collision of player against enemy
    for (size_t i = 0; i < entities.size(); i++) {
        Entity* entity = entities[i];
        if (entity == player) continue;
        std::pair<float, float> penetration;
        bool collided = player->CollidesWith(*entity, penetration);
        if (collided) {
            // Adjust player by collision amount
            if (player->currentAction == ACTION_DEFENDING) {
                player->position.x += penetration.first;
                player->position.y += penetration.second;
                
                player->velocity.y = 0.0f;
            }
            else {
                loseLifeReturn();
            }
        }
        if (entity->entityType == ENTITY_FLOATING) {
            Floater* floater = (Floater*) entity;
            // Check collision with bullets
            for (Bullet& bullet : floater -> bullets) {
                bool collided = bullet.CollidesWith(*player);
                if (collided) {
                    // Play sound
                    if (player -> currentAction != ACTION_DEFENDING) {
                        lives--;
                        
                        if (!lives ) {
                            mode = STATE_GAME_OVER;
                        }
                        return;
                    }
                }
            }
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
        entities[i]->Render(*shader);
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

