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
#include "TextureAtlasParser.hpp"
#include <SDL_mixer.h>

#define TILES "tiles"
#define OBJECTS "objecs"

class Matrix;
class Vector3;
class ShaderProgram;
class Shape;

/*!
 * @discussion A list of game modes that determine the current state of the game
 */
enum GameMode {
    STATE_TITLE_SCREEN,
    STATE_GAME_LEVEL,
    STATE_GAME_OVER,
    STATE_GAME_WON
};

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
 * @discussion Renders a string of text
 * @param program The shader program to use for rendering
 * @param fontTexture The id of the font texture
 * @param text The text to render
 * @param size The font size
 * @param spacing The amount of spacing to use between each character
 * @param x The x location of the string
 * @param y The y location of the string
 */
void DrawWords (ShaderProgram& program, int fontTexture, std::string text, float size, float spacing, float x, float y);

/*!
 * @abstract Renders a shape
 * @discussion Renders a shape via triangles
 * @param program The shader program to use for rendering
 * @param shape The shape to render
 */
void DrawShape(ShaderProgram& program, const Shape& shape);

/*!
 * @discussion Draws a texture with the given shader program
 # @param program The shader program to use for rendering
 # @param textureID The id of the texture to render
 */
void DrawTexture(ShaderProgram& program, GLuint textureID);

/*!
 * @abstract Converts object space vertices to world space
 * @discussion Multiplies each Vector3 vertex by the given transformation matrix to
 * produce a vector of world space coordinates
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

float easeIn(float from, float to, float time);

float easeOut(float from, float to, float time);

float easeInOut(float from, float to, float time);

float easeOutElastic(float from, float to, float time);

float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax);

// Globals
extern SDL_Window* displayWindow;
/// The dimensions of the display window
extern Vector3 windowSize;
/// How far you can see in either direction
extern Vector3 projection;
/// To be polled for user triggered events
extern SDL_Event event;
/// Whether the game is done. Setting to true will close the window
extern bool done;
/// An array of key states which are valid for the entire duration of the application
extern const Uint8 *keys;

extern GameMode mode;

/// Dictionary of textures loaded for the game
extern std::map<std::string, GLuint> textures;

extern std::map<std::string, Mix_Chunk*> sounds;

extern Mix_Music* music;

extern TextureAtlasParser textureAtlas;


#endif
