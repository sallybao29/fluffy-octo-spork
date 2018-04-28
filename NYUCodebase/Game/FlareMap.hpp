#ifndef FlareMap_hpp
#define FlareMap_hpp

#include <stdio.h>
#include <string>
#include <vector>

class ShaderProgram;

/*!
 * @brief A simple representation of an entity extracted from a flare map
 */
struct FlareMapEntity {
    /// The type of the entity. Should correspond to a valid EntityType
    std::string type;
    /// The x position of the entity on the map (in tiles)
    float x;
    /// The y position of the entity on the map (in tiles)
    float y;
};

/*!
 * @abstract A tilemap based on a Flare file
 * @discussion A representation of a tilemap based on a Flare file derived from a map in Tiled.
 * Contains tile and entity data
 */
class FlareMap {
public:
    FlareMap(float tileSize);
    ~FlareMap();
    
    /*!
     * @discussion Initializes the flare map with data read from a Flare file
     * @param filename The path to the Flare file
     */
    void Load(std::string filename);
    
    /*!
     * @discussion Assigns a spritesheet to the flare map
     * @param sheet The id of the spritesheet texture
     * @param spriteCountX The number of columns the spritesheet contains
     * @param spriteCountY The number of rows the spritesheet contains
     */
    void SetSpriteSheet(int sheet, size_t spriteCountX, size_t spriteCountY);
    
    /*!
     * @discussion Renders the map
     * @param shader The shader program to use for rendering
     * @warning The map will render in the fourth quadrant of the screen. Make sure to set a
     * small initial tile size so you can see if the map is actually there.
     */
    void Render(ShaderProgram& shader);
    
    /*!
     * @abstract Converts world coordinates to tile coordinates
     * @discussion Converts world coordinates to grid coordinates corresponding to the tilemap and
     * populates two floats that are passed in with the new coordinate values
     * @param worldX The x coordinate in world space
     * @param worldY The y coordinate in world space
     * @param gridX The place to store the converted x coordinate
     * @param gridY The place to store the converted y coordinate
     */
    void worldToTileCoordinates(float worldX, float worldY, int& gridX, int& gridY) const;
    int worldToTileCoordX(float worldX) const;
    int worldToTileCoordY(float worldY) const;
    
    /*!
     * @discussion Tile data is stored as integers starting from 1, which correspond to sprite indices
     * on the spritesheet. Since spritesheet indices start at 1, there is an offset of one when indexing
     * sprites. Blank tiles are represented by 0.
     */
    unsigned int** mapData;
    /// The position and types of the entities in the map
    std::vector<FlareMapEntity> entities;
    
    /// The size of each tile with respect to the orthographic projection
    float tileSize;
    /// The width of the map in number of tiles
    size_t mapWidth;
    /// The height of the map in number of tiles
    size_t mapHeight;
    
    /// The texture id of the map's spritesheet
    int spriteSheetTexture;
    /// The number of columns the spritesheet contains
    size_t spriteCountX;
    /// The number of rows the spritesheet contains
    size_t spriteCountY;
    
private:
    /*!
     * @discussion Reads in the header data and initializes mapWidth and mapHeight
     * @param stream The file stream being read
     * @returns Whether the read operation was successful
     */
    bool ReadHeader(std::ifstream& stream);
    
    /*!
     * @discussion Reads in the tile layer data and populates mapData
     * @param stream The file stream being read
     * @return Whether the read operation was successful
     */
    bool ReadLayerData(std::ifstream& stream);
    
    /*!
     * @discussion Reads in the entity data
     * @param stream The file stream being read
     * @return Whether the read operation was successful
     */
    bool ReadEntityData(std::ifstream& stream);
};

#endif

