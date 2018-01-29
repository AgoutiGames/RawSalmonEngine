#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "SDL2/SDL.h"
#include "mapdata.hpp"
#include "tile.hpp"

MapData::MapData() {

}

MapData::~MapData() {

}

/**
 * @brief Parse the supplied .tmx file
 *
 * Map infos are directly parsed, Tileset and Layer parsers of
 * the corresponding classes get called. @p renderer needs to be
 * supplied for the loading of the tileset image files.
 *
 * @param filename Name of the .tmx file
 * @param renderer Pointer to SDL2 renderer for loading tileset image files
 * @return an @c XMLError object which indicates success or error type
 */

tinyxml2::XMLError MapData::init_map(std::string filename, SDL_Renderer** renderer) {

    using namespace tinyxml2;

    // This saves the .tmx file to the member m_mapfile
    std::string full_path = m_base_path + filename;
    XMLError eResult = m_mapfile.LoadFile(full_path.c_str());
    if(eResult != XML_SUCCESS) {
        std::cout << "Can't find map at relative path: " << full_path << "\n";
        return eResult;
    }

    // Map info gets parsed
    XMLElement* pMap = m_mapfile.FirstChildElement("map");
    if (pMap == nullptr) return XML_ERROR_PARSING_ELEMENT;

    eResult = pMap->QueryUnsignedAttribute("width", &m_width);
    if(eResult != XML_SUCCESS) return eResult;
    std::cout << "Map width: " << m_width << "\n";
    eResult = pMap->QueryUnsignedAttribute("height", &m_height);
    if(eResult != XML_SUCCESS) return eResult;
    std::cout << "Map height: " << m_height << "\n";
    eResult = pMap->QueryUnsignedAttribute("tilewidth", &m_tile_w);
    if(eResult != XML_SUCCESS) return eResult;
    std::cout << "Tile width: " << m_tile_w << "\n";
    eResult = pMap->QueryUnsignedAttribute("tileheight", &m_tile_h);
    if(eResult != XML_SUCCESS) return eResult;
    std::cout << "Tile height: " << m_tile_h << "\n";


    // All tilesets get parsed
    std::vector<XMLElement*> p_tilesets;

    XMLElement* pTs = pMap->FirstChildElement("tileset");
    if (pTs == nullptr) {
        std::cout << "Error: Parsing Mapfile without any Tileset!\n";
        return XML_ERROR_PARSING_ELEMENT;
        }

    // Collect all tilesets to a vector of pointers
    do{
        p_tilesets.push_back(pTs);
        pTs = pTs->NextSiblingElement("tileset");
    }while(pTs != nullptr );
    std::cout << "Tileset files: " << p_tilesets.size() << "\n";

    // Clear tileset vector member of possible old data
    m_tilesets.clear();
    m_tilesets.resize(p_tilesets.size());

    // (Re)init the Tile and Tileset classes with current info
    Tile::initialize(renderer, m_tile_w, m_tile_h);
    Tileset::initialize(renderer, m_tile_w, m_tile_h);

    // Actually parse each tileset of the vector of pointers
    for(unsigned i = 0; i < p_tilesets.size(); i++) {
        eResult = m_tilesets[i].init(p_tilesets[i]);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing Tileset: " << i << "\n";
            return eResult;
        }
    }

    // This must be called after the parsing of all tilesets!
    // It calculates the minimum rendering overhang due to big tiles and tileset offsets
    Tileset::write_overhang();

    // Collect all layers to a vector of pointers
    std::vector<XMLElement*> p_layers;
    XMLElement* pLa = pMap->FirstChildElement("layer");
    if (pLa == nullptr) return XML_ERROR_PARSING_ELEMENT;
    do{
        p_layers.push_back(pLa);
        pLa = pLa->NextSiblingElement();
    }while(pLa != nullptr );
    std::cout << "Layer count: " << p_layers.size() << "\n";

    // Clear layer vector member of possible old data
    m_layers.clear();
    m_layers.resize(p_layers.size(), Layer(m_tile_w, m_tile_h));

    // Actually parse each layer of the vector of pointers
    for(unsigned i_layer = 0; i_layer < p_layers.size(); i_layer++) {
        XMLError eResult = m_layers[i_layer].init(p_layers[i_layer]);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing layer: " << i_layer << "\n";
            return eResult;
        }
    }
    // This must be called after the parsing of all tilesets!
    // It sets all animated tiles to their starting positions
    // and passes the current timestamp
    Tile::init_anim_tiles();

    return XML_SUCCESS;
}
/**
 * @brief Renders all map layers
 * @param camera The rectangular area of the map to be rendered
 * @return @c bool which indicates success or failure
 */
bool MapData::render(SDL_Rect* camera){
    bool success = true;

    // Checks and changes animated tiles
    Tile::push_all_anim();

    // Renders all layers
    for(unsigned i_layer = 0; i_layer < m_layers.size(); i_layer++) {
        if(!m_layers[i_layer].render(camera)) {
            std::cerr << "Failed at rendering layer " << i_layer << " !\n";
            success = false;
        }
    }
    return success;
}
/**
 * @brief Calls update function of all map layers
 */
void MapData::update() {
    for(unsigned i_layer = 0; i_layer < m_layers.size(); i_layer++) {
        m_layers[i_layer].update();
    }
}
