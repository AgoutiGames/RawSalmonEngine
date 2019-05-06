/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
 *
 * This file is part of the RawSalmonEngine.
 *
 * The RawSalmonEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The RawSalmonEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the RawSalmonEngine.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "map/layer.hpp"

#include <list>
#include <string>
#include <sstream>
#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "map/tile.hpp"
#include "util/base64.h"
#include "util/parse.hpp"
#include "util/tinyxml2.h"

Layer::Layer() {}

Layer::~Layer() {}

Layer* Layer::parse(tinyxml2::XMLElement* source, tinyxml2::XMLError& eresult) {
    /// IMPLEMENT THIS
    return nullptr;
}

/**
 * @brief Constructs empty uninitialized Layer
 * @param tile_w, tile_h Dimensions of a standard tile
 *
Layer::Layer(unsigned tile_w, unsigned tile_h) :
m_tile_w{tile_w}, m_tile_h{tile_h}
{

}

/**
 * @brief Parse the layer information from @c XMLElement*
 * @param source The @c XMLElement* which stores the layer info
 * @param base_map To get file path or renderer for img parsing
 * @return an @c XMLError object which indicates success or error type
 *
tinyxml2::XMLError Layer::init(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;

    XMLError eResult;

    // Parse layer name
    const char* p_name = source->Attribute("name");
    if (p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
    m_name = std::string(p_name);

    // Parse map type "map"
    if(std::string("layer") == source->Name()) {
        m_type = map;

        // Parse layer dimensions
        eResult = source->QueryUnsignedAttribute("width", &m_width);
        if(eResult != XML_SUCCESS) return eResult;
        eResult = source->QueryUnsignedAttribute("height", &m_height);
        if(eResult != XML_SUCCESS) return eResult;

        // Parse layer offset
        int offsetx, offsety;
        eResult = source->QueryIntAttribute("offsetx", &offsetx);
        if(eResult == XML_SUCCESS) m_offset_x = offsetx;
        eResult = source->QueryIntAttribute("offsety", &offsety);
        if(eResult == XML_SUCCESS) m_offset_y = offsety;

        // Parse actual map data
        XMLElement* p_data = source->FirstChildElement("data");
        if(p_data == nullptr) return XML_ERROR_PARSING_ELEMENT;

        // Check encoding of data
        // Only base64 encoding is supported right now
        const char* p_encoding = p_data->Attribute("encoding");
        if(p_encoding == nullptr || std::string("base64") != p_encoding) {
            std::cerr << "wrong encoding type: " << p_encoding << " !\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }

        // Store raw map data
        std::string raw_map(p_data->GetText());

        // Decode the raw map data
        std::string data(base64_decode(raw_map));

        // Clear map from old data
        m_map_grid.resize(m_height);

        unsigned counter = 0; ///< This stores the currently read byte

        // Reassemble each tile id from 4 bytes and store in the 2d vector
        for(unsigned i_y = 0; i_y < m_height; i_y++) {
            for(unsigned i_x = 0; i_x < m_width; i_x++) {
                Uint32 tile_id = 0;
                Uint8 byte = data[counter];
                tile_id += byte;
                byte = data[counter + 1];
                tile_id += byte * 256;
                byte = data[counter + 2];
                tile_id += byte * 256 * 256;
                byte = data[counter + 3];
                tile_id += byte * 256 * 256 * 256;
                counter += 4;

                if(tile_id > 65535) {
                    std::cerr << "Tile Id is out of Uint16 Limit!\n";
                    return XML_ERROR_PARSING_TEXT;
                }

                m_map_grid[i_y].push_back(static_cast<Uint16>(tile_id));
            }
        }
    }

    // Parse map type "object"
    else if(std::string("objectgroup") == source->Name()) {
        m_type = object;

        // Parse individual objects/actors
        XMLElement* p_object = source->FirstChildElement("object");
        while(p_object != nullptr) {
            unsigned gid;
            eResult = p_object->QueryUnsignedAttribute("gid", &gid);
            if(eResult != XML_SUCCESS) {
                std::cerr << "Object in layer: " << m_name << " has no tile id!\n";
                std::cerr << "Currently only tile objects are supported/only true actors\n";
                return eResult;
            }

            // Construct actor from template corresponding to gid
            m_obj_grid.push_back(Actor(gid, &base_map));

            // Initialize actor from the XMLElement*
            eResult = m_obj_grid.back().init_actor(p_object);
            if(eResult != XML_SUCCESS) {
                std::cerr << "Failed at loading object in layer: " << m_name << " with gid: " << gid << "\n";
                return eResult;
            }
            // Move to next object/actor
            p_object = p_object->NextSiblingElement();
        }
    }

    // Parse map type "image"
    else if(std::string("imagelayer") == source->Name()) {
        m_type = image;

        // Parse image position
        eResult = source->QueryIntAttribute("offsetx", &m_offset_x);
        if(eResult != XML_SUCCESS) {
            m_offset_x = 0;
        }
        eResult = source->QueryIntAttribute("offsety", &m_offset_y);
        if(eResult != XML_SUCCESS) {
            m_offset_y = 0;
        }
        // Parse opacity (which works with image layer)
        float opacity;
        eResult = source->QueryFloatAttribute("opacity", &opacity);
        if(eResult == XML_SUCCESS) m_opacity = opacity;

        // Parse image file path
        XMLElement* p_image = source->FirstChildElement("image");
        if(p_image == nullptr) return XML_ERROR_PARSING_ELEMENT;
        const char* p_source = p_image->Attribute("source");
        if(p_source == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        m_img_src = base_map.get_file_path() + std::string(p_source);

        // Parse the actual image data
        if(!m_img.loadFromFile(base_map.get_renderer(), m_img_src)) return XML_ERROR_PARSING;
        m_img.setAlpha(static_cast<Uint8>(m_opacity * 255));
        m_width = static_cast<unsigned>(m_img.getWidth());
        m_height = static_cast<unsigned>(m_img.getHeight());

        // Parse image properties (only blend mode right now)
        XMLElement* p_properties = source->FirstChildElement("properties");
        if(p_properties != nullptr) {
            XMLElement* p_property = p_properties->FirstChildElement("property");
            while(p_property != nullptr) {
                const char* p_name;
                p_name = p_property->Attribute("name");
                std::string name(p_name);
                if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
                if(name == "BLEND_MODE") {
                    eResult = parse::blendmode(p_property, m_img);
                    if(eResult != XML_SUCCESS) {
                        std::cerr << "Failed at parsing blend mode for layer: " << m_name << "\n";
                        return eResult;
                    }
                }
                else if(name == "PARALLAX") {
                    eResult = p_property->QueryBoolAttribute("value", &m_parallax);
                    if(eResult != XML_SUCCESS) return eResult;
                }
                else{
                    std::cerr << "Unknown image layer property " << p_name << " occured\n";
                    return XML_ERROR_PARSING;
                }
                // Move to next property
                p_property = p_property->NextSiblingElement("property");
            }
        }
    }

    // Return error for unknown layer types
    else {
        std::cerr << "Unknown layer type: " << source->Name() << " !\n";
        return XML_ERROR_PARSING_ELEMENT;
    }
    return XML_SUCCESS;
}

/**
 * @brief Renders the image layer to screen
 *
 * This function determines and draws the necessary tiles relative to
 * the camera position and size. This includes offscreen tile culling
 * to save processing power. A margin around the screen is also rendered
 * to make up for oversized tiles and tilset offset values.
 *
 * @param camera The rectangular area of the map to be rendered
 * @param base_map A reference to the map for getting tiles, overhang, renderer, etc.
 * @return @c bool which indicates success or failure

 * @warning The layer opacity value is ignored except for @c image type layers
 *

bool Layer::render(const Camera& camera, const MapData& base_map, const TilesetCollection& ts_collection) const {
    bool success = true;
    switch(m_type) {
        // Render map type "map"
        case map:{
            // Cast tile_w and h to int to simplify calculations
            int tile_w = static_cast<int>(m_tile_w);
            int tile_h = static_cast<int>(m_tile_h);

            // Apply the layer offset
            int x_camera = camera.x() - m_offset_x;
            int y_camera = camera.y() - m_offset_y;

            // Horizontal range of tiles to render
            int x_tile_from = x_camera / tile_w;
            int x_tile_to = (x_camera + camera.w()) / tile_w;

            // Horizontal pixel offset to full tile
            int x_tile_offset = x_camera % tile_w;

            // Vertical range of tiles to render
            int y_tile_from = y_camera / tile_h;
            int y_tile_to = (y_camera + camera.h()) / tile_h;

            // Vertical pixel offset to full tile
            int y_tile_offset = y_camera % tile_h;

            // Apply the margin which makes up for oversized tiles and tileset offset
            x_tile_from -= ts_collection.get_overhang(Direction::left);
            x_tile_to += ts_collection.get_overhang(Direction::right);
            y_tile_from -= ts_collection.get_overhang(Direction::up);
            y_tile_to += ts_collection.get_overhang(Direction::down);

            // Pixel perfect position of the first upper left tile
            int x = -x_tile_offset - (ts_collection.get_overhang(Direction::left) * m_tile_w);
            int y = -y_tile_offset - (ts_collection.get_overhang(Direction::up) * m_tile_h);

            // if(m_opacity < 1.0f) Tileset::set_opacity(m_opacity);

            // Iterates through vertical rows tile by tile
            for(int i_y_tile = y_tile_from; i_y_tile <= y_tile_to; i_y_tile++) {

                // Skips vertical rows if position is off map/layer
                if(i_y_tile >= 0 && i_y_tile < static_cast<int>(m_height)) {

                    // Iterates through horizontal rows tile by tile
                    for(int i_x_tile = x_tile_from; i_x_tile <= x_tile_to; i_x_tile++) {

                        // Skips horizontal rows if position is off map/layer
                        if(i_x_tile >= 0 && i_x_tile < static_cast<int>(m_width)) {

                            // Get tile id from map layer data and draw at current position if tile_id is not 0
                            Uint16 tile_id = m_map_grid[i_y_tile][i_x_tile];
                            if(tile_id != 0) {
                                if(!ts_collection.render(tile_id,x,y)) success = false;
                            }
                        }
                        // Move to next horizontal tile position
                        x += tile_w;
                    }
                }
                // Reset horizontal tile position
                x = -x_tile_offset - (ts_collection.get_overhang(Direction::left) * m_tile_w);

                // Move to next vertical tile position
                y += tile_h;
            }
            break;}
        // Render map type "object"
        case object:{
            int x = camera.x() - m_offset_x;
            int y = camera.y() - m_offset_y;
            int from = y - ts_collection.get_overhang(Direction::up) * m_tile_h;
            int to = y + camera.h() + ts_collection.get_overhang(Direction::down) * m_tile_h;
            // Has y-axis offscreen culling
            for(auto it=m_obj_grid.begin(); it != m_obj_grid.end(); ++it) {
                int feet = it->get_y();
                int head = feet - it->get_h();
                if(feet > from && head < to) {it->render(x,y);}
            }
            break;}
        // Render map type "image"
        case image:{
            if(m_parallax) {
                int x_range = base_map.get_w() - camera.w();
                int y_range = base_map.get_h() - camera.h();
                float x_trans_fact = static_cast<float>(camera.x()) / x_range;
                float y_trans_fact = static_cast<float>(camera.y()) / y_range;
                x_trans_fact = x_trans_fact * (m_width - camera.w());
                y_trans_fact = y_trans_fact * (m_height - camera.h());
                m_img.render(-x_trans_fact, -y_trans_fact);

            }
            else{
                int x = m_offset_x - camera.x();
                int y = m_offset_y - camera.y();
                if(y > camera.h() || x > camera.w() || x < (-static_cast<int>(m_width)) || y < (-static_cast<int>(m_height))) {
                    return success;
                }
                else {m_img.render(x, y);}
            }
            break;}
        // Don't render unknown type
        default:{
            std::cerr << "Could not render layer " << m_name << "because of invalid layer type\n";
            success = false;
            break;}
    }
    return success;
}

/**
 * @brief Updates the states of the objects in the layer
 *
 * This function currently triggers the update function of every
 * object of an object type layer. When it returns false, this is
 * a signal to delete the object.
 *
void Layer::update() {
    if(m_type == object) {
        for(auto it=m_obj_grid.begin(); it != m_obj_grid.end(); ++it) {
            if(it->update() == false) {
                it--;
                m_obj_grid.erase(++it);
            }
        }
        // Establish correct rendering order
        m_obj_grid.sort();
    }

    else {

    }
}

/**
 * @brief Fetch all actors which conform the supplied parameters
 * @return Vector of conforming actors
 * @note "invalid" value indicates that a parameter is ignored
 *
std::vector<Actor*> Layer::get_actors(std::string name, Direction direction, AnimationType animation) {
    std::vector<Actor*> actor_list;
    if(m_type == LayerType::object) {
        for(Actor& actor : m_obj_grid) {
            bool match = true;

            if(name != "" && actor.get_name() != name) {match = false;}
            if(direction != Direction::invalid && actor.get_direction() != direction) {match = false;}
            if(animation != AnimationType::invalid && actor.get_animation() != animation) {match = false;}

            if(match) {actor_list.push_back(&actor);}
        }
    }
    return actor_list;
}

/**
 * @brief Checks if given rect collides with any entity present in this layer
 * @param rect The rect to check against
 * @param x_max, y_max The maximum depth of intersection by axis
 * @param base_map Reference on map instance used for looking up tiles by their gid
 * @param collided A container to which colliding actors are added
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 *
bool Layer::collide(const SDL_Rect* rect, int& x_max, int& y_max, const TilesetCollection& ts_collection, std::vector<Actor*>& collided, std::string type){
    bool collide = false;
    int x_depth = 0;
    int y_depth = 0;
    switch (m_type) {
        case map:{
            if(type != "COLLIDE") {break;}
            // Cast tile_w and h to int to simplify calculations
            int tile_w = static_cast<int>(m_tile_w);
            int tile_h = static_cast<int>(m_tile_h);
            // Calculate possible chunk of tiles which could possibly collide with the rect
            int x_from = (rect->x - m_offset_x) / tile_w;
            int y_from = (rect->y - m_offset_y) / tile_h;
            int x_to = (rect->x - m_offset_x + rect->w) / tile_w;
            int y_to = (rect->y - m_offset_y + rect->h) / tile_h;
            // Iterate through all possible tiles
            for(int y = y_from; y <= y_to && y >= 0 && y < static_cast<int>(m_height); y++) {
                for(int x = x_from; x <= x_to && x >= 0 && x < static_cast<int>(m_width); x++) {
                    Uint16 tile_id = m_map_grid[y][x];
                    // Exclude invalid tiles from check
                    if(tile_id != 0) {
                        Tile* tile = ts_collection.get_tile(tile_id);
                        SDL_Rect tile_rect = tile->get_hitbox();
                        // Only check collision for tiles with valid hitbox
                        if(!SDL_RectEmpty(&tile_rect)) {
                            // Move tile hitbox to tile coordinates
                            tile_rect.x += m_offset_x + x * tile_w;
                            tile_rect.y += m_offset_y + y * tile_h;
                            SDL_Rect intersect;
                            // Get intersection from supplied rect and tile rect
                            if(SDL_IntersectRect(rect, &tile_rect, &intersect)) {
                                // Possibly overwrite maximum collision depth value
                                if(intersect.w > x_max) {x_max = intersect.w;}
                                if(intersect.h > y_max) {y_max = intersect.h;}
                                /*
                                std::cerr << "check " << tile_rect.x << " " << tile_rect.y << " " << tile_rect.w << " " << tile_rect.h << "\n";
                                std::cerr << "check " << rect->x << " " << rect->y << " " << rect->w << " " << rect->h << "\n";
                                std::cerr << "x depth: " << intersect.w << "\n";
                                std::cerr << "y_depth: " << intersect.h << "\n";
                                *
                                collide = true;
                            }
                        }
                    }
                }
            }
            break;}
        case object:
            // Iterate through all actors
            for(Actor& actor : m_obj_grid) {
                // Check collision against each
                if(actor.collide(rect, x_depth, y_depth, type)) {
                    // Possibly overwrite maximum collision depth value
                    if(x_depth > x_max) {x_max = x_depth;}
                    if(y_depth > y_max) {y_max = y_depth;}
                    // Add actor as collided
                    collided.push_back(&actor);
                    collide = true;
                }
            }
            break;
        default:

            break;
    }
    return collide;
}

/**
 * @brief Checks if given rect collides with any entity present in this layer
 * @param rect The rect to check against
 * @param base_map Reference on map instance used for looking up tiles by their gid
 * @param collided A container to which colliding actors are added
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 *
bool Layer::collide(const SDL_Rect* rect, const TilesetCollection& ts_collection, std::vector<Actor*>& collided, std::string type){
    bool collide = false;
    switch (m_type) {
        case map:{
            if(type != "COLLIDE") {break;}
            // Cast tile_w and h to int to simplify calculations
            int tile_w = static_cast<int>(m_tile_w);
            int tile_h = static_cast<int>(m_tile_h);
            // Calculate possible chunk of tiles which could possibly collide with the rect
            int x_from = (rect->x - m_offset_x) / tile_w;
            int y_from = (rect->y - m_offset_y) / tile_h;
            int x_to = (rect->x - m_offset_x + rect->w) / tile_w;
            int y_to = (rect->y - m_offset_y + rect->h) / tile_h;
            // Iterate through all possible tiles
            for(int y = y_from; y <= y_to && y >= 0 && y < static_cast<int>(m_height); y++) {
                for(int x = x_from; x <= x_to && x >= 0 && x < static_cast<int>(m_width); x++) {
                    Uint16 tile_id = m_map_grid[y][x];
                    // Exclude invalid tiles from check
                    if(tile_id != 0) {
                        Tile* tile = ts_collection.get_tile(tile_id);
                        SDL_Rect tile_rect = tile->get_hitbox();
                        // Only check collision for tiles with valid hitbox
                        if(!SDL_RectEmpty(&tile_rect)) {
                            // Move tile hitbox to tile coordinates
                            tile_rect.x += m_offset_x + x * tile_w;
                            tile_rect.y += m_offset_y + y * tile_h;
                            // Get intersection from supplied rect and tile rect
                            if(SDL_HasIntersection(rect, &tile_rect)) {
                                collide = true;
                            }
                        }
                    }
                }
            }
            break;}
        case object:
            // Iterate through all actors
            for(Actor& actor : m_obj_grid) {
                // Check collision against each
                if(actor.collide(rect, type)) {
                    // Add actor as collided
                    collided.push_back(&actor);
                    collide = true;
                }
            }
            break;
        default:

            break;
    }
    return collide;
}

/**
 * @brief Checks if given rect collides with any entity present in this layer
 * @param rect The rect to check against
 * @param base_map Reference on map instance used for looking up tiles by their gid
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 *
bool Layer::collide(const SDL_Rect* rect, const TilesetCollection& ts_collection, std::string type){
    bool collide = false;
    switch (m_type) {
        case map:{
            if(type != "COLLIDE") {break;}
            // Cast tile_w and h to int to simplify calculations
            int tile_w = static_cast<int>(m_tile_w);
            int tile_h = static_cast<int>(m_tile_h);
            // Calculate possible chunk of tiles which could possibly collide with the rect
            int x_from = (rect->x - m_offset_x) / tile_w;
            int y_from = (rect->y - m_offset_y) / tile_h;
            int x_to = (rect->x - m_offset_x + rect->w) / tile_w;
            int y_to = (rect->y - m_offset_y + rect->h) / tile_h;
            // Iterate through all possible tiles
            for(int y = y_from; y <= y_to && y >= 0 && y < static_cast<int>(m_height); y++) {
                for(int x = x_from; x <= x_to && x >= 0 && x < static_cast<int>(m_width); x++) {
                    Uint16 tile_id = m_map_grid[y][x];
                    // Exclude invalid tiles from check
                    if(tile_id != 0) {
                        Tile* tile = ts_collection.get_tile(tile_id);
                        SDL_Rect tile_rect = tile->get_hitbox();
                        // Only check collision for tiles with valid hitbox
                        if(!SDL_RectEmpty(&tile_rect)) {
                            // Move tile hitbox to tile coordinates
                            tile_rect.x += m_offset_x + x * tile_w;
                            tile_rect.y += m_offset_y + y * tile_h;
                            // Get intersection from supplied rect and tile rect
                            if(SDL_HasIntersection(rect, &tile_rect)) {
                                collide = true;
                            }
                        }
                    }
                }
            }
            break;}
        case object:
            // Iterate through all actors
            for(Actor& actor : m_obj_grid) {
                // Check collision against each
                if(actor.collide(rect, type)) {
                    collide = true;
                }
            }
            break;
        default:

            break;
    }
    return collide;
}
*/
