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

#include <SDL2/SDL.h>
#include <string>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"

/**
 * @brief This class manages interaction between player and game modules
 */

class GameInfo {
public:
    GameInfo(unsigned screen_w, unsigned screen_h);
    ~GameInfo();

    bool init();
    void close();

    void render();
    bool update();

    bool load_map(std::string mapfile);


    bool m_key_repeat = false; // If false, ignores automatically repeated key presses

private:
    SDL_Window* m_window = nullptr;
    std::string m_window_title = "Salmon";
    SDL_Renderer* m_renderer = nullptr;
    unsigned m_screen_w;
    unsigned m_screen_h;


    MapData m_map; ///< Stores the currently active game map
};

#endif // GAMEINFO_HPP_INCLUDED
