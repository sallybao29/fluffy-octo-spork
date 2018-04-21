#include "TextureAtlasParser.hpp"

TextureAtlasParser::TextureAtlasParser(const std::string& filename) {
    // Read in the xml file
    file = new rapidxml::file<>(filename.data());
    // Parse the file into an xml doc
    doc.parse<0>(file->data());
    // Find the root node of the doc
    root = doc.first_node("TextureAtlas");
    if (root == nullptr) {
        assert(false);
    }
}

TextureAtlasParser::~TextureAtlasParser() {
    delete file;
}

const rapidxml::xml_node<>* TextureAtlasParser::GetSubTexture(const std::string& attribute, const std::string& value) const {
    for (const rapidxml::xml_node<>* texNode = root->first_node(); texNode != nullptr; texNode = texNode->next_sibling()) {
        const rapidxml::xml_attribute<>* texAttrib = texNode->first_attribute(attribute.data());
        if (texAttrib != nullptr && strcmp(texAttrib->value(), value.data()) == 0) {
            return texNode;
        }
    }
    return nullptr;
}

bool TextureAtlasParser::GetSpriteData(const std::string& name, float& x, float& y, float& width, float& height) const {
    const rapidxml::xml_node<>* subTexture = GetSubTexture("name", name);
    if (subTexture == nullptr) {
        return false;
    }
    x = strtof(subTexture->first_attribute("x")->value(), nullptr);
    y = strtof(subTexture->first_attribute("y")->value(), nullptr);
    width = strtof(subTexture->first_attribute("width")->value(), nullptr);
    height = strtof(subTexture->first_attribute("height")->value(), nullptr);
    
    return true;
}
