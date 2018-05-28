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

#include "map/tile.hpp"
#include "util/game_types.hpp"

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

    // Clear Tile and Tileset data
    mp_tiles.clear();
    mp_tiles.push_back(&m_empty_tile);
    m_anim_tiles.clear();

    m_tilesets.clear();
    m_tilesets.resize(p_tilesets.size());

    // Actually parse each tileset of the vector of pointers
    for(unsigned i = 0; i < p_tilesets.size(); i++) {
        eResult = m_tilesets[i].init(p_tilesets[i], *this);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing Tileset: " << i << "\n";
            return eResult;
        }
    }

    // This must be called after the parsing of all tilesets!
    // It calculates the minimum rendering overhang due to big tiles and tileset offsets
    write_overhang();

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
        XMLError eResult = m_layers[i_layer].init(p_layers[i_layer], *this);
        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at parsing layer: " << i_layer << "\n";
            return eResult;
        }
    }
    // Fetch player
    std::vector<Actor*> actor_list = get_actors(std::string("PLAYER"));
    if(actor_list.size() > 1) {
        std::cerr << "Error: More than one actor called PLAYER!\n";
    }
    else if(actor_list.size() == 0) {
        std::cerr << "Error: No actor called PLAYER found!\n";
    }
    else {
        m_player = actor_list[0];
    }

    // This must be called after the parsing of all tilesets!
    // It sets all animated tiles to their starting positions
    // and passes the current timestamp
    init_anim_tiles();

    return XML_SUCCESS;
}
/**
 * @brief Renders all map layers
 * @param camera The rectangular area of the map to be rendered
 * @return @c bool which indicates success or failure
 */
bool MapData::render(SDL_Rect* camera) const{
    bool success = true;
    // Renders all layers
    for(unsigned i_layer = 0; i_layer < m_layers.size(); i_layer++) {
        if(!m_layers[i_layer].render(camera, *this)) {
            std::cerr << "Failed at rendering layer " << i_layer << " !\n";
            success = false;
        }
    }
    return success;
}
/**
 * @brief Calls update function of all map layers and animates tiles
 */
void MapData::update() {
    for(unsigned i_layer = 0; i_layer < m_layers.size(); i_layer++) {
        m_layers[i_layer].update();
    }
    // Checks and changes animated tiles
    push_all_anim();
}

/**
 * @brief Fetch all actors which conform the supplied parameters
 * @return Vector of conforming actors
 * @note "invalid" value indicates that a parameter is ignored
 */
std::vector<Actor*> MapData::get_actors(std::string name, Direction direction, AnimationType animation) {
    std::vector<Actor*> actor_list;
    for(Layer& layer : m_layers) {
        std::vector<Actor*> sublist = layer.get_actors(name, direction, animation);
        actor_list.insert(actor_list.end(),sublist.begin(),sublist.end());
    }
    return actor_list;
}

/// Returns tile overhang values for the specified direction (up, down, left, right)
unsigned MapData::get_overhang(Direction dir) const{
    switch(dir) {
        case Direction::up:
            return m_up_overhang;
            break;
        case Direction::down:
            return m_down_overhang;
            break;
        case Direction::left:
            return m_left_overhang;
            break;
        case Direction::right:
            return m_right_overhang;
            break;
        default:
            std::cout << "Invalid overhang value requested!\n";
            return 0;
            break;
    }
}

/// Checks for minimum of overhang values for each tileset and saves the corresponding maximum
void MapData::write_overhang() {
    std::map<Direction, unsigned> oh;
    // Check each tileset for maximum overhang values for each direction
    for(Tileset& ts : m_tilesets) {
        std::map<Direction, unsigned> temp;
        temp = ts.determine_overhang(m_tile_w, m_tile_h);
        // Check and write possible new maximum
        if(temp[Direction::up] > oh[Direction::up]) {oh[Direction::up] = temp[Direction::up];}
        if(temp[Direction::down] > oh[Direction::down]) {oh[Direction::down] = temp[Direction::down];}
        if(temp[Direction::left] > oh[Direction::left]) {oh[Direction::left] = temp[Direction::left];}
        if(temp[Direction::right] > oh[Direction::right]) {oh[Direction::right] = temp[Direction::right];}
    }
    // Write the final maximums to the members
    m_up_overhang = oh[Direction::up];
    m_down_overhang = oh[Direction::down];
    m_left_overhang = oh[Direction::left];
    m_right_overhang = oh[Direction::right];
}

