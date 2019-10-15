/*
 * Copyright 2017-2019 Agouti Games Team (see the AUTHORS file)
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

#include <SDL.h>
#include <vector>
#include <string>
#include <tinyxml2.h>

#include "actor/data_block.hpp"
#include "core/input_handler.hpp"
#include "event/ae_me_wrapper.hpp"
#include "event/ae_ge_wrapper.hpp"
#include "event/me_ge_wrapper.hpp"
#include "event/event_collection.hpp"
#include "event/event_queue.hpp"
#include "event/wide_event_collection.hpp"
#include "map/camera.hpp"
#include "map/layer_collection.hpp"
#include "map/tileset_collection.hpp"
#include "util/game_types.hpp"

class Actor;
class Tile;
class Layer;

template <class Scope>
class SmartEvent;

/**
 * @brief Container for the layers, tilesets, additional data, key/event matrix and camera of the map.
 *
 * Initialize all mentioned submodules by parsing the supplied .tmx file.
 * Call update and render of all submodules.
 */

class MapData {
    public:
        MapData(GameInfo* game, unsigned screen_w, unsigned screen_h);

        MapData(const MapData& other) = delete;
        MapData& operator=(const MapData& other) = delete;

        MapData(MapData&& other) = default;
        MapData& operator=(MapData&& other) = default;

        // Map parsing functions
        tinyxml2::XMLError init_map(std::string filename, SDL_Renderer** renderer);
        tinyxml2::XMLError parse_map_info(tinyxml2::XMLElement* pMap);
        tinyxml2::XMLError parse_map_properties(tinyxml2::XMLElement* pMap);

        bool render() const;
        void update();
        void update_camera();
        void resume();

        // Trivial Getters
        SDL_Renderer* get_renderer() const {return *mpp_renderer;} ///< Return pointer to the SDL_Renderer
        std::string get_file_path() const {return m_base_path;} ///< Return path to the .tmx map file location
        std::string get_full_path() const {return m_full_path;}

        unsigned get_w() const;
        unsigned get_h() const;

        float get_delta_time() const {return m_delta_time;}
        DataBlock& get_data() {return m_data;}
        GameInfo& get_game() {return *m_game;}
        TilesetCollection& get_ts_collection() {return m_ts_collection;}
        LayerCollection& get_layer_collection() {return m_layer_collection;}
        EventQueue<MapData>& get_event_queue() {return m_events;}
        Camera& get_camera() {return m_camera;}
        const TileLayout get_tile_layout() {return m_tile_layout;}
        std::string get_key_target() const {return m_key_target;}
        InputHandler& get_input_handler() {return m_input_handler;}

        // Binds or unbinds camera from player position
        void bind_camera_to_actor(bool state) {if(!state) {m_camera.unbind_actor();} m_bind_camera_to_actor = state;}

        // Event management
        template<class Scope=Actor, class Key=std::string>
        void register_event(std::pair<Key, SmartEvent<Scope>> event) {m_event_archive.register_event<Scope,Key>(event);} ///< Link event name with @c Event<Actor>*

        template<class Scope=Actor, class Key=std::string>
        SmartEvent<Scope> get_event(Key name) const {return m_event_archive.get_event<Scope,Key>(name);}

        template<class Scope=Actor, class Key=std::string>
        bool check_event(Key name) const {return m_event_archive.check_event<Scope,Key>(name);} ///< Return true if event is defined

        template<class Key=std::string>
        bool check_event_convert_actor(Key name) const;

        template<class Key=std::string>
        bool check_event_convert_map(Key name) const;

        template<class Key=std::string>
        SmartEvent<Actor> get_event_convert_actor(Key name) const;

        template<class Key=std::string>
        SmartEvent<MapData> get_event_convert_map(Key name) const;

        // Actor management
        bool is_actor(Uint32 gid) const;
        bool is_actor(std::string name) const;
        Actor get_actor(Uint32 gid) const;
        Actor get_actor(std::string name) const;

