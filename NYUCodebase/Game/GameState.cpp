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
}

void GameState::Update(float elapsed) {
}

void GameState::RenderTextures(const std::string& stream) {
    glBindTexture(GL_TEXTURE_2D, textures [stream]);
    float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    glVertexAttribPointer(shader -> positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(shader -> positionAttribute);
    float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    glVertexAttribPointer(shader -> texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(shader -> texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
    modelMatrix.Scale (map -> mapWidth, map ->mapHeight, 1.0f);
    shader -> SetModelMatrix(modelMatrix);
    std::stringstream stream;
    stream << "background_" << level ;
    RenderTextures(stream.str ());
    
    // Draw background tiles
    modelMatrix.Identity();
    modelMatrix.SetPosition (viewX, viewY, 0.0f);
    modelMatrix.Scale (projection.x * 2, projection.y *2  , 1.0f);
    shader -> SetModelMatrix( modelMatrix);
    stream.str ("");
    stream << "tiles_" << level ;
    RenderTextures (stream.str());
    
    // Draw background hills
    modelMatrix.Identity ();
    modelMatrix.SetPosition (viewX + projection.x * 0.25, viewY + projection.y * 0.375 , 0.0f);
    modelMatrix.Scale (projection.x * 2.5, projection.y * 2.5, 1.0f);
    shader -> SetModelMatrix(modelMatrix);
    stream.str ("");
    stream << "hills_" << level ;
    RenderTextures(stream.str());
    
    // Draw map
    modelMatrix.Identity ();
    shader -> SetModelMatrix(modelMatrix);
    map->Render(*shader);
    
    // Draw entities
   
    for (size_t i = 0; i < entities.size(); i++) {
        entities[i]->Render(*shader);
    }
}



