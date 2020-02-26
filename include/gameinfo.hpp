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
#ifndef GAMEINFO_LIB_HPP_INCLUDED
#define GAMEINFO_LIB_HPP_INCLUDED

#include <string>
#include <memory>

#include "./audio_manager_ref.hpp"
#include "./data_block_ref.hpp"
#include "./input_cache_ref.hpp"
#include "./map_ref.hpp"

class GameInfo;

namespace salmon {
    class GameInfo{
        public:
            GameInfo();
            ~GameInfo();

            /**
             * @brief Initialize video, audio and controllers
             * @return True if the initialization succeeded
             * @note This has to be called once at startup before loading any map.
             */
            bool init();
            /// Deinitalize video, audio, etc. Do this before returning from main function.
            void close();

            /// Render the game to screen
            void render();
            /**
             * @brief Update the game state, usually done once before each render call
             * @return False if a critical error occured and game has to shut down
             * @note updates trigger polling of gamepads, keypresses, static animation and render order sorting
             */
            bool update();

            /**
             * @brief Load next map preserving the current map
             * @param mapfile The path to the map to be loaded
             * @param absolute If false the path is relative to the directory of the current map, otherwise it's absolute
             * @return True if loading succeeded, false if it failed
             */
            bool load_map(std::string mapfile, bool absolute = false);

            /// Close the current map, making the map before the new current one
            void close_map();
            /// Return reference object to the currently active map
            salmon::MapRef get_map();

            /// Sets the size of the window of this game to the supplied dimensions in pixels
            void set_window_size(unsigned width, unsigned height);
            /// Sets game when mode is true to borderless fullscreen or else to windowed mode
            bool set_fullscreen(bool mode);
            /// Set the internal resolution of the game in pixels, independent of window resolution
            bool set_game_resolution(unsigned width, unsigned height);
            /// Set on linear filtering for smooth upscaled textures or off for proper sharp pixel art
            bool set_linear_filtering(bool mode);
            /// Locks or unlocks the windows ability to get resized by user (Doesn't work reliably on all platforms)
            void set_window_resizable(bool mode);

            /// Return true if the game is currently minimized
            bool window_minimized() const;
            /// Return true if the game is the currently active windows for the OS
            bool window_active() const;

            /// Return the current internal x resolution of the game in pixels
            unsigned get_game_x_resolution() const;
            /// Return the current internal y resolution of the game in pixels
            unsigned get_game_y_resolution() const;

            /// Return the current width of the games window in pixels
            unsigned get_window_x_resolution() const;
            /// Return the current width of the games window in pixels
            unsigned get_window_y_resolution() const;

            /// Return the horizontal resolution in pixels at which your screen is currently working
            unsigned get_screen_x_resolution() const;
            /// Return the vertical resolution in pixels at which your screen is currently working
            unsigned get_screen_y_resolution() const;

            /// Adds directory for preloading. Path is relative to the data folder
            void add_preload_directory(std::string dir);
            /**
             * @brief Preloads assets from preload directories
             * @param The maxmimum amount of time after preloading returns
             * @return True if preloading is complete or false if some assets still arent loaded
             * @note successive calls to this method start where they previously left off loading
             */
            bool preload(float seconds);

            /// Return reference to the audio manager object of the game
            AudioManagerRef get_audio_manager();
            /// Returns reference to DataBlock of this game which may hold property values added during execution
            DataBlockRef get_data();
            /// Returns reference to input cache, holding information about keypresses, mouse state and gamepad state
            InputCacheRef get_input_cache();

        private:
            std::unique_ptr<::GameInfo> m_impl;
    };
}


#endif // GAMEINFO_LIB_HPP_INCLUDED