/// Initializes all registered animated tiles to the current timestamp and first frame
void MapData::init_anim_tiles() {
    Uint32 time = SDL_GetTicks();
    for(unsigned tile : m_anim_tiles) {
        mp_tiles[tile]->init_anim(time);
    }
}

/**
 * @brief Animates all tiles
 *
 * Checks if next frame of animated tile is due, changes to next frame
 * and wraps around if required.
 * Passing of time is to ensure synchronity of tile animation
 */
void MapData::push_all_anim() {
    Uint32 time = SDL_GetTicks();
    for(unsigned tile : m_anim_tiles) {
        mp_tiles[tile]->push_anim(time);
    }
}

/// Returns the pointer to a tile from it's tile id
Tile* MapData::get_tile(Uint16 tile_id) const{
    if(tile_id >= mp_tiles.size()) {
        std::cerr << "Tile id " << tile_id << " is out of bounds\n";
        return nullptr;
    }
    else return mp_tiles[tile_id];
}

/**
 * @brief Renders a tile by it's id at a coordinate
 * @param x, y The rendering coordinates
 * @param tile_id The id of the tile which will be rendered
 * @return @c bool which indicates success or failure
 *
 * Before rendering it checks if the tile id is valid
 */
bool MapData::render(Uint16 tile_id, int x, int y) const{
    bool success = true;

    // Check if id is valid
    if(tile_id >= mp_tiles.size()) {
        std::cerr << "Tile id " << tile_id << " is out of bounds\n";
        success = false;
    }
    else {
        mp_tiles[tile_id]->render(x,y, *this);
    }
    return success;
}

/**
 * @brief Renders a tile by it's id to a rect
 * @param dest The rendering rectangle
 * @param tile_id The id of the tile which will be rendered
 * @return @c bool which indicates success or failure
 *
 * Before rendering it checks if the tile id is valid.
 * This function can resize the tile image
 */
bool MapData::render(Uint16 tile_id, SDL_Rect& dest) const{
    bool success = true;
    if(tile_id >= mp_tiles.size()) {
        std::cerr << "Tile id " << tile_id << " is out of bounds\n";
        success = false;
    }
    else {
        mp_tiles[tile_id]->render(dest, *this);
    }
    return success;
}

/// Registers tile so it's renderable by its gid
bool MapData::register_tile(Tile* tile, unsigned gid) {
    mp_tiles.push_back(tile);
    if(mp_tiles.size() != gid + 1) {
        std::cerr << "Global tile id does not match! Is: " << gid << " should be: " << mp_tiles.size() - 1 << "\n";
        return false;
    }
    return true;
}

/// Adds tile to the "always animate" list
void MapData::set_tile_animated(unsigned gid) {
    m_anim_tiles.push_back(gid);
}

/// Adds tile to the "always animate" list
void MapData::set_tile_animated(Tile* tile) {
    for(unsigned i = 0; i < mp_tiles.size(); i++) {
        if(mp_tiles[i] == tile) {
            m_anim_tiles.push_back(i);
            return;
        }
    }
    std::cerr << "Could not find Tile to set it to animated, not in global tile list! (has no gid)\n";
}

/**
 * @brief Parse tile information from tileset
 * @param source The @c XMLElement from the tileset
 * @param first_gid The first global tile id of the tileset
 * @return an @c XMLError object which indicates success or error type
 *
 * Determines the tile type and calls the corresponding tile parsers
 */