        tinyxml2::XMLError add_actor_template(tinyxml2::XMLElement* source, Tile* tile);
        void add_actor_animation(std::string name, AnimationType anim, Direction dir, Tile* tile);

        // Key input management
        /*
        bool register_key(SDL_Keycode key, std::string event, bool sustained, bool up, bool down);
        bool process_key_up(SDL_Event e);
        bool process_key_down(SDL_Event e);
        void process_keys_sustained();
        */

        Actor* fetch_actor(std::string name);

    private:

        GameInfo* m_game;

        std::string m_full_path = ""; ///< Path to tmx file
        std::string m_base_path = ""; ///< Path to folder where .tmx map files are
        unsigned m_width;  // The map dimensions in tiles
        unsigned m_height;
        SDL_Color m_bg_color;

        DataBlock m_data; ///< This holds custom user values by string
        Uint32 m_last_update;
        float m_delta_time = 0.f;

        Camera m_camera;
        bool m_bind_camera_to_actor = true;
        std::string m_camera_target = "PLAYER";

        LayerCollection m_layer_collection;

        TileLayout m_tile_layout;

        TilesetCollection m_ts_collection;

        InputHandler m_input_handler;

        std::map<std::string, Actor> m_actor_templates; ///< List of all actor templates by name
        std::map<Uint32, std::string> m_gid_to_actor_temp_name; ///< List of actor template names by global tile id

        std::string m_key_target = "PLAYER";

        /*
        EventCollection<Actor, SDL_Keycode> m_key_up;
        EventCollection<Actor, SDL_Keycode> m_key_down;
        EventCollection<Actor, SDL_Scancode> m_key_sustained;
        */

        WideEventCollection m_event_archive;

        EventQueue<MapData> m_events;
        SmartEvent<MapData> m_on_load;
        SmartEvent<MapData> m_on_always;
        SmartEvent<MapData> m_on_resume;

        SDL_Renderer** mpp_renderer = nullptr;
};

/// Return true if event is defined as either Game/Map or ActorEvent
template<class Key>
bool MapData::check_event_convert_actor(Key name) const {
    return m_event_archive.check_event<Actor,Key>(name) ||
    m_event_archive.check_event<MapData,Key>(name) ||
    m_event_archive.check_event<GameInfo,Key>(name);
}

/// Return true if event is defined as either Game or MapEvent
template<class Key>
bool MapData::check_event_convert_map(Key name) const {
    return m_event_archive.check_event<MapData,Key>(name) ||
    m_event_archive.check_event<GameInfo,Key>(name);
}

/// Return ActorEvent and if needed wrap Game or MapEvent as an ActorEvent
template<class Key>
SmartEvent<Actor> MapData::get_event_convert_actor(Key name) const {
    if(m_event_archive.check_event<Actor,Key>(name)) {
        return m_event_archive.get_event<Actor,Key>(name);
    }
    else if(m_event_archive.check_event<MapData,Key>(name)) {
        return SmartEvent<Actor>(new AeMeWrapper("generated", m_event_archive.get_event<MapData,Key>(name)));
    }
    else if(m_event_archive.check_event<GameInfo,Key>(name)) {
        return SmartEvent<Actor>(new AeGeWrapper("generated", m_event_archive.get_event<GameInfo,Key>(name)));
    }
    else {
        return SmartEvent<Actor>();
    }
}

/// Return MapEvent and if needed wrap GameEvent as a MapEvent
template<class Key>
SmartEvent<MapData> MapData::get_event_convert_map(Key name) const {
    if(m_event_archive.check_event<MapData,Key>(name)) {
        return m_event_archive.get_event<MapData,Key>(name);
    }
    else if(m_event_archive.check_event<GameInfo,Key>(name)) {
        return SmartEvent<MapData>(new MeGeWrapper("generated", m_event_archive.get_event<GameInfo,Key>(name)));
    }
    else {
        return SmartEvent<MapData>();
    }
}

#endif // MAPDATA_HPP_INCLUDED
