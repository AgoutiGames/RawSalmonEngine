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

/// Plain constructor
MapData::MapData(GameInfo* game, unsigned screen_w, unsigned screen_h) : m_game{game}, m_camera{0, 0, static_cast<int>(screen_w), static_cast<int>(screen_h)} {}

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
    std::string full_path;

    // Read in the .tmx file to mapfile var
    full_path = filename;

    // Set the base path by trimming filename off path
    m_base_path = full_path;
    m_base_path = m_base_path.erase(m_base_path.find_last_of('/') + 1);

    tinyxml2::XMLDocument mapfile{true, tinyxml2::COLLAPSE_WHITESPACE};
    eResult = mapfile.LoadFile(full_path.c_str());
    if(eResult != XML_SUCCESS) {
        std::cout << "Can't find map at relative path: " << full_path << "\n";
        return eResult;
    }

    // Map info gets parsed
    XMLElement* pMap = mapfile.FirstChildElement("map");
    if (pMap == nullptr) return XML_ERROR_PARSING_ELEMENT;

    eResult = pMap->QueryUnsignedAttribute("width", &m_width);
    if(eResult != XML_SUCCESS) return eResult;
    std::cout << "Map width: " << m_width << "\n";
    eResult = pMap->QueryUnsignedAttribute("height", &m_height);
    if(eResult != XML_SUCCESS) return eResult;
    std::cout << "Map height: " << m_height << "\n";

    const char* p_orientation = pMap->Attribute("orientation");
    if(p_orientation == nullptr) {
        std::cerr << "Missing map orientation value!\n";
        return XMLError::XML_NO_ATTRIBUTE;
    }
    std::string orientation = p_orientation;
    if(orientation != "orthogonal" && orientation != "hexagonal" && orientation != "staggered") {
        std::cerr << "Tile orientation " << orientation << " isn't supported!\n";
        return XMLError::XML_WRONG_ATTRIBUTE_TYPE;
    }
    m_tile_layout.orientation = orientation;

    const char* p_render_order = pMap->Attribute("renderorder");
    if(p_render_order == nullptr) {
        std::cerr << "Missing map render order value!\n";
        return XMLError::XML_NO_ATTRIBUTE;
    }
    std::string render_order = p_render_order;
    if(render_order != "right-down" && render_order != "right-up" && render_order != "left-down" && render_order != "left-up") {
        std::cerr << "Tile render_order " << render_order << " isn't supported!\n";
        return XMLError::XML_WRONG_ATTRIBUTE_TYPE;
    }
    m_tile_layout.render_order = render_order;

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

    int hexsidelength;
    eResult = pMap->QueryIntAttribute("hexsidelength", &hexsidelength);
    if(eResult == XML_SUCCESS) {
        m_tile_layout.hexsidelength = hexsidelength;
    }

    // Parse possible backgroundcolor
    parse::bg_color(pMap, m_bg_color); // Discard Result since missing bg_color is generally okay

    // Parse properties containing symbolic tilesets or the on_load callback
    std::vector<std::string> symbolic_tilesets;
    std::string on_load = "";
    std::string on_always = "";
    std::string on_resume = "";
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
                // std::cerr << "parsed " << pProp->Attribute("value") << "\n";
            }
            pProp = pProp->NextSiblingElement();
        }
    }

    // Parse all symbolic tilesets
    /// @warning Dont mix key mapping and event mapping .tsx files
    /// @warning Watch for valid ordering of symbolic tilesets (You can bind a event to a key only after it already got parsed)
    for(std::string name : symbolic_tilesets) {
        XMLDocument sym_ts{true, COLLAPSE_WHITESPACE};
        full_path = m_base_path + name;
        eResult = sym_ts.LoadFile(full_path.c_str());
        if(eResult != XML_SUCCESS) {
            std::cerr << "Can't find " << name << " at relative path: " << full_path << "\n";
            return eResult;
        }
        XMLElement* pSymTs = sym_ts.FirstChildElement("tileset");
        if (pSymTs == nullptr) {return XML_ERROR_PARSING_ELEMENT;}

        // Temporarily set base path to symbolic ts path so events can load files properly
        std::string temp = m_base_path;
        m_base_path = full_path;
        m_base_path = m_base_path.erase(m_base_path.find_last_of('/') + 1);

        eResult = Tileset::parse_symbolic(pSymTs, *this);

        //reset base path
        m_base_path = temp;


        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing symbolic tileset " << name << "\n";
            return eResult;
        }
    }

    // Extract method from this! Tidying up parsers is important but absolutely no fun :-(
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

    // First parse tilesets, then layers, because layers depend on tileset information

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

    // Fetch player
    std::vector<Actor*> actor_list =  m_layer_collection.get_actors(std::string("PLAYER"));
    if(actor_list.size() > 1) {
        std::cerr << "Error: More than one actor called PLAYER!\n";
    }
    else if(actor_list.size() == 0) {
        std::cerr << "Error: No actor called PLAYER found!\n";
    }
    else {
        m_player = actor_list[0];

        if(m_player_to_camera) {m_camera.bind_player(m_player);}
    }

    // By default bind the camera to the map borders
    m_camera.bind_map(get_w(),get_h());

    // Trigger the ON_LOAD callback event
    if(m_on_load.valid()) {
        m_events.add_event(m_on_load);
    }

    m_last_update = SDL_GetTicks();

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

    m_layer_collection.update();

    m_camera.update();
    // Checks and changes animated tiles
    m_ts_collection.push_all_anim();

    if(m_on_always.valid()) {
        m_events.add_event(m_on_always);
    }
    // Do nothing with returned signal because we don't have to
    m_events.process_events(*this);

    // Check for active actor called PLAYER because it may change during execution
    std::vector<Actor*> actor_list =  m_layer_collection.get_actors(std::string("PLAYER"));
    if(actor_list.size() > 1) {
        // std::cerr << "Error: More than one actor called PLAYER!\n";
    }
    else if(actor_list.size() == 0) {
        // std::cerr << "Error: No actor called PLAYER found!\n";
    }
    else {
        m_player = actor_list[0];

        if(m_player_to_camera) {m_camera.bind_player(m_player);}
    }
}

