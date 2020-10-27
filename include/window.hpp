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
#ifndef SALMON_WINDOW_HPP_INCLUDED
#define SALMON_WINDOW_HPP_INCLUDED

#include <string>

#include "transform.hpp"

struct SDL_Window;

namespace salmon {

// forward declare internal::GameInfo so it can be a friend
namespace internal {class GameInfo;}

class Window {

friend class internal::GameInfo;

public:
    /// Returns transform representation of window.
    /// All scale and move operations are applied to the window before rendering
    Transform& get_transform() {return m_transform;}

    /// Returns the dimensions of the screen where the window center resides in
    PixelRect get_screen_size() const;
    /// Returns screen dimensions minus the space occupied by taskbar etc.
    PixelRect get_usable_screen_size() const;
    /// Returns the name of the display where the window is located
    std::string get_display_name() const;

    /// Sets the title of the window to the given string
    void set_title(std::string title);
    /// Returns the current title of the window
    std::string get_title() const;

    /// Expands the window to a "fake" fullscreen which is more compatible than a "hardware" fullscreen which effectively changes the display mode
    bool set_fullscreen(bool mode);
    bool is_fullscreen() const;

    /// Allows/forbids to resize the window by dragging a corner
    void set_resizable(bool mode);
    bool is_resizable() const;

    /// Removes/adds the border around the window
    void set_borderless(bool mode);
    bool is_borderless() const;

    /// Hide window completely from view with no means for the user to restore by themselves
    void hide();
    bool is_hidden() const;

    /// Bring up window again from prior hide() call
    void show();
    bool is_shown() const;

    /// Minimize the window to the taskbar
    void minimize();
    bool is_minimized() const;

    /// Bring window to its maximum size
    /// @note The window has to be resizable for this to work
    void maximize();
    bool is_maximized() const;

    /// Restores window either from being minimized to taskbar or maximized to its "normal" dimensions and position
    /// @warning I tested restor after minimize and it didn't work on my machine though the SDL documentation is clear
    ///          about that part. Raise() worked for me in that case though.
    void restore();

    /// Raises window and gives input focus
    void raise();
    /// Try to set input focus, return true on success.
    /// @warning Calling raise() should do the same while moving the window in front of all other windows; use with caution
    bool set_active();
    /// Returns true if window has input focus
    bool is_active() const;

    /// Explicitly synchronizes transform with current position of window
    /// @note This should only be of use when continuous window movement has to take
    ///       window dragging into account. In that case call sync pos once before applying motion.
    void sync_pos();

private:
    /// Forbid API users the creation of a window
    Window() = default;
    /// Creates the SDL_Window. Returns nullptr if something went wrong
    SDL_Window* init();
    /// Apply position and scale if m_transform was changed from the outside
    void update();
    /// Destroys the SDL_Window
    void destroy();

    Transform m_transform;
    SDL_Window* m_window = nullptr;
};

} // namespace salmon

#endif // SALMON_WINDOW_HPP_INCLUDED
