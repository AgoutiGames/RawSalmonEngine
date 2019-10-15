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
#include <iostream>

#include "core/gameinfo.hpp"
#include "util/logger.hpp"

#if defined(_WIN32)
#include <SDL.h>
#undef main
#endif

/// The game Loop!
int main()
{
    // Better read resolution from config file
    GameInfo game(1920, 1080);

    if(!game.load_map("main.tmx")) {
        Logger(Logger::fatal) << "Engine startup failed! Entry point \"main.tmx\" couldn't be load!" << std::endl;
    }
    else {
        //While application is running
        while(game.update()) {
            game.render();
        }
    }
    return 0;
}
