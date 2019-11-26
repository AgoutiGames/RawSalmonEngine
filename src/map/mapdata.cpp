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
#include "map/mapdata.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

#include "actor/actor.hpp"
#include "map/tile.hpp"
#include "map/tileset.hpp"
#include "map/layer.hpp"
#include "util/parse.hpp"
#include "util/attribute_parser.hpp"
#include "util/logger.hpp"

/// Plain constructor
MapData::MapData(GameInfo* game, unsigned screen_w, unsigned screen_h) : m_game{game},
m_camera{0, 0, static_cast<int>(screen_w), static_cast<int>(screen_h), this}
#ifndef LIB_BUILD
    ,m_input_handler{*this}
#endif // LIB_BUILD
{}

/**
 * @brief Parses the supplied .tmx file
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

    mpp_renderer = renderer;
    XMLError eResult;

    // Set the base path by trimming filename off path
    m_full_path = filename;
    m_base_path = filename;
    m_base_path = m_base_path.erase(m_base_path.find_last_of('/') + 1);

    // Load the .tmx mapfile from disk
    tinyxml2::XMLDocument mapfile{true, tinyxml2::COLLAPSE_WHITESPACE};
    eResult = mapfile.LoadFile(filename.c_str());
    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Can't find file at: " << filename;
        return eResult;
    }

    // Check for map base element
    XMLElement* pMap = mapfile.FirstChildElement("map");
    if (pMap == nullptr)  {
        Logger(Logger::error) << "Missing base node \"map\" inside .tmx file!";
        return XML_ERROR_PARSING_ELEMENT;
    }

    // Parse map info
    eResult = parse_map_info(pMap);
    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing essential map info!";
        return eResult;
    }

    // Parse map properties
    eResult = parse_map_properties(pMap);
    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed at parsing map properties!";
        return eResult;
    }

    /// @note First parse tilesets, then layers, because layers depend on tileset information
    // This initiates the parsing of all tilesets
    eResult = m_ts_collection.init(pMap, this);
    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed at parsing tilesets!";
        return eResult;
    }

    // Check if all actor templates properly initiated
    for(auto& actor_pair : m_actor_templates) {
        if(!actor_pair.second.is_valid()) {
            Logger(Logger::error) << "Actor called: " << actor_pair.first << " failed to properly parse! Aborting!";
            Logger(Logger::error) << "Probably the name of Actor Animation and Actor Template doesn't match or the template is missing!";
            return XML_ERROR_MISMATCHED_ELEMENT;
        }
    }

    // Get the first layer of three possible layer types
    std::string l = "layer";
    std::string i = "imagelayer";
    std::string o = "objectgroup";
    XMLElement* pLa = pMap->FirstChildElement();
    while(pLa != nullptr) {
        const char* name = pLa->Name();
        if(name == l || name == i || name == o) {break;}
        pLa = pLa->NextSiblingElement();
    }

    // Parse all layers of the map file
    eResult = m_layer_collection.init(pLa, *this);
    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed at parsing layers!";
        return eResult;
    }

    // By default bind the camera to the map borders
    m_camera.bind_map();

    #ifndef LIB_BUILD
        // Trigger the ON_LOAD callback event
        if(m_on_load.valid()) {
            m_events.add_event(m_on_load);
        }
    #endif // LIB_BUILD

    // Initialize last_update timestamp
    m_last_update = SDL_GetTicks();

    return XML_SUCCESS;
}

/**
 * @brief Parse map dimensions, orientation, stagger-axis, stagger-index, hexsidelength and bg-color
 * @param pMap @c XMLElement* which points to the first map file element called "map"
 * @return @c XMLError which indicates sucess or failure
 */
