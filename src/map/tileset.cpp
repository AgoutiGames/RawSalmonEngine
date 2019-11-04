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
#include "map/tileset.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "map/mapdata.hpp"
#include "map/tile.hpp"
#include "util/logger.hpp"
#include "util/parse.hpp"

/**
 * @brief Initialize a tileset from XML info
 * @param ts_file The @c XMLElement which stores the tileset information
 * @param ts_collection Reference to tileset collection to register tiles, etc.
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError Tileset::init(tinyxml2::XMLElement* ts_file, TilesetCollection& ts_collection) {

    using namespace tinyxml2;

    mp_ts_collection = &ts_collection;

    XMLError eResult;
    eResult = ts_file->QueryUnsignedAttribute("firstgid", &m_first_gid);
    if(eResult != XML_SUCCESS) return eResult;

    // If attribute "source" is set, load external .tsx tileset file
    std::string full_path = ts_collection.get_mapdata().get_file_path();

    const char* p_source;
    p_source = ts_file->Attribute("source");
    XMLDocument tsx_tileset{true, tinyxml2::COLLAPSE_WHITESPACE};
    if(p_source != nullptr) {
        full_path += std::string(p_source);

        XMLError eResult = tsx_tileset.LoadFile(full_path.c_str());
        if(eResult != XML_SUCCESS) return eResult;

        // Trim string
        full_path.erase(full_path.find_last_of('/') + 1);

        XMLElement* pTileset = tsx_tileset.FirstChildElement("tileset");
        if (pTileset == nullptr) return XML_ERROR_PARSING_ELEMENT;

        ts_file = pTileset;
    }

    // Parse tileset name, tile dimensions and tile count
    const char* p_ts_name;
    p_ts_name = ts_file->Attribute("name");
    if(p_ts_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
    m_name = std::string(p_ts_name);
    eResult = ts_file->QueryUnsignedAttribute("tilewidth", &m_tile_width);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = ts_file->QueryUnsignedAttribute("tileheight", &m_tile_height);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = ts_file->QueryUnsignedAttribute("tilecount", &m_tile_count);
    if(eResult != XML_SUCCESS) return eResult;

    // Parse tile offset values
    XMLElement* p_offset = ts_file->FirstChildElement("tileoffset");
    if(p_offset != nullptr) {
        eResult = p_offset->QueryIntAttribute("x", &m_x_offset);
        if(eResult != XML_SUCCESS) return eResult;
        eResult = p_offset->QueryIntAttribute("y", &m_y_offset);
        if(eResult != XML_SUCCESS) return eResult;
    }

    // Parse image path and dimensions, and load the image file
    XMLElement* p_image = ts_file->FirstChildElement("image");
    if(p_image == nullptr) return XML_ERROR_PARSING_ELEMENT;
    const char* p_ts_source;
    p_ts_source = p_image->Attribute("source");
    if (p_ts_source == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
    m_image.loadFromFile(mp_ts_collection->get_mapdata().get_renderer(), full_path + std::string(p_ts_source));
    eResult = p_image->QueryUnsignedAttribute("width", &m_width);
    if(eResult != XML_SUCCESS) return eResult;
    eResult = p_image->QueryUnsignedAttribute("height", &m_height);
    if(eResult != XML_SUCCESS) return eResult;

    // Check for matching image dimensions
    // If tiles don't perfectly fit, the tileset gets rejected
    if(m_width % m_tile_width != 0) {
        Logger(Logger::error) << "Image width isn't divisible by its tile width in tileset: " << m_name;
        return XML_ERROR_PARSING;
    }
    if(m_height % m_tile_height != 0) {
        Logger(Logger::error) << "Image height isn't divisible by its tile height in tileset: " << m_name;
        return XML_ERROR_PARSING;
    }
    if(m_tile_count != (m_width / m_tile_width) * (m_height / m_tile_height)) {
        Logger(Logger::error) << "Wrong tile count given in .tmx/.tsx file in tileset: " << m_name;
        return XML_ERROR_PARSING;
    }

    // Set reserve to keep pointers to tile stable!
    m_tiles.reserve(m_tile_count);

    for(unsigned i_tile = 0; i_tile < m_tile_count; i_tile++) {
        // Set the clip rect of each tile in the tileset
        SDL_Rect temp;
        temp.x = i_tile % (m_width / m_tile_width) * m_tile_width;
        temp.y = i_tile / (m_width / m_tile_width) * m_tile_height;
        temp.w = m_tile_width;
        temp.h = m_tile_height;

        // Construct each tile of the tilset and store in m_tiles
        m_tiles.push_back(Tile(this, temp));
        if(!ts_collection.register_tile(&m_tiles.back(), i_tile + m_first_gid)) {
            Logger(Logger::error) << "Failed to register Tile, abort parsing process!";
            return XML_ERROR_PARSING;
        }
    }

    // Parse user specified properties of the tileset (only blend mode right now)
    XMLElement* p_properties = ts_file->FirstChildElement("properties");
    if(p_properties != nullptr) {
        XMLElement* p_property = p_properties->FirstChildElement("property");
        while(p_property != nullptr) {
            const char* p_name;
            p_name = p_property->Attribute("name");
            std::string name(p_name);
            if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
            if(name == "BLEND_MODE") {
                eResult = parse::blendmode(p_property, m_image);
                if(eResult != XML_SUCCESS) {
                    Logger(Logger::error) << "Failed at parsing blend mode for tileset: " << m_name;
                    return eResult;
                }
            }
            else{
                Logger(Logger::error) << "Unknown tileset property " << p_name << " occured in tileset: " << m_name;
                return XML_ERROR_PARSING;
            }
            // Move to next property
            p_property = p_property->NextSiblingElement("property");
        }
    }

    // Check if there is specific tile info
    XMLElement* p_tile = ts_file->FirstChildElement("tile");

    if(p_tile != nullptr) {

        // Method of MapData object deals with the parsing of all tiles
        eResult = parse_tile_info(p_tile);
        if(eResult != XML_SUCCESS) {
            Logger(Logger::error) << "Failed at loading tile info of tileset: " << m_name;
            return eResult;
        }
    }
    return XML_SUCCESS;
}

/**
 * @brief Renders a tile of the tileset at a coordinate
 * @param x, y The specified coordinate
 * @param local_tile_id The ID of the tile corresponding to it's tileset
 * @return @c bool which indicates success or failure
 */
