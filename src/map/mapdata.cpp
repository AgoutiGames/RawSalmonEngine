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

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "event/actor_event.hpp"
#include "map/tile.hpp"
#include "map/tileset.hpp"
#include "map/layer.hpp"
#include "map/layer_collection.hpp"
#include "util/game_types.hpp"
#include "util/parse.hpp"

/// Plain constructor
MapData::MapData(unsigned screen_w, unsigned screen_h) : m_camera{0, 0, static_cast<int>(screen_w), static_cast<int>(screen_h)} {}

/// Empty destructor
MapData::~MapData() {}

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

    mpp_renderer = renderer;
    XMLError eResult;
    std::string full_path;

    // Better read this from the tiled map as a custom property
    std::vector<std::string> symbolic_tilesets{"events.tsx", "key_mapping.tsx"};

    for(std::string name : symbolic_tilesets) {
        XMLDocument sym_ts{true, COLLAPSE_WHITESPACE};
        full_path = m_base_path + name;
        eResult = sym_ts.LoadFile(full_path.c_str());
        if(eResult != XML_SUCCESS) {
            std::cerr << "Can't find" << name << " at relative path: " << full_path << "\n";
            return eResult;
        }
        XMLElement* pSymTs = sym_ts.FirstChildElement("tileset");
        if (pSymTs == nullptr) {return XML_ERROR_PARSING_ELEMENT;}
        eResult = Tileset::parse_symbolic(pSymTs, *this);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing symbolic tileset " << name << "\n";
            return eResult;
        }
    }

    /*
    // First parse possible events
    XMLDocument sym_ts{true, COLLAPSE_WHITESPACE};
    std::string full_path = m_base_path + "events.tsx";
    XMLError eResult = sym_ts.LoadFile(full_path.c_str());
    if(eResult != XML_SUCCESS) {
        std::cout << "Can't find events.tsx at relative path: " << full_path << "\n";
        return eResult;
    }
    XMLElement* pSymTs = sym_ts.FirstChildElement("tileset");
    if (pSymTs == nullptr) return XML_ERROR_PARSING_ELEMENT;
    eResult = Tileset::parse_symbolic(pSymTs, *this);
    if(eResult != XML_SUCCESS) {
        std::cerr << "Failed at parsing symbolic tileset events.tsx\n";
        return eResult;
    }
    */


    // This saves the .tmx file to the member m_mapfile
    full_path = m_base_path + filename;
    eResult = m_mapfile.LoadFile(full_path.c_str());
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

    // Parse possible backgroundcolor
    parse::bg_color(pMap, m_bg_color); // Discard Result since missing bg_color is generally okay

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

        m_camera.bind_player(m_player);
    }

    // By default bind the camera to the map borders
    m_camera.bind_map(get_w(),get_h());

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

    m_layer_collection.update();

    m_camera.update();
    // Checks and changes animated tiles
    m_ts_collection.push_all_anim();

    ActorEvent::print_size();
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
        eResult = parse_actor_properties(p_property, a.speed, a.direction, a.response);
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
tinyxml2::XMLError MapData::parse_actor_properties(tinyxml2::XMLElement* source, float& speed, Direction& dir, std::map<Response, ActorEvent*>& resp) {
    using namespace tinyxml2;
    XMLError eResult;
    while(source != nullptr) {
        const char* p_name;
        p_name = source->Attribute("name");
        std::string name(p_name);
        if(p_name == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

        // Parse base speed
        else if(name == "BASE_SPEED") {
            eResult = source->QueryFloatAttribute("value", &speed);
            if(eResult != XML_SUCCESS) {
                std::cerr << "Failed at loading speed value\n";
                return eResult;
            }
        }

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
                if(check_event(event)) {
                    resp[str_to_response(name)] = get_event(event);
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
    if(m_events.find(event) == m_events.end()) {
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
            m_key_sustained[scancode] = get_event(event);
            SDL_Keysym temp;
            temp.sym = key;
            temp.scancode = scancode;
            m_key_sustained[scancode]->set_key(temp);
        }
        if(up) {
            m_key_up[key] = get_event(event);
        }
        if(down) {
            m_key_down[key] = get_event(event);
        }
        return true;
    }
}

/**
 * @brief Add event to player if key is pressed
 * @param e The keypress
 * @return @c bool which indicates if key triggered event
 */
bool MapData::process_key_down(SDL_Event  e) {
    if(m_player != nullptr && m_key_down.find(e.key.keysym.sym) != m_key_down.end()) {
        ActorEvent* event = m_key_down.at(e.key.keysym.sym)->copy();
        event->set_key(e.key.keysym);
        m_player->add_event(event);
        return true;
    }
    return false;
}

/**
 * @brief Add event to player if key is released
 * @param e The keypress
 * @return @c bool which indicates if key triggered event
 */
bool MapData::process_key_up(SDL_Event  e) {
    if(m_player != nullptr && m_key_up.find(e.key.keysym.sym) != m_key_up.end()) {
        ActorEvent* event = m_key_up.at(e.key.keysym.sym)->copy();
        event->set_key(e.key.keysym);
        m_player->add_event(event);
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
        for(std::pair<SDL_Scancode, ActorEvent*> x : m_key_sustained) {
            if(keys[x.first]) {
                m_player->add_event(x.second->copy());
            }
        }
    }
}

unsigned MapData::get_w() const {return m_width * m_ts_collection.get_tile_w();} ///< Return map width in pixels

unsigned MapData::get_h() const {return m_height * m_ts_collection.get_tile_h();} ///< Return map height in pixels

ActorEvent* MapData::get_event(std::string name) const {return m_events.at(name)->copy();} ///< Return copy of named event