tinyxml2::XMLError MapData::parse_map_info(tinyxml2::XMLElement* pMap) {
    using namespace tinyxml2;
    XMLError eResult;

    AttributeParser parser;
    parser.add(m_width, "width");
    parser.add(m_height, "height");
    std::string orientation, render_order;
    parser.add(orientation, "orientation");
    parser.add(render_order, "renderorder");

    eResult = parser.parse(pMap);
    if(eResult != XML_SUCCESS) {return eResult;}

    if(orientation != "orthogonal" && orientation != "hexagonal" && orientation != "staggered") {
        Logger(Logger::error) << "Tile orientation " << orientation << " isn't supported!";
        return XMLError::XML_WRONG_ATTRIBUTE_TYPE;
    }
    m_tile_layout.orientation = orientation;

    if(render_order != "right-down" && render_order != "right-up" && render_order != "left-down" && render_order != "left-up") {
        Logger(Logger::error) << "Tile render_order " << render_order << " isn't supported!";
        return XMLError::XML_WRONG_ATTRIBUTE_TYPE;
    }
    m_tile_layout.render_order = render_order;

    // Parse the (optional) stagger-axi of the map and check it
    const char* p_stagger_axis = pMap->Attribute("staggeraxis");
    if(p_stagger_axis != nullptr) {
        if(std::string("x") == p_stagger_axis) {
            m_tile_layout.stagger_axis_y = false;
        }
        else if(std::string("y") == p_stagger_axis) {
            m_tile_layout.stagger_axis_y = true;
        }
        else {
            Logger(Logger::error) << "Stagger axis " << p_stagger_axis << " isn't supported! Use x or y!";
            return XMLError::XML_WRONG_ATTRIBUTE_TYPE;
        }
    }

    // Parse the (optional) stagger index and check for illegal values
    const char* p_stagger_index = pMap->Attribute("staggerindex");
    if(p_stagger_index != nullptr) {
        if(std::string("odd") == p_stagger_index) {
            m_tile_layout.stagger_index_odd = true;
        }
        else if(std::string("even") == p_stagger_index) {
            m_tile_layout.stagger_index_odd = false;
        }
        else {
            Logger(Logger::error) << "Stagger index " << p_stagger_index << " isn't supported! Use odd or even!";
            return XMLError::XML_WRONG_ATTRIBUTE_TYPE;
        }
    }

    // Parse the (optional) hexsidelength i case of a hexagonal orientation
    int hexsidelength;
    eResult = pMap->QueryIntAttribute("hexsidelength", &hexsidelength);
    if(eResult == XML_SUCCESS) {
        m_tile_layout.hexsidelength = hexsidelength;
    }

    // Parse possible backgroundcolor
    parse::bg_color(pMap, m_bg_color); // Discard Result since missing bg_color is generally okay

    return XML_SUCCESS;
}

/**
 * @brief Parse on_* callbacks of map and possible symbolic tilesets yielding events
 * @param pMap @c XMLElement* which points to the first map file element called "map"
 * @return @c XMLError which indicates sucess or failure
 */
