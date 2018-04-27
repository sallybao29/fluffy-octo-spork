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

GameMode mode = STATE_GAME_LEVEL;
GameState state(&program);

std::map<std::string, GLuint> textures;

TextureAtlasParser textureAtlas(RESOURCE_FOLDER"Resources/Spritesheets/spritesheet_complete.xml");

/*-------------------------------------------- Functions ---------------------------------------------*/

void ProcessTitleScreenInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                mode = STATE_GAME_LEVEL;
            }
        }
    }
}

void RenderTitleScreen() {
    
}

void ProcessGameOverScreenInput() {
    
}

void RenderGameOver() {
    
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
        default:
            break;
    }
    SDL_GL_SwapWindow(displayWindow);
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
    
    Matrix projectionMatrix, viewMatrix;
    
    program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    projectionMatrix.SetOrthoProjection(-projection.x, projection.x, -projection.y, projection.y, -projection.z, projection.z);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(program.programID);
    
    // Load textures
    GLuint tiles = LoadTexture(RESOURCE_FOLDER"Resources/Spritesheets/tilesheet_complete.png", GL_LINEAR);
    GLuint objects = LoadTexture(RESOURCE_FOLDER"Resources/Spritesheets/spritesheet_complete.png", GL_LINEAR);
    
    // Store texture references in lookup table
    textures[TILES] = tiles;
    textures[OBJECTS] = objects;
    
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
