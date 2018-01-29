#ifndef LAYER_HPP_INCLUDED
#define LAYER_HPP_INCLUDED
#include <vector>
#include <string>
#include "tileset.hpp"
#include "texture.hpp"
#include "SDL2/SDL.h"
#include "game_types.hpp"
#include "tinyxml2.h"
#include "actor.hpp"

/**
 * @brief Parse, store and draw individual layers of the game map
 *
 * Can store all possible layer types
 */

class Layer{
    private:
        enum LayerType{
            map,
            object,
            image,
            undefinied
        };

    public:
        Layer(unsigned tile_w, unsigned tile_h);

        tinyxml2::XMLError init(tinyxml2::XMLElement* source);

        bool render(SDL_Rect* camera) const;
        void update();

    private:
        LayerType m_type = undefinied;

        // members for m_type map
        std::string m_name;
        unsigned m_width;
        unsigned m_height;
        unsigned m_tile_w;
        unsigned m_tile_h;
        std::vector<std::vector<Uint16> > m_map_grid; ///< The actual map layer information
        float m_opacity = 1.0f; ///< @warning value has no effect yet!
        int m_offset_x = 0;
        int m_offset_y = 0;

        // members for m_type image
        std::string m_img_src;
        Texture m_img;

        // members for m_type object
        std::vector<Actor> m_obj_grid;

        static std::string m_base_path;

};


#endif // LAYER_HPP_INCLUDED
