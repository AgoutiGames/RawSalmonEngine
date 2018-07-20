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
#ifndef MAPDATA_HPP_INCLUDED
#define MAPDATA_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "map/tile.hpp"
#include "map/layer.hpp"
#include "map/tileset.hpp"
#include "util/tinyxml2.h"

/**
 * @brief Contain, render and update all game map layers
 *
 * This class is the starting point of parsing the Tiled mapfile.
 * It parses the used tilesets and map layers and stores them.
 * It renders layers and tiles.
 */

class MapData {
    public:
        MapData() {}
        ~MapData() {}

        tinyxml2::XMLError init_map(std::string filename, SDL_Renderer** renderer);
        bool render(SDL_Rect* camera) const;
        void update();

        unsigned get_overhang(Direction dir) const;
        SDL_Renderer* get_renderer() const {return *mpp_renderer;} ///< Return pointer to the SDL_Renderer
        std::string get_file_path() const {return m_base_path;} ///< Return path to the .tmx map file location
        Uint16 get_gid(Tile* tile)  const;
        Tile* get_tile(Uint16 tile_id) const;
        unsigned get_tile_h() const {return m_tile_h;} ///< Return base tile height
        unsigned get_tile_w() const {return m_tile_w;} ///< Return base tile width
        unsigned get_w() const {return m_width * m_tile_w;} ///< Return map width in pixels
        unsigned get_h() const {return m_height * m_tile_h;} ///< Return map height in pixels
        void register_event(std::pair<std::string, ActorEvent*> event) {m_events.insert(event);} ///< Link event name with @c ActorEvent*
        ActorEvent* get_event(std::string name) const {return m_events.at(name)->copy();} ///< Return copy of named event
        bool check_event(std::string name) const {if(m_events.find(name) != m_events.end()) {return true;} else {return false;}} ///< Return true if event is defined

        const ActorTemplate& get_actor_template(Uint16 gid) const {return m_templates.at(m_gid_to_temp_name.at(gid));}
        ActorTemplate& get_actor_template(std::string actor) {return m_templates[actor];}
        std::vector<Actor*> get_actors(std::string name = "", Direction direction = Direction::invalid,
                                       AnimationType animation = AnimationType::invalid);
        bool collide(const SDL_Rect* rect, int& x_max, int& y_max, std::vector<Actor*>& collided, std::string type = "COLLIDE");
        bool collide(const SDL_Rect* rect, std::vector<Actor*>& collided, std::string type = "COLLIDE");
        bool collide(const SDL_Rect* rect, std::string type = "COLLIDE");

        tinyxml2::XMLError add_actor_template(tinyxml2::XMLElement* source, Tile* tile);
        void add_actor_animation(std::string name, AnimationType anim, Direction dir, Tile* tile);
        bool add_actor_hitbox(std::string actor, std::string hitbox, const SDL_Rect& rect);

        tinyxml2::XMLError parse_tiles_from_tileset(tinyxml2::XMLElement* source, unsigned first_gid);
        bool register_tile(Tile* tile, unsigned gid);
        void set_tile_animated(unsigned gid);
        void set_tile_animated(Tile* tile);

        void init_anim_tiles();
        void push_all_anim();

        bool render(Uint16 tile_id, int x, int y) const;
        bool render(Uint16 tile_id, SDL_Rect& dest) const;

        tinyxml2::XMLError parse_actor_properties(tinyxml2::XMLElement* source, float& speed, Direction& dir, std::map<Response, ActorEvent*>& resp);

        bool register_key(SDL_Keycode key, std::string event, bool sustained, bool up, bool down);
        bool process_key_up(SDL_Event e);
        bool process_key_down(SDL_Event e);
        void process_keys_sustained();

    private:
        tinyxml2::XMLDocument m_mapfile{true, tinyxml2::COLLAPSE_WHITESPACE}; ///< Contains the .tmx map file
        std::string m_base_path = "../data/"; ///< Path to folder where .tmx map files are
        unsigned m_tile_w; // The tile dimensions in pixels
        unsigned m_tile_h;
        unsigned m_width;  // The map dimensions in tiles
        unsigned m_height;
        SDL_Color m_bg_color;
                               //                         | |
        void write_overhang(); // sets the 4 values below v v
        unsigned m_up_overhang = 0;
        unsigned m_down_overhang = 0;
        unsigned m_left_overhang = 0;
        unsigned m_right_overhang = 0;

        std::vector<Tileset> m_tilesets; ///< Contains all used Tilesets
        std::vector<Layer> m_layers; ///< Contains all used layers

        std::vector<Tile*> mp_tiles;      ///< List of pointers to all tiles in order
        std::vector<Uint16> m_anim_tiles; ///< List of ids of all animated tiles
        Tile m_empty_tile; ///< A placeholder which currently has no use except matching gids (this is tile 0)

        std::map<std::string, ActorTemplate> m_templates; ///< List of all actor templates by name
        std::map<Uint16, std::string> m_gid_to_temp_name; ///< List of actor template names by global tile id

        std::map<std::string, ActorEvent*> m_events; ///< List of all parsed events by name
        Actor* m_player = nullptr;

        std::map<SDL_Keycode, ActorEvent*> m_key_up;
        std::map<SDL_Keycode, ActorEvent*> m_key_down;
        std::map<SDL_Scancode, ActorEvent*> m_key_sustained;

        SDL_Renderer** mpp_renderer = nullptr;
};


#endif // MAPDATA_HPP_INCLUDED