/**
 * @brief Adds a copy of an animation tile to an actor template
 * @param name Name of the @c ActorTemplate
 * @param anim The @c AnimationType of the tile
 * @param dir The @c Direction of the tile
 * @param tile A pointer to the corresponding animation tile
 */
void MapData::add_actor_animation(std::string name, AnimationType anim, Direction dir, Tile* tile) {
    // Get a reference to the ActorTemplate from its name
    ActorTemplate& templ = m_templates[name];

    // Add a copy of the tile to the ActorTemplate
    templ.animations[anim][dir] = *tile;

    // Initialize the animation state of the copied tile
    templ.animations[anim][dir].init_anim();
}

/**
 * @brief Add the hitbox by its name to the actor template
 * @param actor The name of the actor
 * @param hitbox The name of the hitbox
 * @param rect The data of the hitbox
 * @return @c bool which indicates multiple definition of the same hitbox/ error
 */
bool MapData::add_actor_hitbox(std::string actor, std::string hitbox, const SDL_Rect& rect) {
    // Get a reference to the ActorTemplate from its name
    ActorTemplate& templ = m_templates[actor];
    if(templ.hitbox.find(hitbox) != templ.hitbox.end()) {
        std::cerr << "Multiple definition of the hitbox " << hitbox << " of the actor " << actor << "\n";
        return false;
    }
    else {
        templ.hitbox[hitbox] = rect;
        return true;
    }
}

/**
 * @brief Add an @c ActorTemplate to the vector @c m_templates from an @c XMLElement
 * @param source The @c XMLElement which contains the information
 * @param tile The pointer to the @c ActorTemplate tile
 * @return an @c XMLError object which indicates success or error type
 */
tinyxml2::XMLError MapData::add_actor_template(tinyxml2::XMLElement* source, Tile* tile) {
    using namespace tinyxml2;
    XMLError eResult;

    std::string actor_name;

    // Parse user specified properties of the ActorTemplate
    XMLElement* p_tile_properties = source->FirstChildElement("properties");
    if(p_tile_properties != nullptr) {
        XMLElement* p_property = p_tile_properties->FirstChildElement("property");

        // First check for name because this is a prequisite for parsing the remaining information
        const char* p_name;
        p_name = p_property->Attribute("name");
        std::string name(p_name);
        if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

        // Parse name of the ActorTemplate
        else if(name == "ACTOR_NAME") {
            const char* p_actor_name = p_property->Attribute("value");
            if(p_actor_name != nullptr) {

                // Register the ActorTemplate
                actor_name = p_actor_name;
                m_templates[actor_name].template_name = actor_name;
                m_gid_to_temp_name[m_ts_collection.get_gid(tile)] = actor_name;
            }
            else {
                std::cerr << "Invalid actor name in actor template\n";
                return XML_ERROR_PARSING_ATTRIBUTE;
            }
        }
        else {
            std::cerr << "Actor name isn't first property of actor template\n";
            return XML_NO_ATTRIBUTE;
        }
        p_property = p_property->NextSiblingElement("property");

        // Parse user specified properties of the ActorTemplate
        ActorTemplate& a = m_templates[actor_name];
        eResult = parse_actor_properties(p_property, a.direction, a.response);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing actor properties for actor template of actor: " << actor_name << "\n";
            return eResult;
        }
    }
    else {
        std::cerr << "Missing properties on actor template\n";
        return XML_NO_ATTRIBUTE;
    }

    // Parse the default hitbox
    XMLElement* p_objgroup = source->FirstChildElement("objectgroup");
    if(p_objgroup != nullptr) {
        XMLElement* p_object = p_objgroup->FirstChildElement("object");
        if(p_object != nullptr) {
            eResult = parse::hitboxes(p_object, m_templates[actor_name].hitbox);
            if(eResult != XML_SUCCESS) {
                std::cerr << "Failed at parsing hitbox for actor template: " << actor_name << "\n";
                return eResult;
            }
        }
    }
    return XML_SUCCESS;
}

