#ifndef GAMEINFO_HPP_INCLUDED
#define GAMEINFO_HPP_INCLUDED
#include <SDL2/SDL.h>
#include <string>
#include "mapdata.hpp"

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

    bool load_map(std::string mapfile);

    SDL_Rect m_camera;

private:
    SDL_Window* m_window = nullptr;
    std::string m_window_title = "Buya Game";
    SDL_Renderer* m_renderer = nullptr;
    unsigned m_screen_w;
    unsigned m_screen_h;

    MapData m_map; ///< Stores the currently active game map
};

#endif // GAMEINFO_HPP_INCLUDED
