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
#ifndef SALMON_TYPES_HPP_INCLUDED
#define SALMON_TYPES_HPP_INCLUDED

#include <string>

namespace salmon {

struct Point {
    Point move(Point p);
    float x, y;
};

struct PixelPoint {
    PixelPoint(int ix, int iy) : x{ix}, y{iy} {}
    // You can downcast from real float coords to pixel coords but not back again
    PixelPoint(Point p);
    int x, y;
};

struct Scale {
    float w, h;
};

struct Dimensions {
    Dimensions scale(Scale s);
    float w, h;
};

struct PixelDimensions {
    PixelDimensions(int iw, int ih) : w{iw}, h{ih} {}
    // You can downcast from real float coords to pixel coords but not back again
    PixelDimensions(Dimensions d);
    int w, h;
};

struct Rect {
    Rect(float ix, float iy, float iw, float ih) : x{ix}, y{iy}, w{iw}, h{ih} {}
    Rect(Point p, Dimensions d) : x{p.x}, y{p.y}, w{d.w}, h{d.h} {}
    Rect scale(Scale s);
    Rect move(Point r);
    bool empty() const {return (w <= 0 || h <= 0) ? true : false ;}

    float x, y;
    float w, h;
};

/// A rectangle with its origin in the upper left corner rounded to integer pixel coordinates
struct PixelRect {
    PixelRect(int ix, int iy, int iw, int ih) : x{ix}, y{iy}, w{iw}, h{ih} {}
    // You can downcast from real float coords to pixel coords but not back again
    PixelRect(Rect r);
    PixelRect(PixelPoint p, PixelDimensions d) : x{p.x}, y{p.y}, w{d.w}, h{d.h} {}
    bool empty() const {return (w <= 0 || h <= 0) ? true : false ;}

    int x, y;
    int w, h;
};

#ifdef __EMSCRIPTEN__
    constexpr bool WEB_BUILD = true;
#else
    constexpr bool WEB_BUILD = false;
#endif // __EMSCRIPTEN__

/// The name of a hitbox if you don't specify an actual
const std::string DEFAULT_HITBOX = "DEFAULT";

/// Determine with what kind of objects you can collide
enum class Collidees {
    tile = 1,
    actor = 2,
    tile_and_actor = 3,
};

/// Useful enum used in many different contexts
enum class Direction {
    up = 0,
    up_right = 45,
    right = 90,
    down_right = 135,
    down = 180,
    down_left = 225,
    left = 270,
    up_left = 315,
    current = -2, ///< Keyword for last used direction
    none = -1,
    invalid = -3,
};

/// String constants which may be used as animation state keywords
namespace AnimationType {
    // Reserved Keywords for special behaviour
    const std::string none = "NONE";
    const std::string current = "CURRENT";
    const std::string invalid = "INVALID";

    // Generic AnimationTypes for not having to deal with magic values
    const std::string idle = "IDLE";
    const std::string walk = "WALK";
    const std::string run = "RUN";
    const std::string jump = "JUMP";
    const std::string fall = "FALL";
    const std::string attack = "ATTACK";
    const std::string action = "ACTION";
    const std::string die = "DIE";
}

/// Return value for animation methods
enum class AnimSignal{
    missing = -1, ///< Animation tile is missing
    none = 0, ///< Success but still on same frame
    next = 1, ///< Advanced one animation frame
    trigger = 2, ///< Just reached the predefined trigger frame
    wrap = 3, ///< Just reached first frame again after finishing the last frame
};

/// Show the state of a button
struct ButtonState {
    bool pressed = false; ///< True if up in frame before and now down
    bool released = false; ///< True if down in frame before and now up
    bool down = false; ///< True if button is down right now
};

/// Bundle mouse buttons and values in one struct
struct MouseState {
    ButtonState left; ///< Left mouse button
    ButtonState right; ///< Right mouse button
    ButtonState middle; ///< Mousewheel click or dedicated midddle button
    ButtonState extra1; ///< First extra mouse button
    ButtonState extra2; ///< Second extra mouse button
    int x_scroll = 0; ///< Amount of vertical scroll since last frame
    int y_scroll = 0; ///< Amount of horizontal scroll since last frame
    int x_pos = 0; ///< Current x-coord of mouse pointer relative to upper left corner of window
    int y_pos = 0; ///< Current y-coord of mouse pointer relative to upper left corner of window
    int x_delta = 0; ///< Amount of x-movement since last frame
    int y_delta = 0; ///< Amount of y-movement since last frame
    bool mouse_focus = false; ///< True if mouse is inside window frame
};

/// Collection of buttons and axis for a controller
/// Button layout closely mimics xbox 360 controller
///   l-tr         r-tr
///   l-sh         r-sh
///    _____        _____
///   /     |______/     |
///  /                    |
/// /   ^   bk |g| st   Y  |
/// | <lst>           X   B |
/// |   v    ^      ^   A   |
/// |      <dpd>  <rst>     |
/// |     ___v______v___    |
/// |    /              |   |
///  |__/                |__/
struct GamepadState {
    struct {
    ButtonState a;
    ButtonState b;
    ButtonState x;
    ButtonState y;
    ButtonState back;
    ButtonState guide; /// Many gamepads don't have this one
    ButtonState start;
    ButtonState left_stick;
    ButtonState right_stick;
    ButtonState left_shoulder;
    ButtonState right_shoulder;
    ButtonState up;
    ButtonState down;
    ButtonState left;
    ButtonState right;
    } button;

    struct {
    float left_x = 0; /// Range from -1 to 1
    float left_y = 0; /// Range from -1 to 1
    float right_x = 0; /// Range from -1 to 1
    float right_y = 0; /// Range from -1 to 1
    float trigger_left = 0; /// Range from 0 to 1
    float trigger_right = 0; /// Range from 0 to 1
    } axis;
};
}

#endif // SALMON_TYPES_HPP_INCLUDED
