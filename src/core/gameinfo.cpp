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
#include "core/gameinfo.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

#include "util/tinyxml2.h"

/// Constructs a @c GameInfo Object
GameInfo::GameInfo(unsigned screen_w, unsigned screen_h)
: m_screen_w {screen_w}, m_screen_h {screen_h}, m_map {m_screen_w, m_screen_h}
{
    //Start up SDL and create window
	if( !init() ) {
		std::cerr << "Failed to initialize!\n";
	}

}

/**
 * @brief Initializes SDL2, its settings and renderer
 * @return @c bool which indicates success or failure
 */
bool GameInfo::init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
	{
		std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			std::cerr << "Warning: Linear texture filtering not enabled!\n";
		}

		//Create window
		m_window = SDL_CreateWindow( m_window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screen_w, m_screen_h, SDL_WINDOW_SHOWN );
		if( m_window == nullptr )
		{
			std::cerr << "Window could not be created! SDL Error: "<< SDL_GetError() << "\n";
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( m_renderer == nullptr )
			{
				std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( m_renderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					std::cerr << "SDL_image could not initialize! SDL_image Error: "<< IMG_GetError() << "\n" ;
					success = false;
				}

				//Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
			}
		}
	}
	return success;
}

/**
 * @brief Loads the supplied mapfile
 * @param mapfile Name of the .tmx map
 * @return @c bool which indicates success or failure
 * @warning The SDL2 renderer must be initialized prior loading!
 */
bool GameInfo::load_map(std::string mapfile) {
    // Empty event storage to avoid memory leaks
    ActorEvent::initialize_all();

    tinyxml2::XMLError eResult = m_map.init_map(mapfile, &m_renderer);
    if(eResult == tinyxml2::XML_SUCCESS) return true;
    else return false;
}

/**
 * @brief Updates the map
 * @return @c bool false if quit
 */

bool GameInfo::update() {

    //Event handler
    SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 ) {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            return false;
        }
        //User presses a key
        else if( e.type == SDL_KEYDOWN && (m_key_repeat == true || e.key.repeat == false)) {
            m_map.process_key_down(e);
        }
        else if( e.type == SDL_KEYUP && (m_key_repeat == true || e.key.repeat == false)) {
            m_map.process_key_up(e);
        }
    }

    m_map.process_keys_sustained();

    m_map.update();
    return true;
}

/**
 * @brief Draws the current map to screen
 */
void GameInfo::render() {
    m_map.render();
    SDL_RenderPresent(m_renderer);
}

/// Cleans up SDL2 stuff
void GameInfo::close() {

	//Destroy window
	SDL_DestroyRenderer( m_renderer );
	SDL_DestroyWindow( m_window );
	m_window = nullptr;
	m_renderer = nullptr;

	//Quit SDL subsystems
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

/// Makes sure everything gets cleaned up upon destruction
GameInfo::~GameInfo() {
    close();
}
