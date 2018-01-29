#ifndef TILE_HPP_INCLUDED
#define TILE_HPP_INCLUDED
#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include "tinyxml2.h"

// forward-declare the parts you need in one of the files and leave the #include out of that file.

class Tileset; // forward declaration

/**
 * @brief Parse, store and manage all tiles
 */

class Tile{
public:
    Tile(bool append = false);
    Tile(Tileset* ts, SDL_Rect& clp); // The initializing constructor
    Tile(const Tile& other);
    ~Tile();

    Tile& operator=(const Tile& other);

    void render(int x, int y) const;
    void render(SDL_Rect& dest) const; // Resizable render
    const SDL_Rect& get_clip() const;

    tinyxml2::XMLError parse_tile(tinyxml2::XMLElement* source, unsigned first_gid);
    tinyxml2::XMLError parse_actor_anim(tinyxml2::XMLElement* source, unsigned first_gid);
    tinyxml2::XMLError parse_actor_templ(tinyxml2::XMLElement* source);

    static void initialize(SDL_Renderer** renderer, unsigned base_tile_w, unsigned base_tile_h);

    static tinyxml2::XMLError parse_from_tileset(tinyxml2::XMLElement* source, unsigned first_gid);

    static void init_anim_tiles();
    void init_anim();

    static void push_all_anim();
    bool push_anim();

    static SDL_Renderer* get_renderer();
    static bool render(Uint16 tile_id, int x, int y);
    static bool render(Uint16 tile_id, SDL_Rect& dest);
    static Tile* get_tile(Uint16 tile_id);

    // I needed this for finding a nasty bug
    static void diagnose();

private:
    Uint16 m_gid = 0;
    Tileset* mp_tileset = nullptr;
    SDL_Rect m_clip;
    SDL_Rect m_hitbox;
    float m_speed = 1.0f;
    bool m_animated = false;

    // Variables required for animated tiles
    Uint16 m_current_id = 0;
    std::vector<Uint16> m_anim_ids;
    std::vector<unsigned> m_durations;
    Uint32 m_anim_timestamp = 0;

    static std::vector<Tile*> mp_tiles;      ///< List of pointers to all tiles in order
    static std::vector<Uint16> m_anim_tiles; ///< List of ids of all animated tiles
    static unsigned m_base_tile_w;
    static unsigned m_base_tile_h;
    static SDL_Renderer** mpp_renderer;
    static Tile empty_tile; ///< A placeholder which currently has no use...
};



#endif // TILE_HPP_INCLUDED
