#include <iostream>
#include "gameinfo.hpp"

using namespace std;

/// The game Loop!
int main()
{
    GameInfo game(1024, 768);
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