tinyxml2::XMLError MapData::parse_tiles_from_tileset(tinyxml2::XMLElement* source, unsigned first_gid) {
    using namespace tinyxml2;

    XMLError eResult;

    while(source != nullptr) {
        unsigned tile_id;
        eResult = source->QueryUnsignedAttribute("id", &tile_id);
        if(eResult != XML_SUCCESS) return eResult;
        tile_id += first_gid;

        Tile* T = mp_tiles[tile_id];
        const char* p_type;
        p_type = source->Attribute("type");

        std::string tile_type;
        if(p_type != nullptr) tile_type = p_type;

        // Parse normal map tile
        if(p_type == nullptr) {
            eResult = T->parse_tile(source, first_gid, *this);
        }
        // Parse Actor_Animation tile
        else if(tile_type == "ACTOR_ANIMATION") {
            eResult = T->parse_actor_anim(source, first_gid, *this);
        }
        // Parse Actor_Template
        else if(tile_type == "ACTOR_TEMPLATE") {
            eResult = T->parse_actor_templ(source, *this);
        }
        else {
            std::cerr << "Unknown tile type: " << tile_type << "\n";
            return XML_WRONG_ATTRIBUTE_TYPE;
        }

        if(eResult != XML_SUCCESS) {
            std::cerr << "Failed at loading tile gid: " << tile_id << " local id: " << tile_id - first_gid << "\n";
            return eResult;
        }

        source = source->NextSiblingElement();
    }
    return XML_SUCCESS;
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
                m_gid_to_temp_name[get_gid(tile)] = actor_name;
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
            eResult = parse_hitboxes(p_object, m_templates[actor_name].hitbox);
            if(eResult != XML_SUCCESS) {
                std::cerr << "Failed at parsing hitbox for actor template: " << actor_name << "\n";
                return eResult;
            }
        }
    }
    return XML_SUCCESS;
}

/// Return global Id of a tile if it's registered or 0 if not
Uint16 MapData::get_gid(Tile* tile)  const{
    for(Uint16 i = 0; i < mp_tiles.size(); i++) {
        if(mp_tiles[i] == tile) {
            return i;
        }
    }
    std::cerr << "Could not find Tile to get its gid, not in global list!\n";
    return 0;
}

/**
 * @brief Checks if given rect collides with any layer present in this map
 * @param rect The rect to check against
 * @param x_max, y_max The maximum depth of intersection by axis
 * @param collided A container to which colliding actors are added
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 */
bool MapData::collide(const SDL_Rect* rect, int& x_max, int& y_max, std::vector<Actor*>& collided, std::string type) {
    if(SDL_RectEmpty(rect)) {return false;}
    bool collide = false;
    int x_depth = 0;
    int y_depth = 0;
    // Iterate through all layers
    for(Layer& layer : m_layers) {
        if(layer.collide(rect, x_depth, y_depth, *this, collided, type)) {
            if(x_depth > x_max) {x_max = x_depth;}
            if(y_depth > y_max) {y_max = y_depth;}
            collide = true;
        }
    }
    return collide;
}

/**
 * @brief Checks if given rect collides with any layer present in this map
 * @param rect The rect to check against
 * @param collided A container to which colliding actors are added
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 */
bool MapData::collide(const SDL_Rect* rect, std::vector<Actor*>& collided, std::string type) {
    if(SDL_RectEmpty(rect)) {return false;}
    bool collide = false;
    // Iterate through all layers
    for(Layer& layer : m_layers) {
        if(layer.collide(rect, *this, collided, type)) {
            collide = true;
        }
    }
    return collide;
}

/**
 * @brief Checks if given rect collides with any layer present in this map
 * @param rect The rect to check against
 * @param type The type of the hitbox
 * @return @c bool which indicates collision
 */
bool MapData::collide(const SDL_Rect* rect, std::string type) {
    if(SDL_RectEmpty(rect)) {return false;}
    bool collide = false;
    // Iterate through all layers
    for(Layer& layer : m_layers) {
        if(layer.collide(rect, *this, type)) {
            collide = true;
        }
    }
    return collide;
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
