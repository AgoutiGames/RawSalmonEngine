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
#ifndef GAMEINFO_HPP_INCLUDED
#define GAMEINFO_HPP_INCLUDED

#include <SDL.h>
#include <string>
#include <stack>

#include "window.hpp"
#include "actor/data_block.hpp"
#include "audio/audio_manager.hpp"
#include "core/input_cache.hpp"
#include "core/font_manager.hpp"
#include "graphics/texture_cache.hpp"
#include "util/preloader.hpp"

namespace salmon { namespace internal {

class MapData;

/**
 * @brief Manages the initialization of SDL2 and the game map. Pipe keyboard input to the map.
 *
 */

class GameInfo {
public:
    GameInfo();
    ~GameInfo();

    void render();
    bool update();

    bool load_map(std::string mapfile, bool absolute = false);
    void close_map();

    bool set_game_resolution(unsigned width, unsigned height);
    bool set_linear_filtering(bool mode);

    Window& get_window() {return m_window;}

    unsigned get_game_x_resolution() const {return m_x_resolution;}
    unsigned get_game_y_resolution() const {return m_y_resolution;}

    MapData& get_map();
    std::vector<MapData>& get_maps();
    DataBlock& get_data() {return m_data;}
    std::string get_base_path() const {return m_base_path;}
    std::string get_resource_path() const {return m_resource_path;}

    bool m_key_repeat = false; // If false, ignores automatically repeated key presses

    Preloader& get_preloader() {return m_preloader;}
    TextureCache& get_texture_cache() {return m_texture_cache;}
    AudioManager& get_audio_manager() {return m_audio_manager;}
    FontManager& get_font_manager() {return m_font_manager;}
    InputCache& get_input_cache() {return m_input_cache;}

private:
    bool init();
    void close();

    void update_path();

    std::string m_window_title = "Salmon";
    SDL_Renderer* m_renderer = nullptr;
    // default screen resolution
    unsigned m_x_resolution = 800;
    unsigned m_y_resolution = 600;

    Window m_window;
    Preloader m_preloader;
    TextureCache m_texture_cache;
    AudioManager m_audio_manager;
    InputCache m_input_cache;
    DataBlock m_data; ///< This holds custom user values by string

    FontManager m_font_manager;

    std::string m_base_path = ""; ///< Path to the directory where the executable lies

    #ifdef __EMSCRIPTEN__
        std::string m_resource_path = "data"; ///< Path to the data directory where typically all game assets are
    #else
        std::string m_resource_path = "../data"; ///< Path to the data directory where typically all game assets are
    #endif

    std::string m_current_path = ""; ///< Path to the directory of the currently active mapfile

    std::vector<MapData> m_maps; ///< Stores the currently active game map
};
}} // namespace salmon::internal

#endif // GAMEINFO_HPP_INCLUDED
