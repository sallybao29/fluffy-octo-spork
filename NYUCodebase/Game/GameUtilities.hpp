#ifndef GameUtilities_hpp
#define GameUtilities_hpp

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <string>
#include <vector>
#include <map>

#define TILES 0
#define OBJECTS 1

class Matrix;
class Vector3;
class ShaderProgram;
class Shape;

/*!
 * @discussion A list of game modes
 */
enum GameMode {
    STATE_TITLE_SCREEN,
    STATE_GAME_LEVEL,
    STATE_GAME_OVER
};

void ProcessTitleScreenInput();
void RenderTitleScreen();

void ProcessGameOverScreenInput();
void RenderGameOver();

// Utility functions
/*!
 * @discussion Loads a texture from a file
 * @param filePath The path to the file
 * @param filter The filter to use when rendering the texture: GL_LINEAR or GL_NEAREST
 * @return The id of the generated texture
 */
GLuint LoadTexture(const char *filePath, GLuint filter);

/*!
 * @discussion Renders a string of text
 * @param program The shader program to use for rendering
 * @param fontTexture The id of the font texture
 * @param text The text to render
 * @param size The font size
 * @param spacing The amount of spacing to use between each character
 */
void DrawText(ShaderProgram& program, int fontTexture, std::string text, float size, float spacing);

/*!
 * @abstract Renders a shape
 * @discussion Renders a shape via triangles
 * @param program The shader program to use for rendering
 * @param shape The shape to render
 */
void DrawShape(ShaderProgram& program, const Shape& shape);

/*!
 * @abstract Converts object space vertices to world space
 * @discussion Multiplies each Vector3 vertex by the given transformation matrix to
 *   produce a vector of world space coordinates
 * @param matrix The transformation matrix
 * @param points The vertices to convert
 * @return A vector of world space coordinates as (x, y) pairs
 */
std::vector<std::pair<float, float>> ToWorldSpace(const Matrix& matrix, const std::vector<Vector3>& points);

/*!
 * @discussion Linearly interpolates between two values by a given amount
 * @param v0 The starting value
 * @param v1 The ending value
 * @param t The amount to interpolate by
 * @return The interpolated value
 */
float lerp(float v0, float v1, float t);

// Globals
extern SDL_Window* displayWindow;
extern Vector3 windowSize;
extern Vector3 projection;

extern SDL_Event event;
extern bool done;
extern const Uint8 *keys;

extern GameMode mode;

extern std::map<int, GLuint> textures;


#endif
