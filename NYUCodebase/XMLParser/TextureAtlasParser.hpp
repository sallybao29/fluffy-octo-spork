#ifndef TextureAtlasParser_hpp
#define TextureAtlasParser_hpp

#include <stdio.h>
#include "rapidxml_utils.hpp"

/*!
 * @discussion Class for parsing an xml texture atlas
 */
class TextureAtlasParser {
public:
    /*!
     * @discussion Initializes the parser with the xml doc to parse
     * @param filename The path to the xml file
     */
    TextureAtlasParser(const std::string& filename);
    ~TextureAtlasParser();
    
    /*!
     * @discussion Finds the subtexture node, matching on the given attribute and value
     * @param attribute The xml attribute to search by
     * @param value The value of the xml attribute to match
     * @return A pointer to the subtexture node, or nullptr if not found
     */
    const rapidxml::xml_node<>* GetSubTexture(const std::string& attribute, const std::string& value) const;
    
    /*!
     * @discussion Finds the subtexture with the given name attribute and populates x, y,
     * width and height with the data from the texture
     * @param x The x-coordinate of the sprite on the texture
     * @param y The y-coordinate of the sprite on the texture
     * @param width The width of the sprite on the texture
     * @param height The height of the sprite on the texture
     * @return Whether the data was sucessfully found and parsed
     */
    bool GetSpriteData(const std::string& name, float& x, float& y, float& width, float& height) const;
    
private:
    /// The file containing the xml texture atlas data. Must persist for as long as doc is being used
    rapidxml::file<>* file;
    /// The xml document based on the xml file
    rapidxml::xml_document<> doc;
    /// The root node of the xml document
    rapidxml::xml_node<>* root;
};

#endif
