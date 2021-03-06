#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL_image.h>
#include "GameState.hpp"

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;
Vector3 windowSize(1280, 720, 0);
Vector3 projection(2.0f * windowSize.x / windowSize.y, 2.0f, 1.0f);
ShaderProgram program;

SDL_Event event;
bool done = false;
const Uint8 *keys = SDL_GetKeyboardState(nullptr);

Matrix projectionMatrix, viewMatrix, modelMatrix;

GameMode mode = STATE_TITLE_SCREEN;
GameState state(&program);

std::map<std::string, GLuint> textures;

std::map<std::string, Mix_Chunk*> sounds;
Mix_Music* music;

TextureAtlasParser textureAtlas(RESOURCE_FOLDER"Resources/Spritesheets/spritesheet_complete.xml");

/*-------------------------------------------- Functions ---------------------------------------------*/

void ProcessWinScreeInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                state.Reset();
                mode = STATE_TITLE_SCREEN;
                Mix_PauseMusic();
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                done = true;
            }
        }
    }
}

void RenderWinScreen() {
    viewMatrix.Identity();
    program.SetViewMatrix(viewMatrix);
    program.SetAlpha(1.0f);
    DrawWords(program, textures[FONT], "YOU WIN!", 0.4f, 0.0f, 0.0f, 0.5f);
    DrawWords(program, textures[FONT], "Weary from his journey,", 0.2f, 0.0f, 0.0f, -0.5f);
    DrawWords(program, textures[FONT], "Jeff finally returns home...", 0.2f, 0.0f, 0.0f, -0.7f);
}

void ProcessTitleScreenInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                mode = STATE_GAME_LEVEL;
                Mix_PlayMusic(music, -1);
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                done = true;
            }
        }
    }
}

void RenderTitleScreen() {
    program.SetAlpha(1.0f);
    modelMatrix.Identity();
    modelMatrix.Scale(7.5, 4, 1.0f);
    program.SetModelMatrix(modelMatrix);
    
    DrawTexture(program, textures["sample"], 0.0f, 0.0f, 0.5f, 0.5f);
    DrawWords(program, textures[FONT], "JEFF WAS LOST", 0.4f, 0.0f, 0.0f, 0.5f);
    DrawWords(program, textures[FONT], "Press Space to Play", 0.2f, 0.0f, 0.0f, 0.0f);
    
    modelMatrix.Identity();
    modelMatrix.Translate(-projection.x * 0.25, -0.5f, 0.0f);
    program.SetModelMatrix(modelMatrix);
    DrawText(program, textures[FONT], "ARROWS   MOVE", 0.15f, 0.0f);
    
    modelMatrix.Translate(0.0f, -0.25f, 0.0f);
    program.SetModelMatrix(modelMatrix);
    DrawText(program, textures[FONT], "D        DEFEND", 0.15f, 0.0f);
    
    modelMatrix.Translate(0.0f, -0.25f, 0.0f);
    program.SetModelMatrix(modelMatrix);
    DrawText(program, textures[FONT], "ESC      EXIT", 0.15f, 0.0f);
}

void ProcessGameOverScreenInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                mode = STATE_GAME_LEVEL;
                state.Reset();
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                done = true;
            }
        }
    }
}

void RenderGameOver() {
    viewMatrix.Identity();
    program.SetViewMatrix(viewMatrix);
    program.SetAlpha(1.0f);
    DrawWords(program, textures[FONT], "GAME OVER", 0.4f, 0.0f, 0.0f, 0.5f);
    DrawWords(program, textures[FONT], "Press Start to Play Again", 0.2f, 0.0f, 0.0f, -0.5f);
}

void ProcessEvents() {
    switch (mode) {
        case STATE_TITLE_SCREEN:
            ProcessTitleScreenInput();
            break;
        case STATE_GAME_OVER:
            ProcessGameOverScreenInput();
            break;
        case STATE_GAME_LEVEL:
            state.ProcessInput();
            break;
        case STATE_GAME_WON:
            ProcessWinScreeInput();
            break;
        default:
            break;
    }
}

