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

using namespace std;

/// The game Loop!
int main()
{
    GameInfo game(800, 600);
    if(!game.load_map("example.tmx")) {
        std::cerr << "Map couldn't load\n";
    }
    else {
        //Event handler
        SDL_Event e;

        //Main loop flag
        bool quit = false;

        //While application is running
        while( !quit ){
            //Handle events on queue
            while( SDL_PollEvent( &e ) != 0 ) {
                //User requests quit
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }
                					//User presses a key
                else if( e.type == SDL_KEYDOWN )
                {
                    //Select surfaces based on key press
                    switch( e.key.keysym.sym )
                    {
                        case SDLK_UP:
                        game.m_camera.y -= 50;
                        break;

                        case SDLK_w:
                        game.m_camera.y -= 1;
                        break;

                        case SDLK_DOWN:
                        game.m_camera.y += 50;
                        break;

                        case SDLK_s:
                        game.m_camera.y += 1;
                        break;

                        case SDLK_LEFT:
                        game.m_camera.x -= 50;
                        break;

                        case SDLK_a:
                        game.m_camera.x -= 1;
                        break;

                        case SDLK_RIGHT:
                        game.m_camera.x += 50;
                        break;

                        case SDLK_d:
                        game.m_camera.x += 1;
                        break;
                    }
                }
            }
            game.render();
        }
    }
    return 0;
}
