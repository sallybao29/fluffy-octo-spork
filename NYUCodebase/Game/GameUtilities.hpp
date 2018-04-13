#ifndef GameUtilities_hpp
#define GameUtilities_hpp

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <string>
#include <vector>

class Matrix;
class Vector3;
class ShaderProgram;
class Shape;

enum GameMode { STATE_TITLE_SCREEN, STATE_GAME_LEVEL, STATE_GAME_OVER };

void ProcessTitleScreenInput();
void RenderTitleScreen();

void ProcessGameOverScreenInput();
void RenderGameOver();

// Utility functions
GLuint LoadTexture(const char *filePath, GLuint filter);
void DrawText(ShaderProgram& program, int fontTexture, std::string text, float size, float spacing);
void DrawShape(ShaderProgram& program, const Shape& shape);
std::vector<std::pair<float, float>> ToWorldSpace(const Matrix& matrix, const std::vector<Vector3>& points);

float lerp(float v0, float v1, float t);

// Globals
extern SDL_Window* displayWindow;
extern Vector3 windowSize;
extern Vector3 projection;

extern SDL_Event event;
extern bool done;
extern const Uint8 *keys;

extern GameMode mode;


#endif
