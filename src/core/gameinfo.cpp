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
#include "core/gameinfo.hpp"

#include <iostream>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <tinyxml2.h>

#include "map/mapdata.hpp"

// Strangely we have to include this here
#include "actor/actor.hpp"
#include "map/tileset.hpp"
#include "map/layer.hpp"
#include "util/logger.hpp"

/// Constructs a @c GameInfo Object
GameInfo::GameInfo(unsigned screen_w, unsigned screen_h)
: m_screen_w {screen_w}, m_screen_h {screen_h}
{
    //Start up SDL and create window
	if( !init() ) {
		Logger(Logger::error) << "Failed to initialize SDL!";
	}
}

/**
 * @brief Initializes SDL2, its settings and renderer
 * @return @c bool which indicates success or failure
 */
bool GameInfo::init() {
	//Initialization flag
	bool success = true;

	Logger() << "Initialize SDL and its subsystems";

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		Logger(Logger::error) << "SDL could not initialize! SDL Error: " << SDL_GetError();
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			Logger(Logger::warning) << "Linear texture filtering not enabled!";
		}

		//Create window
		m_window = SDL_CreateWindow( m_window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screen_w, m_screen_h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
		if( m_window == nullptr )
		{
			Logger(Logger::error) << "Window could not be created! SDL Error: "<< SDL_GetError();
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( m_renderer == nullptr )
			{
				Logger(Logger::error) << "Renderer could not be created! SDL Error: " << SDL_GetError();
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( m_renderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					Logger(Logger::error) << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError();
					success = false;
				}

				//Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    Logger(Logger::error) << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError();
                    success = false;
                }
			}
		}
	}

	 //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        Logger(Logger::error) << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError();
        success = false;
    }

	return success;
}

/**
 * @brief Loads the supplied mapfile
 * @param mapfile Name of the .tmx map
 * @return @c bool which indicates success or failure
 * @warning The SDL2 renderer must be initialized prior loading!
 */
bool GameInfo::load_map(std::string mapfile, bool absolute) {
    m_maps.emplace(this, m_screen_w, m_screen_h);
    if(!absolute) {mapfile = m_current_path + mapfile;}
    Logger() << "Load map at: " << mapfile;
    tinyxml2::XMLError eResult = m_maps.top().init_map(mapfile, &m_renderer);
    if(eResult == tinyxml2::XML_SUCCESS) {
        Logger() << "Successfully loaded map";
        update_path();
        return true;
    }
    else {
        Logger(Logger::error) << "Failed loading map:" << mapfile;
        m_maps.pop();
        return false;
    }
}

/**
 * @brief Remove the currently active map from map stack
 */
void GameInfo::close_map() {
    Logger() << "Close map at: " << m_maps.top().get_full_path();
    m_maps.pop();
    if(!m_maps.empty()) {
        m_maps.top().resume();
        update_path();
    }
}

/**
 * @brief Updates the map
 * @return @c bool which is false if game ended
 */
bool GameInfo::update() {

    if(m_maps.empty()) {
        Logger(Logger::fatal) << "No active map left on stack to update and render! Aborting!";
        return false;
    }

    //Event handler
    SDL_Event e;
    #ifndef LIB_BUILD
    InputHandler& handler = m_maps.top().get_input_handler();
    #endif // LIB_BUILD

    m_input_cache.clear();

    while( SDL_PollEvent( &e ) != 0 ) {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            return false;
        }
        //User presses a key
        else if( e.type == SDL_KEYDOWN && (m_key_repeat == true || e.key.repeat == false)) {
            #ifndef LIB_BUILD
            handler.process_key_down(e.key);
            #endif // LIB_BUILD
            m_input_cache.set(e.key.keysym.sym, true);
        }
        else if( e.type == SDL_KEYUP && (m_key_repeat == true || e.key.repeat == false)) {
            #ifndef LIB_BUILD
            handler.process_key_up(e.key);
            #endif // LIB_BUILD
            m_input_cache.set(e.key.keysym.sym, false);
        }
        else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
            m_input_cache.set(e.button);
        }
        else if(e.type == SDL_MOUSEWHEEL) {
            m_input_cache.set(e.wheel);
        }
        else if(e.type == SDL_MOUSEMOTION) {
            m_input_cache.set(e.motion);
        }
    }

    #ifndef LIB_BUILD
    // In addition to pressed and released events also check for down state of button
    handler.process_keys_sustained();
    #endif // LIB_BUILD

    m_input_cache.poll();

    m_maps.top().update();

    #ifndef LIB_BUILD
    // Do nothing with returned signal because we don't have to
    m_events.process_events(*this);
    #endif // LIB_BUILD

    return true;
}

/**
 * @brief Draws the current map to screen
 */
void GameInfo::render() {
    if(!m_maps.empty()) {
        m_maps.top().render();
        SDL_RenderPresent(m_renderer);
    }
}

void GameInfo::update_path() {
    m_current_path = m_maps.top().get_file_path();
    m_current_path.erase(m_current_path.find_last_of('/') + 1);
}


MapData& GameInfo::get_map() {return m_maps.top();}
std::stack<MapData>& GameInfo::get_maps() {return m_maps;}

/// Cleans up SDL2 stuff
void GameInfo::close() {

    Logger() << "Clean up and quit SDL and its subsystems";

	//Destroy window
	SDL_DestroyRenderer( m_renderer );
	SDL_DestroyWindow( m_window );
	m_window = nullptr;
	m_renderer = nullptr;

	// Clear all currently open fonts
	m_font_manager.clear();

	//Quit SDL subsystems
	Mix_Quit();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

/// Cleans up all resources
GameInfo::~GameInfo() {
    close();
}