tinyxml2::XMLError MapData::parse_map_properties(tinyxml2::XMLElement* pMap) {
    using namespace tinyxml2;
    #ifndef LIB_BUILD
        std::vector<std::string> symbolic_tilesets;
        std::string on_load = "";
        std::string on_always = "";
        std::string on_resume = "";

        std::string key_name = "KEY";
        std::string event_name = "EVENT";
    #endif // LIB_BUILD

    // Parse names of possible on_* callbacks and filenames of possible symbolic tilesets
    XMLElement* pProp = pMap->FirstChildElement("properties");
    if (pProp != nullptr) {
        pProp = pProp->FirstChildElement("property");
        while(pProp != nullptr) {

            const char* p_name = pProp->Attribute("name");
            std::string name = (p_name) ? p_name : "";

            #ifndef LIB_BUILD
                if(std::string("ON_LOAD") == p_name) {
                    on_load = pProp->Attribute("value");
                }
                else if(std::string("ON_ALWAYS") == p_name) {
                    on_always = pProp->Attribute("value");
                }
                else if(std::string("ON_RESUME") == p_name) {
                    on_resume = pProp->Attribute("value");
                }
                // Check if symbolic tilesets start with KEY or EVENT
                else if (std::equal(key_name.begin(), key_name.end(), name.begin()) || std::equal(event_name.begin(), event_name.end(), name.begin())) {
                    symbolic_tilesets.push_back(pProp->Attribute("value"));
                }
            #else
                if(false) {}
            #endif
            else {
                XMLError eResult;
                const char* p_type = pProp->Attribute("type");
                std::string type = (p_type) ? p_type : "";
                if(type == "bool") {
                    bool temp;
                    eResult = pProp->QueryBoolAttribute("value", &temp);
                    if(eResult != XML_SUCCESS) {
                        Logger(Logger::error) << "Malformed bool property: " << name;
                        return eResult;
                    }
                    m_data.set_val(name, temp);
                }
                else if(type == "int") {
                    int temp;
                    eResult = pProp->QueryIntAttribute("value", &temp);
                    if(eResult != XML_SUCCESS) {
                        Logger(Logger::error) << "Malformed int property: " << name;
                        return eResult;
                    }
                    m_data.set_val(name, temp);
                }
                else if(type == "float") {
                    float temp;
                    eResult = pProp->QueryFloatAttribute("value", &temp);
                    if(eResult != XML_SUCCESS) {
                        Logger(Logger::error) << "Malformed float property: " << name;
                        return eResult;
                    }
                    m_data.set_val(name, temp);
                }
                else if(type == "" || type == "file") {
                    const char* p_value = pProp->Attribute("value");
                    if(p_value == nullptr) {
                        Logger(Logger::error) << "Malformed string property: " << name;
                        return XML_ERROR_PARSING_ATTRIBUTE;
                    }
                    std::string value = "";
                    if(type == "file") {value += get_file_path();}
                    value += p_value;
                    m_data.set_val(name, value);
                }
                else {
                    Logger(Logger::error) << "Unknown type " << type << " specified! This shouldn't happen at all! Tiled must have messed up";
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
            }
            pProp = pProp->NextSiblingElement();
        }
    }

    #ifndef LIB_BUILD
        // Parse all symbolic tilesets
        /// @warning Dont mix key mapping and event mapping .tsx files
        /// @warning Watch for valid ordering of symbolic tilesets (You can bind a event to a key only after it already got parsed)
        for(std::string name : symbolic_tilesets) {
            XMLDocument sym_ts{true, COLLAPSE_WHITESPACE};
            std::string full_path = m_base_path + name;
            XMLError eResult = sym_ts.LoadFile(full_path.c_str());
            if(eResult != XML_SUCCESS) {
                Logger(Logger::error) << "Can't find symbolic tileset " << name << " at path: " << full_path;
                return eResult;
            }
            XMLElement* pSymTs = sym_ts.FirstChildElement("tileset");
            if (pSymTs == nullptr) {
                Logger(Logger::error) << "Symbolic tileset " << full_path << " is of wrong type/has no \"tileset\" node!";
                return XML_ERROR_PARSING_ELEMENT;
            }

            // Temporarily set base path to symbolic ts path so events can load files from their base path
            std::string path_backup = m_base_path;
            m_base_path = full_path;
            m_base_path = m_base_path.erase(m_base_path.find_last_of('/') + 1);

            // Static function of Tileset class actually takes care of parsing the symbolic tileset
            eResult = Tileset::parse_symbolic(pSymTs, *this);

            //reset base path
            m_base_path = path_backup;

            if(eResult != XML_SUCCESS) {
                Logger(Logger::error) << "Failed at parsing symbolic tileset " << name;
                return eResult;
            }
        }

        // Parse actual events from the on_* callback names
        if(on_load != "") {
            if(check_event_convert_map(on_load) != true) {
                Logger(Logger::error) << "The event: " << on_load << " couldn't be found / is no valid game or map event";
                Logger(Logger::error) << "Failed adding " << on_load << " as the ON_LOAD callback to map";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            else {
                m_on_load = get_event_convert_map(on_load);
            }
        }
        if(on_always != "") {
            if(check_event_convert_map(on_always) != true) {
                Logger(Logger::error) << "The event: " << on_always << " couldn't be found / is no valid game or map event";
                Logger(Logger::error) << "Failed adding " << on_always << " as the ON_ALWAYS callback to map";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            else {
                m_on_always = get_event_convert_map(on_always);
            }
        }
        if(on_resume != "") {
            if(check_event_convert_map(on_resume) != true) {
                Logger(Logger::error) << "The event: " << on_resume << " couldn't be found / is no valid game or map event";
                Logger(Logger::error) << "Failed adding " << on_resume << " as the ON_RESUME callback to map";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
            else {
                m_on_resume = get_event_convert_map(on_resume);
            }
        }
    #endif // LIB_BUILD
    return XML_SUCCESS;
}

/**
 * @brief Renders all map layers
 * @param camera The rectangular area of the map to be rendered
 * @return @c bool which indicates success or failure
 */
bool MapData::render() const{

    SDL_SetRenderDrawColor(*mpp_renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, m_bg_color.a);
    SDL_RenderClear(*mpp_renderer);

    return m_layer_collection.render(m_camera);
}

/**
 * @brief Calls update function of all map layers and animates tiles
 */
void MapData::update() {
    Uint32 current_time = SDL_GetTicks();
    m_delta_time = (current_time - m_last_update) / 1000.f;
    // If debugging use this
    //m_delta_time = 1.0f / 60.0f;
    m_last_update = current_time;

    // Early polling
    m_layer_collection.update(false);

    update_camera();
    // Checks and changes animated tiles
    m_ts_collection.push_all_anim();

    #ifndef LIB_BUILD
        if(m_on_always.valid()) {
            m_events.add_event(m_on_always);
        }
        // Do nothing with returned signal because we don't have to
        m_events.process_events(*this);
    #endif // LIB_BUILD

    // Late polling
    m_layer_collection.update(true);
}

/// If necessary binds camera to actor target and updates the camera position
void MapData::update_camera() {
    Actor* target = fetch_actor(m_camera_target);
    if(m_bind_camera_to_actor && target != nullptr) {m_camera.bind_actor(target);}
    m_camera.update();
}

/**
 * @brief Sets up the map for properly resuming after another map loaded and closed again
 */
void MapData::resume() {
    m_last_update = SDL_GetTicks();
    #ifndef LIB_BUILD
        if(m_on_resume.valid()) {
            m_events.add_event(m_on_resume);
        }
    #endif // LIB_BUILD
}

/// Returns map width in pixels
unsigned MapData::get_w() const {
    int width = m_width * m_ts_collection.get_tile_w();
    if(m_tile_layout.orientation != "orthogonal") {
        if(!m_tile_layout.stagger_axis_y) {
            width /= 2;
            width += m_width * m_tile_layout.hexsidelength / 2;
        }
        width += m_ts_collection.get_tile_w() / 2;
    }
    return width;
}

/// Returns map height in pixels
unsigned MapData::get_h() const {
    int height = m_height * m_ts_collection.get_tile_h();
    if(m_tile_layout.orientation != "orthogonal") {
        if(m_tile_layout.stagger_axis_y) {
            height /= 2;
            height += m_height * m_tile_layout.hexsidelength / 2;
        }
        height += m_ts_collection.get_tile_h() / 2;
    }
    return height;
}


/**
 * @brief Adds a copy of an animation tile to an actor template
 * @param name Name of the Actor template
 * @param anim The @c AnimationType of the tile
 * @param dir The @c Direction of the tile
 * @param tile A pointer to the corresponding animation tile
 */
void MapData::add_actor_animation(std::string name, salmon::AnimationType anim, salmon::Direction dir, Tile* tile) {
    // If actor template doesn't exist, construct it!
    if(m_actor_templates.find(name) == m_actor_templates.end()) {m_actor_templates.insert(std::make_pair(name, Actor(this)));}

    Actor& temp = m_actor_templates.at(name);
    auto& animations = temp.get_animation_container();
    animations[anim][dir] = *tile;
    animations[anim][dir].init_anim();
}

/**
 * @brief Add an Actor template to the vector @c m_actor_templates from an @c XMLElement
 * @param source The @c XMLElement which contains the information
 * @param tile The pointer to the Actor template tile
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError MapData::add_actor_template(tinyxml2::XMLElement* source, Tile* tile) {
    using namespace tinyxml2;
    XMLError eResult;

    // First parse properties into dummy actor simply to retrieve type
    Actor temp(this);
    eResult = temp.parse_properties(source);
    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing properties of actor of type: " << temp.get_type();
        return eResult;
    }

    if(temp.get_type() == "") {
        Logger(Logger::error) << "Actor template with is missing a type!";
        return XML_NO_ATTRIBUTE;
    }

    // If actor template doesn't exist, construct it!
    std::string name = temp.get_type();
    if(m_actor_templates.find(name) == m_actor_templates.end()) {m_actor_templates.insert(std::make_pair(name, Actor(this)));}

    // Parse the real actor
    Actor& current_actor = m_actor_templates.at(name);

    eResult = current_actor.parse_properties(source);
    if(eResult != XML_SUCCESS) {
        Logger(Logger::error) << "Failed parsing properties of actor of type: " << current_actor.get_type();
        return eResult;
    }
    if(current_actor.get_type() == "") {
        Logger(Logger::error) << "Actor template with is missing a type!";
        return XML_NO_ATTRIBUTE;
    }

    current_actor.set_w(tile->get_tileset().get_tile_width());
    current_actor.set_h(tile->get_tileset().get_tile_height());
    current_actor.set_tile(*tile);

    // Make gid an alias of actor template type name
    m_gid_to_actor_temp_name[m_ts_collection.get_gid(tile)] = temp.get_type();

    return XML_SUCCESS;
}

/// @brief Returns the first actor with the given name
Actor* MapData::fetch_actor(std::string name) {
    std::vector<Actor*> actor_list =  m_layer_collection.get_actors(std::string(name));
    if(actor_list.size() > 1) {
        // std::cerr << "Error: More than one actor called " << name<< " !\n";;
    }
    else if(actor_list.size() == 0) {
        // std::cerr << "Error: No actor called " << name << " found!\n";
    }
    else {
        return actor_list[0];
    }
    return nullptr;
}

/// Returns true if the tile with the supplied gid is an actor
bool MapData::is_actor(Uint32 gid) const {
    return m_gid_to_actor_temp_name.find(gid) != m_gid_to_actor_temp_name.end();
}

/// Returns true if the actor with the given name exists
bool MapData::is_actor(std::string name) const {
    return m_actor_templates.find(name) != m_actor_templates.end();
}

/// Return Actor template which was parsed with tile with the given tile ID
Actor MapData::get_actor(Uint32 gid) const {
    return m_actor_templates.at(m_gid_to_actor_temp_name.at(gid));
}

/// Return Actor template by name
Actor MapData::get_actor(std::string name) const {
    return m_actor_templates.at(name);
}
