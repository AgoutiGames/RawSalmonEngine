/*
 * Copyright 2017-2020 Agouti Games Team (see the AUTHORS file)
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
#include "map/camera.hpp"
#include "map/layer_collection.hpp"
#include "map/tileset_collection.hpp"
#include "util/game_types.hpp"

namespace salmon {
class Transform;
namespace internal {

class Actor;
class GameInfo;
class Tile;

/**
 * @brief Container for the layers, tilesets, additional data, key/event matrix and camera of the map.
 *
 * Initialize all mentioned submodules by parsing the supplied .tmx file.
 * Call update and render of all submodules.
 */

class MapData {
    public:
        struct TileLayout{
            std::string orientation = "";
            std::string render_order = "";
            int hexsidelength = 0;
            bool stagger_axis_y = true;
            bool stagger_index_odd = true;
        };

    public:
        MapData(GameInfo* game);

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
        void set_file_path(std::string path) {m_base_path = path;}
        std::string get_file_path() const {return m_base_path;} ///< Return path to the .tmx map file location
        std::string get_full_path() const {return m_full_path;} ///< Return path including name of the map

        PixelDimensions get_dimensions() const;

        float get_delta_time() const {return m_delta_time;}
        DataBlock& get_data() {return m_data;}
        GameInfo& get_game() {return *m_game;}
        TilesetCollection& get_ts_collection() {return m_ts_collection;}
        LayerCollection& get_layer_collection() {return m_layer_collection;}
        Camera& get_camera() {return m_camera;}
        const TileLayout get_tile_layout() {return m_tile_layout;}

        // Binds or unbinds camera from player position
        void bind_camera_to_actor(bool state) {if(!state) {m_camera.unbind_actor();} m_bind_camera_to_actor = state;}

        // Actor management
        bool is_actor(Uint32 gid) const;
        bool is_actor(std::string name) const;
        Actor get_actor(Uint32 gid) const;
        Actor get_actor(std::string name) const;

        tinyxml2::XMLError add_actor_template(tinyxml2::XMLElement* source, Tile* tile);
        void add_actor_animation(std::string name, std::string anim, Direction dir, Tile* tile);

        Actor* fetch_actor(std::string name);

        Transform* get_layer_transform(std::string layer_name);

    private:
        unsigned get_w() const;
        unsigned get_h() const;

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

        std::map<std::string, Actor> m_actor_templates; ///< List of all actor templates by name
        std::map<Uint32, std::string> m_gid_to_actor_temp_name; ///< List of actor template names by global tile id

        SDL_Renderer** mpp_renderer = nullptr;
};
}} // namespace salmon::internal

#endif // MAPDATA_HPP_INCLUDED