/**
 * @brief Parse the properties of an actor via @c XMLElement
 * @param source The first property element
 * @param speed, dir, resp The possible actor member vars which can be defined via properties
 * @return @c XMLError which indicates success or failure
 */
tinyxml2::XMLError MapData::parse_actor_properties(tinyxml2::XMLElement* source, Direction& dir, EventCollection<Actor, Response>& resp) {
    using namespace tinyxml2;
    while(source != nullptr) {
        const char* p_name;
        p_name = source->Attribute("name");
        std::string name(p_name);
        if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

        // Parse current direction facing
        else if(name == "DIRECTION") {
            const char* p_direction = source->Attribute("value");
            if(p_direction != nullptr) {
                dir = str_to_direction(std::string(p_direction));
                if(dir == Direction::invalid) {
                    std::cerr << "Invalid direction type \"" << p_direction << "\"specified\n";
                    return XML_WRONG_ATTRIBUTE_TYPE;
                }

                if(dir == Direction::current) {
                    std::cerr << "There is no current direction upon actor initialization\n";
                    return XML_WRONG_ATTRIBUTE_TYPE;
                }
            }
            else {
                std::cerr << "Empty direction value specified\n";
                return XML_NO_ATTRIBUTE;
            }

        }

        // Parse response values
        else if(str_to_response(name) != Response::invalid) {
            const char* p_event = source->Attribute("value");
            if(p_event != nullptr) {
                std::string event(p_event);
                if(check_event_convert_actor(event)) {
                    resp.register_event(str_to_response(name), get_event_convert_actor(event));
                }
                else {
                    std::cerr << "An event called: " << event << " does not exist/ never got parsed!\n";
                    return XML_ERROR_PARSING_ATTRIBUTE;
                }
            }
            else {
                std::cerr << "Empty response event specified\n";
                return XML_NO_ATTRIBUTE;
            }
        }

        else {
            std::cerr << "Unknown actor property \"" << p_name << "\" specified\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
        // Move to next property
        source = source->NextSiblingElement("property");
    }
    return XML_SUCCESS;
}

/**
 * @brief Links key with event which is sent to player upon press/release
 * @param key The keypress which triggers the event
 * @param event The event which gets triggered by a key
 * @param sustained, up, down Booleans which indicate when the event should be sent
 */
bool MapData::register_key(SDL_Keycode key, std::string event, bool sustained, bool up, bool down) {
    if(!check_event_convert_actor(event)) {
        std::cerr << "An event called: " << event << " does not exist/ never got parsed!\n";
        return false;
    }
    if( (sustained && up) || (sustained && down) ) {
        std::cerr << "Cant parse key event as sustained AND up or down\n";
        return false;
    }
    else {
        if(sustained) {
            SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
            if(scancode == SDL_SCANCODE_UNKNOWN) {
                std::cerr << "No corresponding scancode to key " << key <<" which is required for checking sustained\n";
                return false;
            }
            SmartEvent<Actor> event_data = get_event_convert_actor(event);
            SDL_Keysym temp;
            temp.sym = key;
            temp.scancode = scancode;
            event_data->set_cause(Cause(temp));
            m_key_sustained.register_event(scancode, event_data);
        }
        if(up) {
            //m_key_up[key] = get_event(event);
            m_key_up.register_event(key,get_event_convert_actor(event));
        }
        if(down) {
            //m_key_down[key] = get_event(event);
            m_key_down.register_event(key,get_event_convert_actor(event));
        }
        return true;
    }
}

/**
 * @brief Adds event to player if key is pressed
 * @param e The keypress
 * @return @c bool which indicates if key triggered event
 */
bool MapData::process_key_down(SDL_Event  e) {
    if(m_player != nullptr && m_key_down.check_event(e.key.keysym.sym)) {
        SmartEvent<Actor> event = m_key_down.get_event(e.key.keysym.sym);
        event->set_cause(Cause(e.key.keysym));
        m_player->get_event_queue().add_event(event);
        return true;
    }
    return false;
}

/**
 * @brief Adds event to player if key is released
 * @param e The keypress
 * @return @c bool which indicates if key triggered event
 */
bool MapData::process_key_up(SDL_Event  e) {
    if(m_player != nullptr && m_key_up.check_event(e.key.keysym.sym)) {
        SmartEvent<Actor> event = m_key_up.get_event(e.key.keysym.sym);
        event->set_cause(Cause(e.key.keysym));
        m_player->get_event_queue().add_event(event);
        return true;
    }
    return false;
}

/**
 * @brief Checks if key is down and sends associated event to player
 */
void MapData::process_keys_sustained() {
    if(m_player != nullptr) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        for(std::pair<const SDL_Scancode, SmartEvent<Actor>>& x : m_key_sustained.get_container()) {
            if(keys[x.first]) {
                m_player->get_event_queue().add_event(x.second);
            }
        }
    }
}

/// Return ActorTemplate which was parsed with tile with the given tile ID
const ActorTemplate& MapData::get_actor_template(Uint16 gid) const {
    return m_templates.at(m_gid_to_temp_name.at(gid));
}

/// Return ActorTemplate by name
ActorTemplate& MapData::get_actor_template(std::string actor) {
    return m_templates[actor];
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
