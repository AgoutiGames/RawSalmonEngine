#ifndef GAME_TYPES_HPP_INCLUDED
#define GAME_TYPES_HPP_INCLUDED
#include <string>
#include <vector>
#include <map>
#include "tile.hpp"
#include "SDL2/SDL.h"

/**
 * @brief A collection of various enums
 */

enum class Behaviour {
    idle,
    walk_around,
    invalid,
};

enum class Direction {
    up,
    down,
    left,
    right,
    invalid,
};

enum class AnimationType {
    idle,
    walk,
    invalid,
};

struct ActorTemplate {
    std::string template_name = "_";
    float speed = 250.0f;
    Behaviour AI = Behaviour::idle;
    Direction direction = Direction::down;
    SDL_Rect hitbox = {0,0,0,0};
    std::map<AnimationType, std::map<Direction, Tile>> animations;
};

AnimationType str_to_anim_type(const std::string& name);
Direction str_to_direction(const std::string& name);
Behaviour str_to_behaviour(const std::string& name);



#endif // GAME_TYPES_HPP_INCLUDED
