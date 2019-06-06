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
#include "game_types.hpp"

#include <iostream>

/// Converts a @c string to an @c enum of @c AnimationType
AnimationType str_to_anim_type(const std::string& name) {
    if(name == "IDLE") return AnimationType::idle;
    if(name == "WALK") return AnimationType::walk;
    if(name == "RUN") return AnimationType::run;
    if(name == "JUMP") return AnimationType::jump;
    if(name == "FALL") return AnimationType::fall;
    if(name == "CURRENT") return AnimationType::current;
    if(name == "NONE") return AnimationType::none;
    else return AnimationType::invalid;
}

/// Converts a @c string to an @c enum of @c Direction
Direction str_to_direction(const std::string& name) {
    if(name == "UP") return Direction::up;
    if(name == "DOWN") return Direction::down;
    if(name == "LEFT") return Direction::left;
    if(name == "RIGHT") return Direction::right;
    if(name == "CURRENT") return Direction::current;
    else return Direction::invalid;
}

/// Converts a @c string to an @c enum of @c Priority
Priority str_to_priority(const std::string& name) {
    if(name == "LOW") return Priority::low;
    if(name == "MEDIUM") return Priority::medium;
    if(name == "HIGH") return Priority::high;
    else return Priority::invalid;
}

/// Converts a @c string to an @c enum of @c EventSignal
EventSignal str_to_event_signal(const std::string& name) {
    if(name == "NEXT") return EventSignal::next;
    if(name == "STOP") return EventSignal::stop;
    if(name == "END") return EventSignal::end;
    if(name == "ABORT") return EventSignal::abort;
    if(name == "ERASE") return EventSignal::erase;
    else return EventSignal::invalid;
}

/// Converts a @c string to an @c enum of @c Response
Response str_to_response(const std::string& name) {
    if(name == "ON_HIT") return Response::on_hit;
    if(name == "ON_COLLISION") return Response::on_collision;
    if(name == "ON_ACTIVATION") return Response::on_activation;
    if(name == "ON_DEATH") return Response::on_death;
    if(name == "ON_IDLE") return Response::on_idle;
    if(name == "ON_ALWAYS") return Response::on_always;
    if(name == "ON_SPAWN") return Response::on_spawn;
    else return Response::invalid;
}

/// Converts a @c Direction to x and y factors
std::vector<float> dir_to_mov(const Direction dir) {
    if(dir == Direction::up)    return std::vector<float>{0,-1};
    if(dir == Direction::right) return std::vector<float>{1,0};
    if(dir == Direction::down)  return std::vector<float>{0,1};
    if(dir == Direction::left)  return std::vector<float>{-1,0};
    else return std::vector<float>{0,0};
}
