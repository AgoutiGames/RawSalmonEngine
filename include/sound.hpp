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
#ifndef SOUND_REF_HPP_INCLUDED
#define SOUND_REF_HPP_INCLUDED

namespace salmon {

namespace internal{class SoundEffect;}

class Sound {
    public:
        Sound();
        Sound(internal::SoundEffect& impl);

        Sound(const Sound& sound);
        Sound(Sound&& sound);
        Sound& operator=(const Sound& sound);
        Sound& operator=(Sound&& sound);
        ~Sound();

        /// Returns true if the sound exists and could be loaded
        bool good() const;

        /**
         * @brief Plays the loaded sound n-times for a maximum length
         * @param loops The sound will be played loops + 1 times. Supply -1 for infinite looping.
         * @param length_ms The sound will be played at most this amount of milliseconds
         * @note If both loops and length_ms is supplied, the sound will stop when at least one condition is reached
         */
        void play(int loops = 0, int length_ms = -1);

        /**
         * @brief Plays sound with optional fade in effect
         * @param fadein_ms The amount of milliseconds the fade in effect will take
         * @note See "play" method for the other parameters
         */
        void play_fadein(int fadein_ms, int loops = 0, int length_ms = -1);

        /// Pauses the play back of the sound
        void pause();
        /// Resumes the play back after pause
        void resume();
        /// Stops and resets play back.
        void halt();
        /// Stops and resets playback with a fade out effect of supplied length
        void halt_fadeout(int fadeout_ms);

        /**
         * @brief Sets "virtual" position relative to listener
         * @param angle The angle relative to listener in degrees. Zero means in front of listener.
         * @param distance The distance to the listener ranging from 0.0 to 1.0
         */
        void set_position(int angle, float distance = 0);
        /// Resets the position to default (In front of listener with no distance at all)
        void reset_position();

        /// Returns sound volume ranging from 0.0 to 1.0
        float get_volume();
        /// Sets the volume to the supplied factor clamped from 0.0 to 1.0
        void set_volume(float factor);
        /// Returns true if the sound is currently playing
        bool playing();
        /// Returns true if the sound is currently paused
        bool paused();
    private:
        internal::SoundEffect* m_impl;
};
}

#endif // SOUND_REF_HPP_INCLUDED
