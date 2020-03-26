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

#include "actor/data_block.hpp"
#include "audio/audio_manager.hpp"
#include "core/input_cache.hpp"
#include "core/font_manager.hpp"
#include "graphics/texture_cache.hpp"
#include "util/preloader.hpp"

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

    void set_window_size(unsigned width, unsigned height);
    bool set_fullscreen(bool mode);
    bool set_game_resolution(unsigned width, unsigned height);
    bool set_linear_filtering(bool mode);
    void set_window_resizable(bool mode); ///< Currently not working properly! Need to handle resize events!

    unsigned get_game_x_resolution() const {return m_x_resolution;}
    unsigned get_game_y_resolution() const {return m_y_resolution;}

    unsigned get_window_x_resolution() const;
    unsigned get_window_y_resolution() const;

    unsigned get_screen_x_resolution() const;
    unsigned get_screen_y_resolution() const;

    bool window_minimized() const {return m_window_minimized;}
    bool window_active() const {return m_window_active;}

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

    SDL_Window* m_window = nullptr;
    std::string m_window_title = "Salmon";
    SDL_Renderer* m_renderer = nullptr;
    // default screen resolution
    unsigned m_x_resolution = 800;
    unsigned m_y_resolution = 600;

    unsigned m_window_w = 800;
    unsigned m_window_h = 600;

    bool m_fullscreen = false;

    bool m_window_minimized = false;
    bool m_window_active = true;

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

#endif // GAMEINFO_HPP_INCLUDED
