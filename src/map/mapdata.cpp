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

#include <iostream>
#include <fstream>
#include <sstream>

#include "actor/actor.hpp"
#include "map/tile.hpp"
#include "map/tileset.hpp"
#include "map/layer.hpp"
#include "util/parse.hpp"
#include "util/attribute_parser.hpp"

/// Plain constructor
MapData::MapData(GameInfo* game, unsigned screen_w, unsigned screen_h) : m_game{game}, m_camera{0, 0, static_cast<int>(screen_w), static_cast<int>(screen_h)}, m_input_handler{*this} {}

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
    m_base_path = filename;
    m_base_path = m_base_path.erase(m_base_path.find_last_of('/') + 1);

    // Load the .tmx mapfile from disk
    tinyxml2::XMLDocument mapfile{true, tinyxml2::COLLAPSE_WHITESPACE};
    eResult = mapfile.LoadFile(filename.c_str());
    if(eResult != XML_SUCCESS) {
        std::cout << "Can't find map at relative path: " << filename << "\n";
        return eResult;
    }

    // Check for map base element
    XMLElement* pMap = mapfile.FirstChildElement("map");
    if (pMap == nullptr)  {
        std::cerr << "Missing base node \"map\" inside .tmx file " << filename << "\n";
        return XML_ERROR_PARSING_ELEMENT;
    }

    // Parse map info
    eResult = parse_map_info(pMap);
    if(eResult != XML_SUCCESS) {
        std::cerr << "Couldn't parse essential map info of map " << filename << "\n";
        return eResult;
    }

    // Parse map properties
    eResult = parse_map_properties(pMap);
    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed at parsing map properties of map " << filename << "\n";
        return eResult;
    }

    /// @note First parse tilesets, then layers, because layers depend on tileset information
    // This initiates the parsing of all tilesets
    eResult = m_ts_collection.init(pMap, this);
    if(eResult != XML_SUCCESS) {
        std::cerr << "Error at parsing tilesets!\n";
        return eResult;
    }

    // Get the first Element that isn't a tileset (can be layer, imagelayer or objectgroup)
    XMLElement* pLa;
    for(pLa = pMap->FirstChildElement("tileset");
        pLa != nullptr && std::string("tileset") == pLa->Name();
        pLa = pLa->NextSiblingElement()) {;}

    // Parse all layers of the map file
    eResult = m_layer_collection.init(pLa, *this);
    if(eResult != XML_SUCCESS) {
        std::cerr << "Error at parsing layers!\n";
        return eResult;
    }

    // By default bind the camera to the map borders
    m_camera.bind_map(get_w(),get_h());

    // Trigger the ON_LOAD callback event
    if(m_on_load.valid()) {
        m_events.add_event(m_on_load);
    }

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

    std::cout << "Map width: " << m_width << "\n";
    std::cout << "Map height: " << m_height << "\n";

    if(orientation != "orthogonal" && orientation != "hexagonal" && orientation != "staggered") {
        std::cerr << "Tile orientation " << orientation << " isn't supported!\n";
        return XMLError::XML_WRONG_ATTRIBUTE_TYPE;
    }
    m_tile_layout.orientation = orientation;

    if(render_order != "right-down" && render_order != "right-up" && render_order != "left-down" && render_order != "left-up") {
        std::cerr << "Tile render_order " << render_order << " isn't supported!\n";
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
            std::cerr << "Stagger axis " << p_stagger_axis << " isn't supported!\n";
            std::cerr << "Use x or y!\n";
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
            std::cerr << "Stagger index " << p_stagger_index << " isn't supported!\n";
            std::cerr << "Use odd or even!\n";
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

    std::vector<std::string> symbolic_tilesets;
    std::string on_load = "";
    std::string on_always = "";
    std::string on_resume = "";

    // Parse names of possible on_* callbacks and filenames of possible symbolic tilesets
    XMLElement* pProp = pMap->FirstChildElement("properties");
    if (pProp != nullptr) {
        pProp = pProp->FirstChildElement("property");
        while(pProp != nullptr) {
            if(std::string("ON_LOAD") == pProp->Attribute("name")) {
                on_load = pProp->Attribute("value");
            }
            else if(std::string("ON_ALWAYS") == pProp->Attribute("name")) {
                on_always = pProp->Attribute("value");
            }
            else if(std::string("ON_RESUME") == pProp->Attribute("name")) {
                on_resume = pProp->Attribute("value");
            }
            else {
                symbolic_tilesets.push_back(pProp->Attribute("value"));
            }
            pProp = pProp->NextSiblingElement();
        }
    }

    // Parse all symbolic tilesets
    /// @warning Dont mix key mapping and event mapping .tsx files
    /// @warning Watch for valid ordering of symbolic tilesets (You can bind a event to a key only after it already got parsed)
    for(std::string name : symbolic_tilesets) {
        XMLDocument sym_ts{true, COLLAPSE_WHITESPACE};
        std::string full_path = m_base_path + name;
        XMLError eResult = sym_ts.LoadFile(full_path.c_str());
        if(eResult != XML_SUCCESS) {
            std::cerr << "Can't find symbolic tileset " << name << " at relative path: " << full_path << "\n";
            return eResult;
        }
        XMLElement* pSymTs = sym_ts.FirstChildElement("tileset");
        if (pSymTs == nullptr) {
            std::cerr << "Symbolic tileset " << full_path << " is of wrong type/has no \"tileset\" node!\n";
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
            std::cerr << "Failed at parsing symbolic tileset " << name << "\n";
            return eResult;
        }
    }

    // Parse actual events from the on_* callback names
    if(on_load != "") {
        if(check_event_convert_map(on_load) != true) {
            std::cerr << "The event: " << on_load << " couldn't be found / is no valid game or map event\n";
            std::cerr << "Failed adding " << on_load << " as the ON_LOAD callback to map\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_on_load = get_event_convert_map(on_load);
        }
    }
    if(on_always != "") {
        if(check_event_convert_map(on_always) != true) {
            std::cerr << "The event: " << on_always << " couldn't be found / is no valid game or map event\n";
            std::cerr << "Failed adding " << on_always << " as the ON_ALWAYS callback to map\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_on_always = get_event_convert_map(on_always);
        }
    }
    if(on_resume != "") {
        if(check_event_convert_map(on_resume) != true) {
            std::cerr << "The event: " << on_resume << " couldn't be found / is no valid game or map event\n";
            std::cerr << "Failed adding " << on_resume << " as the ON_RESUME callback to map\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        else {
            m_on_resume = get_event_convert_map(on_resume);
        }
    }
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

    if(m_on_always.valid()) {
        m_events.add_event(m_on_always);
    }
    // Do nothing with returned signal because we don't have to
    m_events.process_events(*this);

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
    if(m_on_resume.valid()) {
        m_events.add_event(m_on_resume);
    }
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
void MapData::add_actor_animation(std::string name, AnimationType anim, Direction dir, Tile* tile) {
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

    Actor temp(this);
    eResult = temp.parse_properties(source);
    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing properties of actor of type: " << temp.get_type() << "\n";
        return eResult;
    }

    eResult = temp.parse_hitbox(source);
    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed parsing hitbox of actor of type: " << temp.get_type() << "\n";
        return eResult;
    }

    if(temp.get_type() == "") {
        std::cerr << "Actor template with is missing a type!\n";
        return XML_NO_ATTRIBUTE;
    }

    // Store the parsed actor in the actor templates map
    m_actor_templates[temp.get_type()] = temp;
    // Make gid an alias of actor template type name
    m_gid_to_actor_temp_name[m_ts_collection.get_gid(tile)] = temp.get_type();

    return XML_SUCCESS;
}

/// @brief Returns the first actor with the given name
Actor* MapData::fetch_actor(std::string name) {
    std::vector<Actor*> actor_list =  m_layer_collection.get_actors(std::string(name));
    if(actor_list.size() > 1) {
        std::cerr << "Error: More than one actor called " << name<< " !\n";;
    }
    else if(actor_list.size() == 0) {
        std::cerr << "Error: No actor called " << name << " found!\n";
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

/// Return Actor template which was parsed with tile with the given tile ID
Actor MapData::get_actor(Uint32 gid) const {
    return m_actor_templates.at(m_gid_to_actor_temp_name.at(gid));
}

/// Return Actor template by name
Actor MapData::get_actor(std::string name) const {
    return m_actor_templates.at(name);
}
