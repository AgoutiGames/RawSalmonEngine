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
#ifndef MUSIC_REF_HPP_INCLUDED
#define MUSIC_REF_HPP_INCLUDED

namespace salmon {

namespace internal{class Music;}

class Music {
    public:
        Music();
        Music(internal::Music& impl);

        /// Returns true if the sound exists and could be loaded
        bool good() const;

        /**
         * @brief Plays the loaded music n-times
         * @param loops The sound will be played loops + 1 times. Supply -1 for infinite looping.
         */
        void play(int loops = -1);

        /**
         * @brief Plays music n-times with optional fade in effect
         * @param loops The sound will be played loops + 1 times. Supply -1 for infinite looping.
         * @param fadein_ms The amount of milliseconds the fade in effect will take
         */
        void play_fadein(int fadein_ms, int loops = -1);

        /**
         * @brief Plays music starting at a determined point looping n-times with optional fade in effect
         * @param seconds The amount of seconds to the starting position
         * @param loops The sound will be played loops + 1 times. Supply -1 for infinite looping.
         * @param fadein_ms The amount of milliseconds the fade in effect will take
         */
        void play_fadein_pos(double seconds, int fadein_ms, int loops = -1);

        /// Pauses the play back of the music
        void pause();
        /// Resumes the play back of the music after pause
        void resume();
        /// Stops and resets play back.
        void halt();
        /// Stops and resets play back with a fadeout effect
        void halt_fadeout(int fadeout_ms);
        /// Scrolls music back to starting position
        void rewind();

        /**
         * @brief Scrolls the music to a certain position
         * @param seconds The amount of seconds from start to the position to be scrolled
         * @warning This works only for .ogg and .flac files! If used on .mp3 it calls rewind()
         */
        void set_position(double seconds);

        /// Returns music volume ranging from 0.0 to 1.0
        float get_volume();
        /// Sets the volume to the supplied factor clamped from 0.0 to 1.0
        void set_volume(float factor);
        /// Returns true if the music is currently playing
        bool playing();
        /// Returns true if the music is currently paused
        bool paused();
    private:
        internal::Music* m_impl;
};
}

#endif // MUSIC_REF_HPP_INCLUDED