void Update(float elapsed) {
    switch (mode) {
        case STATE_TITLE_SCREEN:
            break;
        case STATE_GAME_LEVEL:
            state.Update(elapsed);
            break;
        case STATE_GAME_OVER:
            break;
        default:
            break;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    switch (mode) {
        case STATE_TITLE_SCREEN:
            RenderTitleScreen();
            break;
        case STATE_GAME_LEVEL:
            state.Render();
            break;
        case STATE_GAME_OVER:
            RenderGameOver();
            break;
        case STATE_GAME_WON:
            RenderWinScreen();
            break;
        default:
            break;
    }
    SDL_GL_SwapWindow(displayWindow);
}

void LoadSounds() {
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    sounds["jump"] = Mix_LoadWAV(RESOURCE_FOLDER"Resources/Sounds/jump.wav");
    sounds["land"] = Mix_LoadWAV(RESOURCE_FOLDER"Resources/Sounds/land.wav");
    sounds["hurt"] = Mix_LoadWAV(RESOURCE_FOLDER"Resources/Sounds/hurt.wav");
    sounds["defense"] = Mix_LoadWAV(RESOURCE_FOLDER"Resources/Sounds/defense_hit.wav");
    sounds["bounce"] = Mix_LoadWAV(RESOURCE_FOLDER"Resources/Sounds/bounce.wav");
    sounds["fanfare"] = Mix_LoadWAV(RESOURCE_FOLDER"Resources/Sounds/fanfare.wav");
    sounds["door_open"] = Mix_LoadWAV(RESOURCE_FOLDER"Resources/Sounds/door_open.wav");
    sounds["flyer"] = Mix_LoadWAV(RESOURCE_FOLDER"Resources/Sounds/flyer_cry.wav");
    
    Mix_VolumeChunk(sounds["jump"], 25);
    Mix_VolumeChunk(sounds["hurt"], 25);
    
    Mix_VolumeMusic(35);
    
    music = Mix_LoadMUS(RESOURCE_FOLDER"Resources/Sounds/bgm.mp3");
}

void Setup() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowSize.x, windowSize.y, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, windowSize.x, windowSize.y);
    
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    projectionMatrix.SetOrthoProjection(-projection.x, projection.x, -projection.y, projection.y, -projection.z, projection.z);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(program.programID);
    
    // Load textures
    GLuint tiles = LoadTexture(RESOURCE_FOLDER"Resources/Spritesheets/tilesheet_complete.png", GL_NEAREST);
    GLuint objects = LoadTexture(RESOURCE_FOLDER"Resources/Spritesheets/spritesheet_complete.png", GL_NEAREST);
    GLuint fonts = LoadTexture(RESOURCE_FOLDER"Resources/Spritesheets/font1.png", GL_LINEAR);
    GLuint sample = LoadTexture(RESOURCE_FOLDER"Resources/Sample.png", GL_LINEAR);
    
    // Store texture references in lookup table
    textures[TILES] = tiles;
    textures[OBJECTS] = objects;
    textures[FONT] = fonts;
    textures["sample"] = sample;
    
    std::stringstream stream;
    std::stringstream streamLoad;
    for (int i = 1; i < 4; i++ ) {
        
        stream.str ("");
        streamLoad.str ("");
        
        stream << "background_" << i ;
        streamLoad << RESOURCE_FOLDER << "Resources/Backgrounds/set" << i << "_background.png";
        GLuint background = LoadTexture(streamLoad.str().data(), GL_NEAREST);
        textures [stream.str ()] = background;
        
        stream.str ("");
        streamLoad.str ("");
        
        stream << "hills_" << i ;
        streamLoad << RESOURCE_FOLDER << "Resources/Backgrounds/set" << i << "_hills.png";
        GLuint hills = LoadTexture(streamLoad.str().data(), GL_NEAREST);
        textures [stream.str ()] = hills;
        
        stream.str ("");
        streamLoad.str ("");
        
        stream << "tiles_" << i ;
        streamLoad << RESOURCE_FOLDER << "Resources/Backgrounds/set" << i << "_tiles.png";
        GLuint tiles = LoadTexture(streamLoad.str().data(), GL_NEAREST);
        textures [stream.str ()] = tiles;
    }
    
    LoadSounds();
    
    // Initialize the game state
    state.Initialize();
}

int main(int argc, char *argv[])
{
    float elapsed = 0.0f;
    float accumulator = 0.0f;
    float lastFrameTicks = 0.0f;
    
    Setup();
    while (!done) {
        ProcessEvents();
        
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        elapsed += accumulator;
        if (elapsed < FIXED_TIMESTEP) {
            accumulator = elapsed;
            continue;
        }
        
        while (elapsed >= FIXED_TIMESTEP) {
            Update(FIXED_TIMESTEP);
            elapsed -= FIXED_TIMESTEP;
        }
        accumulator = elapsed;
        
        Render();
    }
    
    SDL_Quit();
    return 0;
}
