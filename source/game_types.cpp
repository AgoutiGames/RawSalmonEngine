#include "game_types.hpp"

/// Converts a @c string to an @c enum of @c AnimationType
AnimationType str_to_anim_type(const std::string& name) {
    if(name == "IDLE") return AnimationType::idle;
    if(name == "WALK") return AnimationType::walk;
    else return AnimationType::invalid;
}

/// Converts a @c string to an @c enum of @c Direction
Direction str_to_direction(const std::string& name) {
    if(name == "UP") return Direction::up;
    if(name == "DOWN") return Direction::down;
    if(name == "LEFT") return Direction::left;
    if(name == "RIGHT") return Direction::right;
    else return Direction::invalid;
}

/// Converts a @p string to an @p enum of @c Behaviour
Behaviour str_to_behaviour(const std::string& name) {
    if(name == "IDLE") return Behaviour::idle;
    if(name == "WALK_AROUND") return Behaviour::walk_around;
    else return Behaviour::invalid;
}
