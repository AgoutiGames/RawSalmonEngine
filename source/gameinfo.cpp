#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "gameinfo.hpp"
#include "tinyxml2.h"

/// Constructs a @c GameInfo Object
GameInfo::GameInfo(unsigned screen_w, unsigned screen_h)
: m_screen_w {screen_w}, m_screen_h {screen_h}
{
    //Start up SDL and create window
	if( !init() ) {
		std::cerr << "Failed to initialize!\n";
	}
	//Put camera at default position with default size
	m_camera.x = 0;
	m_camera.y = 0;
	m_camera.w = m_screen_w;
	m_camera.h = m_screen_h;
}

/**
 * @brief Initializes SDL2, its settings and renderer
 * @return @c bool which indicates success or failure
 */
bool GameInfo::init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
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
    tinyxml2::XMLError eResult = m_map.init_map(mapfile, &m_renderer);
    if(eResult == tinyxml2::XML_SUCCESS) return true;
    else return false;
}

/**
 * @brief Draws the current map in correlation to the camera to screen
 */
void GameInfo::render() {
    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF ); // White Background
    SDL_RenderClear(m_renderer);
    m_map.update();
    m_map.render(&m_camera);
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
	IMG_Quit();
	SDL_Quit();
}

/// Makes sure everything gets cleaned up upon destruction
GameInfo::~GameInfo() {
    close();
}
