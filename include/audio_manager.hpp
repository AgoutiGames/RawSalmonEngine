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
#ifndef AUDIO_MANAGER_REF_HPP_INCLUDED
#define AUDIO_MANAGER_REF_HPP_INCLUDED

#include <string>

#include "music.hpp"
#include "sound.hpp"

namespace salmon {

namespace internal{class AudioManager;}

class AudioManager {
    public:
        AudioManager(internal::AudioManager& impl);

        /**
         * @brief Load music file from supplied path
         * @param path The path to the music file
         * @param absolute If false the path is relative to data folder, otherwise it's absolute
         * @return True if file could be found and is a supported file
         * @note Supported filetypes are .ogg .flac .mp3 and .wav
         */
        bool load_music(std::string path, bool absolute = false);
        /**
         * @brief Load sound file from supplied path
         * @param path The path to the sound file
         * @param absolute If false the path is relative to data folder, otherwise it's absolute
         * @return True if file could be found and is a supported file
         * @note Supported filetypes are .ogg .flac .mp3 and .wav
         */
        bool load_sound(std::string path, bool absolute = false);

        /**
         * @brief Returns music file from supplied path
         * @param path The path to the music file
         * @param absolute If false the path is relative to data folder, otherwise it's absolute
         * @return Reference to the music object
         * @note Supported filetypes are .ogg .flac .mp3 and .wav
         * @note Only loads the file from disk if it wasn't loaded before
         *
         * @warning Always test if music could load by checking if good() returns true
         */
        Music get_music(std::string path, bool absolute = false);
        /**
         * @brief Returns sound file from supplied path
         * @param path The path to the sound file
         * @param absolute If false the path is relative to data folder, otherwise it's absolute
         * @return Reference to the sound object
         * @note Supported filetypes are .ogg .flac .mp3 and .wav
         * @note Only loads the file from disk if it wasn't loaded before
         *
         * @warning Always test if sound could load by checking if good() returns true
         */
        Sound get_sound(std::string path, bool absolute = false);

        /// Set the global volume of sound effects to a value between 0.0 and 1.0. Doesn't apply to music!
        void set_master_sound_volume(float factor);
        /// Get the global sound volume which is between 0.0 and 1.0
        float get_master_sound_volume();

        /// Clear all priorily loaded music from RAM
        void clear_music();
        /// Clear all priorily loaded sound effects from RAM
        void clear_sounds();
        /// Clear both music and sound effects
        void clear();
    private:
        internal::AudioManager* m_impl;
};
}

#endif // AUDIO_MANAGER_REF_HPP_INCLUDED
