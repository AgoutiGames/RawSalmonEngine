#ifndef MAPDATA_HPP_INCLUDED
#define MAPDATA_HPP_INCLUDED
#include <vector>
#include <string>
#include "SDL2/SDL.h"
#include "tileset.hpp"
#include "layer.hpp"
#include "tinyxml2.h"

/**
 * @brief Contain, render and update all game map layers
 *
 * This class is the starting point of parsing the Tiled mapfile.
 * It parses the used tilesets and map layers and stores them.
 * It mostly passes the instructions to the corresponding classes.
 */

class MapData {
    public:
        MapData();
        ~MapData();

        tinyxml2::XMLError init_map(std::string filename, SDL_Renderer** renderer);
        bool render(SDL_Rect* camera);
        void update();

    private:
        tinyxml2::XMLDocument m_mapfile{true, tinyxml2::COLLAPSE_WHITESPACE}; ///< Contains the .tmx map file
        std::string m_base_path = "../data/"; ///< Path to folder where .tmx map files are
        unsigned m_tile_w; // The tile dimensions
        unsigned m_tile_h;
        unsigned m_width;  // The map dimensions
        unsigned m_height;

        std::vector<Tileset> m_tilesets; ///< Contains all used Tilesets
        std::vector<Layer> m_layers; ///< Contains all used layers
};


#endif // MAPDATA_HPP_INCLUDED