bool Tileset::render(Uint32 local_tile_id, int x, int y) const {
    bool success = true;
    if(local_tile_id >= m_tiles.size()) {
        Logger(Logger::error) << "Local tileset tile id " << local_tile_id << " is out of bounds";
        success = false;
    }
    else {
        m_tiles[local_tile_id].render(x, y);
    }
    return success;
}

/**
 * @brief Return the required render margin for this tileset
 *
 * Since we support oversized tiles and tileset offsets, we have to render an additional
 * margin around the actual screen to display an extra tree for example, which protrudes
 * from the lower screen boundary into the screen. This is easier than checking each tile
 * seperately while having a better performance than without offcreen tile culling.
 *
 * @todo Use pixel margin instead of tile margin for possibly slightly better performance
 */
std::map<Direction, unsigned> Tileset::determine_overhang(unsigned tile_w, unsigned tile_h) const{
    int pix_up = 0;
    int pix_down = 0;
    int pix_left = 0;
    int pix_right = 0;

    // Translate offset into required margin sizes
    if (m_x_offset > 0) {
        pix_left += m_x_offset;
    }
    else if (m_x_offset < 0) {
        pix_right -= m_x_offset;
    }
    if (m_y_offset > 0) {
        pix_up += m_y_offset;
    }
    else if(m_y_offset < 0) {
        pix_down -= m_y_offset;
    }

    // Take oversized tiles into account
    pix_left += m_tile_width - tile_w;
    pix_down += m_tile_height - tile_h;

    if (pix_up < 0) {pix_up = 0;}
    if (pix_down < 0) {pix_down = 0;}
    if (pix_left < 0) {pix_left = 0;}
    if (pix_right < 0) {pix_right = 0;}

    std::map<Direction, unsigned> oh_map;
    oh_map[Direction::up] = pix_up;
    oh_map[Direction::down] = pix_down;
    oh_map[Direction::left] = pix_left;
    oh_map[Direction::right] = pix_right;

    return oh_map;
}

