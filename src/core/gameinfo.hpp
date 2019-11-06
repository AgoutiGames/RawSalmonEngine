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
#ifndef GAMEINFO_HPP_INCLUDED
#define GAMEINFO_HPP_INCLUDED

#include <SDL.h>
#include <string>
#include <stack>

#include "actor/data_block.hpp"
#include "event/event_queue.hpp"
#include "core/input_cache.hpp"
#include "core/font_manager.hpp"

class MapData;

/**
 * @brief Manages the initialization of SDL2 and the game map. Pipe keyboard input to the map.
 *
 */

class GameInfo {
public:
    GameInfo(unsigned screen_w, unsigned screen_h);
    ~GameInfo();

    bool init();
    void close();

    void render();
    bool update();

    bool load_map(std::string mapfile, bool absolute = false);
    void close_map();

    DataBlock& get_data() {return m_data;}

    bool m_key_repeat = false; // If false, ignores automatically repeated key presses

    EventQueue<GameInfo>& get_event_queue() {return m_events;}
    FontManager& get_font_manager() {return m_font_manager;}
    InputCache& get_input_cache() {return m_input_cache;}

private:
    void update_path();

    SDL_Window* m_window = nullptr;
    std::string m_window_title = "Salmon";
    SDL_Renderer* m_renderer = nullptr;
    unsigned m_screen_w;
    unsigned m_screen_h;

    InputCache m_input_cache;
    DataBlock m_data; ///< This holds custom user values by string

    EventQueue<GameInfo> m_events;
    FontManager m_font_manager;

    std::string m_current_path = "../data/";

    std::stack<MapData> m_maps; ///< Stores the currently active game map
};

#endif // GAMEINFO_HPP_INCLUDED
