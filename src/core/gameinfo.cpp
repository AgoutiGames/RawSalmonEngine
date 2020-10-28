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

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

namespace salmon { namespace internal {

/// Constructs a @c GameInfo Object
GameInfo::GameInfo() : m_preloader{this}, m_input_cache{this} {
    //Start up SDL and create window
	if( !init() ) {
		Logger(Logger::error) << "Failed to initialize SDL!";
	}

    char* base_path = SDL_GetBasePath();
    if(base_path != nullptr) {m_base_path = base_path;}
    else {
        Logger(Logger::error) << "Couldn't get location of executable! Probably running on currently unsupported OS";
    }
    m_resource_path = m_base_path + m_resource_path;
    make_path_absolute(m_resource_path);
    m_resource_path = m_resource_path + "/";
    m_resource_path = fs::path(m_resource_path).make_preferred().string();
    m_current_path = m_resource_path;

    m_audio_manager.set_music_path(m_resource_path);
    m_audio_manager.set_sound_path(m_resource_path);
    m_font_manager.set_font_path(m_resource_path + "fonts/");
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
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0 )
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
		m_window.get_transform().set_dimensions(m_x_resolution,m_y_resolution);
		SDL_Window* window = m_window.init();
		m_window.set_title(m_window_title);

		if( window == nullptr )
		{
			Logger(Logger::error) << "Window could not be created! SDL Error: "<< SDL_GetError();
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			m_renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( m_renderer == nullptr )
			{
				Logger(Logger::error) << "Renderer could not be created! SDL Error: " << SDL_GetError();
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( m_renderer, 0xFF, 0xFF, 0xFF, 0xFF );

				#ifdef __EMSCRIPTEN__
				    int img_flags = IMG_INIT_PNG  | IMG_INIT_JPG;
			    #else
			        int img_flags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF;
			    #endif
				if( (IMG_Init(img_flags) & img_flags) != img_flags)
				{
					Logger(Logger::error) << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError();
					success = false;
				}

				m_texture_cache.init(m_renderer);

                #ifdef __EMSCRIPTEN__
				    int mix_flags = MIX_INIT_OGG;
				#else
				    int mix_flags = MIX_INIT_OGG | MIX_INIT_MP3 | MIX_INIT_FLAC;
				#endif
                if( (Mix_Init(mix_flags) & mix_flags) != mix_flags) {
                    Logger(Logger::error) << "Mix_Init: Failed to init required sound file support! SDL_mixer Error: " << Mix_GetError();
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

    // Strangely not needed at all
    // m_input_cache.init_controllers();

	return success;
}

bool GameInfo::set_linear_filtering(bool mode) {
    if(mode) {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            Logger(Logger::warning) << "Linear texture filtering not enabled!";
            return false;
        }
    }
    else {
        //Set texture filtering to nearest neighour
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) )
        {
            Logger(Logger::warning) << "Nearest neighbour filtering not enabled!";
            return false;
        }
    }
    return true;
}

/**
 * @brief Loads the supplied mapfile
 * @param mapfile Name of the .tmx map
 * @return @c bool which indicates success or failure
 * @warning The SDL2 renderer must be initialized prior loading!
 */
bool GameInfo::load_map(std::string mapfile, bool absolute) {
    m_maps.emplace_back(this);

    // Set camera dimensions to current internal resolution
    m_maps.back().get_camera().get_transform().set_dimensions(m_x_resolution,m_y_resolution);

    if(!absolute) {mapfile = m_current_path + mapfile;}
    Logger() << "Load map at: " << mapfile;
    tinyxml2::XMLError eResult = m_maps.back().init_map(mapfile, &m_renderer);
    if(eResult == tinyxml2::XML_SUCCESS) {
        Logger() << "Successfully loaded map";
        update_path();
        return true;
    }
    else {
        Logger(Logger::error) << "Failed loading map:" << mapfile;
        m_maps.pop_back();
        return false;
    }
}

/**
 * @brief Remove the currently active map from map stack
 */
void GameInfo::close_map() {
    Logger() << "Close map at: " << m_maps.back().get_full_path();
    m_maps.pop_back();
    if(!m_maps.empty()) {
        m_maps.back().resume();
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

    if(!poll_input_events()) {return false;}

    m_maps.back().update();

    return true;
}

bool GameInfo::poll_input_events() {
    //Event handler
    SDL_Event e;

    m_input_cache.clear();

    while( SDL_PollEvent( &e ) != 0 ) {

        switch(e.type) {
        //User requests quit
            case SDL_QUIT : {
                // Close game by aborting update
                return false;
            }
            //User presses a key
            case SDL_KEYDOWN : {
                if(m_key_repeat == true || e.key.repeat == false) {
                    m_input_cache.set(e.key.keysym.sym, true);
                }
                break;
            }
            case SDL_KEYUP : {
                if(m_key_repeat == true || e.key.repeat == false) {
                    m_input_cache.set(e.key.keysym.sym, false);
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN : {
                m_input_cache.set(e.button);
                break;
            }
            case SDL_MOUSEBUTTONUP : {
                m_input_cache.set(e.button);
                break;
            }
            case SDL_MOUSEWHEEL : {
                m_input_cache.set(e.wheel);
                break;
            }
            case SDL_MOUSEMOTION : {
                m_input_cache.set(e.motion);
                break;
            }
            case SDL_CONTROLLERAXISMOTION : {
                m_input_cache.set(e.caxis);
                break;
            } /**< Game controller axis motion */
            case SDL_CONTROLLERBUTTONDOWN : {
                m_input_cache.set(e.cbutton);
                break;
            }          /**< Game controller button pressed */
            case SDL_CONTROLLERBUTTONUP : {
                m_input_cache.set(e.cbutton);
                break;
            }            /**< Game controller button released */
            case SDL_CONTROLLERDEVICEADDED : {
                m_input_cache.set(e.cdevice);
                break;
            }         /**< A new Game controller has been inserted into the system */
            case SDL_CONTROLLERDEVICEREMOVED : {
                m_input_cache.set(e.cdevice);
                break;
            }       /**< An opened Game controller has been removed */
            case SDL_CONTROLLERDEVICEREMAPPED : {
                m_input_cache.set(e.cdevice);
                break;
            }
            case SDL_FINGERDOWN : {
                m_input_cache.set(e.tfinger);
                break;
            }
            case SDL_FINGERUP : {
                m_input_cache.set(e.tfinger);
                break;
            }
            case SDL_FINGERMOTION : {
                m_input_cache.set(e.tfinger);
                break;
            }
            case SDL_WINDOWEVENT : {
                switch(e.window.event) {
                    case SDL_WINDOWEVENT_CLOSE : {
                        // Close game by aborting update
                        return false;
                    }
                    case SDL_WINDOWEVENT_ENTER : {
                        m_input_cache.set(e.window);
                        break;
                    }
                    case SDL_WINDOWEVENT_LEAVE : {
                        m_input_cache.set(e.window);
                        break;
                    }
                    default : {break;}
                }
                break;
            }
            default : {break;}
        }
    }

    m_input_cache.poll();
    return true;
}

/**
 * @brief Draws the current map to screen
 */
void GameInfo::render() {
    // Apply possible transformations to the window
    m_window.update();

    if(!m_maps.empty() && !m_window.is_hidden()) {
        // Set internal resolution to current camera dimensions
        update_internal_resolution();
        // Render map and present image buffer
        m_maps.back().render();
        SDL_RenderPresent(m_renderer);
    }
}

bool GameInfo::update_internal_resolution() {
    // Apply possible camera dimension change to internal resolution
    PixelDimensions dim = m_maps.back().get_camera().get_transform().get_dimensions();
    if(dim.w != m_x_resolution || dim.h != m_y_resolution) {
        m_x_resolution = dim.w;
        m_y_resolution = dim.h;
        if(SDL_RenderSetLogicalSize(m_renderer,m_x_resolution,m_y_resolution)) {
            Logger(Logger::error) << "Failed to set game resolution to " << dim.w << "x" << dim.h <<" , SDL Error: " << SDL_GetError();
            return false;
        }
    }
    return true;
}

void GameInfo::update_path() {
    m_current_path = m_maps.back().get_file_path();
    // The path gets trimmed by map already
    // m_current_path.erase(m_current_path.find_last_of('/') + 1);
}


MapData& GameInfo::get_map() {return m_maps.back();}
std::vector<MapData>& GameInfo::get_maps() {return m_maps;}

/// Cleans up SDL2 stuff
void GameInfo::close() {

    Logger() << "Clean up and quit SDL and its subsystems";

	//Destroy window
	SDL_DestroyRenderer( m_renderer );
	m_renderer = nullptr;
	m_window.destroy();

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

}} // namespace salmon::internal
