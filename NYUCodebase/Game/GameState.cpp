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
    /*
    // Generate subtexture name
    std::stringstream subTextureName;
    subTextureName << type << ".png";
    
    // Get sprite data from texture atlas
    float spriteX, spriteY, width, height;
    textureAtlas.GetSpriteData(subTextureName.str(), spriteX, spriteY, width, height);
 
    // Create sprite from atlas data
    SheetSprite* sprite = new SheetSprite(textures[OBJECTS], spriteX / 1024, spriteY / 1024, width / 1024, height / 1024, width / height, 0.3f);
    
    // Calculate entity position in world coordinates based on tile coordinates
    float entityX = x * map->tileSize + map->tileSize / 2;
    float entityY = y * -map->tileSize - map->tileSize / 2;
    
    // Determine the entity's type
    // FIXUP: Find a less janky way of determining the entity type, preferably not by string parsing
    EntityType entityType = strstr(type.data(), "player") != nullptr ? ENTITY_PLAYER : ENTITY_ENEMY;
    // Create entity
    Entity* entity = new Entity(entityX, entityY, sprite, entityType);
    entities.push_back(entity);
    
    if (entityType == ENTITY_PLAYER) {
        player = entity;
    }
     */
    
    EntityType entityType = strstr(type.data(), "player") != nullptr ? ENTITY_PLAYER : ENTITY_ENEMY;
    if (entityType == ENTITY_PLAYER) {
        std::stringstream subTextureName;
        std::vector<float> spriteData;
        for (int i = 0; i < 3; i++) {
            subTextureName.str("");
            subTextureName << type << i + 1 << ".png";
            float spriteX, spriteY, width, height;
            textureAtlas.GetSpriteData(subTextureName.str(), spriteX, spriteY, width, height);
            spriteData.insert(spriteData.end(), {
                spriteX, spriteY, width, height
            });
        }
       
        float entityX = x * map->tileSize + map->tileSize / 2;
        float entityY = y * -map->tileSize - map->tileSize / 2;
        
        entities.push_back(new Entity(entityX, entityY, Rectangle(0.3, 0.3), ENTITY_PLAYER));
        entities.back()->animation = new SpriteAnimation(textures[OBJECTS], spriteData,
                                                         1024, 1024, map->tileSize, LOOP_REVERSE);
        player = entities.back();
        player->animation->SetSpeed(30);
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