/**
 * @brief Parse symbolic tiles to register them as events or keys
 * @param source The XMLElement pointing to the tileset
 * @param base_map Reference to map object to register each event or key
 * @return @c XMLElement indication sucess of parsing
 *
 * @todo Somehow refactor this ugly ugly key mapping parsing
 */
tinyxml2::XMLError Tileset::parse_symbolic(tinyxml2::XMLElement* source, MapData& base_map) {
    using namespace tinyxml2;
    XMLError eResult;
    XMLElement* p_tile = source->FirstChildElement("tile");
    while(p_tile != nullptr) {
        // Safely parse type
        const char* p_type = p_tile->Attribute("type");
        std::string type;
        if(p_type != nullptr) {type = p_type;}
        // Parse key mapping
        if(type == "KEY_MAPPING"){
            bool up = false;
            bool down = false;
            bool sustained = false;
            std::string event = "";
            SDL_Keycode key = SDLK_UNKNOWN;

            XMLElement* p_property = nullptr;
            XMLElement* p_properties = p_tile->FirstChildElement("properties");
            if(p_properties != nullptr) {
                p_property = p_properties->FirstChildElement("property");
                if(p_property == nullptr) {
                    Logger(Logger::error) << "Error: Missing first property in key mapping: " << p_tile->Attribute("id");
                    return XML_ERROR_PARSING_ELEMENT;
                }
            }
            else {
                Logger(Logger::error) << "Error: Missing properties in key mapping: " << p_tile->Attribute("id");
                return XML_ERROR_PARSING_ELEMENT;
            }
            while(p_property != nullptr) {
                const char* p_name;
                p_name = p_property->Attribute("name");
                std::string name(p_name);
                if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
                if(name == "UP") {
                    eResult = p_property->QueryBoolAttribute("value", &up);
                    if(eResult != XML_SUCCESS) return eResult;
                }
                else if(name == "DOWN") {
                    eResult = p_property->QueryBoolAttribute("value", &down);
                    if(eResult != XML_SUCCESS) return eResult;
                }
                else if(name == "SUSTAINED") {
                    eResult = p_property->QueryBoolAttribute("value", &sustained);
                    if(eResult != XML_SUCCESS) return eResult;
                }
                else if(name == "KEYPRESS") {
                    const char* p_key_name = p_property->Attribute("value");
                    if(p_key_name == nullptr) {
                        Logger(Logger::error) << "Missing keypress value! Tile ID: " << p_tile->Attribute("id");
                        return XML_ERROR_PARSING_ATTRIBUTE;
                    }
                    key = SDL_GetKeyFromName(p_key_name);
                    if(key == SDLK_UNKNOWN) {
                        Logger(Logger::error) << "Unknown key value " << p_key_name << " Tile ID: " << p_tile->Attribute("id");
                        return XML_ERROR_PARSING_ATTRIBUTE;
                    }

                }
                // If the event value is missing, instead of throwing an error the key is skipped
                else if(name == "EVENT") {
                    const char* p_event_name = p_property->Attribute("value");
                    if(p_event_name != nullptr) {
                        event.assign(p_event_name);
                    }
                }

                else {
                    Logger(Logger::error) << "Unknown property " << name << " for keypress, Tile ID: " << p_tile->Attribute("id");
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }

                p_property = p_property->NextSiblingElement("property");
            }

            if(event != "") {
                if(key == SDLK_UNKNOWN) {
                    Logger(Logger::error) << "Missing keypress value after parsing properties! Tile ID: " << p_tile->Attribute("id");
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
                if(!base_map.get_input_handler().register_key(key, event, sustained, up, down)) {
                    Logger(Logger::error) << "Failed registering key " << SDL_GetKeyName(key) << " with event " << event << " Tile ID: " << p_tile->Attribute("id");
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
            }
        }

        // Parse events
        else if (type != "") {
            const char identifier = type.front();
            switch (identifier) {
                case 'A': {
                    std::pair<std::string, SmartEvent<Actor>> event;
                    event.second = SmartEvent<Actor>(p_tile, base_map);
                    if(!event.second) {
                        Logger(Logger::error) << "Failed at parsing symbolic tile yielding an event Tile ID: " << p_tile->Attribute("id");
                        return XML_ERROR_PARSING;
                    }
                    else {
                        event.first = event.second->get_name();
                        base_map.register_event<Actor>(event);
                    }
                    // std::cerr << "Just Parsed " << event.second->get_name() << "\n";
                    break;
                }

                case 'M': {
                    std::pair<std::string, SmartEvent<MapData>> event;
                    event.second = SmartEvent<MapData>(p_tile, base_map);
                    if(!event.second) {
                        Logger(Logger::error) << "Failed at parsing symbolic tile yielding an event Tile ID: " << p_tile->Attribute("id");
                        return XML_ERROR_PARSING;
                    }
                    else {
                        event.first = event.second->get_name();
                        base_map.register_event<MapData>(event);
                    }
                    // std::cerr << "Just Parsed " << event.second->get_name() << "\n";
                    break;
                }

                case 'G': {
                    std::pair<std::string, SmartEvent<GameInfo>> event;
                    event.second = SmartEvent<GameInfo>(p_tile, base_map);
                    if(!event.second) {
                        Logger(Logger::error) << "Failed at parsing symbolic tile yielding an event Tile ID: " << p_tile->Attribute("id");
                        return XML_ERROR_PARSING;
                    }
                    else {
                        event.first = event.second->get_name();
                        base_map.register_event<GameInfo>(event);
                    }
                    // std::cerr << "Just Parsed " << event.second->get_name() << "\n";
                    break;
                }

                default: {
                    Logger(Logger::error) << "Unknown event type: " << type << " at Tile ID: " << p_tile->Attribute("id");
                    return XML_ERROR_PARSING;
                }
            }
        }

        else {
            Logger(Logger::warning) << "Missing event type at Tile ID: " << p_tile->Attribute("id") << ", skipping";
            //return XML_ERROR_PARSING;
        }

        p_tile = p_tile->NextSiblingElement("tile");
    }
    return XML_SUCCESS;
}


/**
 * @brief Parse tile information from tileset
 * @param source The @c XMLElement from the tileset
 * @param first_gid The first global tile id of the tileset
 * @return an @c XMLError object which indicates success or error type
 *
 * Determines the tile type and calls the corresponding tile parsers
 */
tinyxml2::XMLError Tileset::parse_tile_info(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    XMLError eResult;

    while(source != nullptr) {
        unsigned tile_id;
        eResult = source->QueryUnsignedAttribute("id", &tile_id);
        if(eResult != XML_SUCCESS) return eResult;

        Tile& tile = m_tiles[tile_id];
        const char* p_type;
        p_type = source->Attribute("type");

        std::string tile_type;
        if(p_type != nullptr) tile_type = p_type;

        // Parse normal map tile
        if(p_type == nullptr) {
            eResult = tile.parse_tile(source);
        }
        // Parse Actor_Animation tile
        else if(tile_type == "ACTOR_ANIMATION") {
            eResult = tile.parse_actor_anim(source);
        }
        // Parse Actor_Template
        else if(tile_type == "ACTOR_TEMPLATE") {
            eResult = tile.parse_actor_templ(source);
        }
        else {
            Logger(Logger::error) << "Unknown tile type: " << tile_type;
            return XML_WRONG_ATTRIBUTE_TYPE;
        }

        if(eResult != XML_SUCCESS) {
            Logger(Logger::error) << "Failed at loading tile gid: " << tile_id + m_first_gid << " local id: " << tile_id;
            return eResult;
        }

        source = source->NextSiblingElement();
    }
    return XML_SUCCESS;
}
