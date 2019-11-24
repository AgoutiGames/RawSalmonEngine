/*
 * Copyright 2017-2019 Agouti Games Team (see the AUTHORS file)
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
#ifndef SOUND_EFFECT_HPP_INCLUDED
#define SOUND_EFFECT_HPP_INCLUDED

#include <iostream>
#include <string>
#include <memory>
#include <SDL_mixer.h>

/**
 * @brief Load and play sounds
 */
class SoundEffect {

    public:
        SoundEffect() = default;
        SoundEffect(std::string path);
        bool good() const {return (m_sound.get() == nullptr) ? false : true ;}
        void play(int loops = 0, int length_ms = -1);
        void play_fadein(int fadein_ms, int loops = 0, int length_ms = -1);
        void pause();
        void resume();
        void halt();
        void halt_fadeout(int fadeout_ms);

        void set_position(int angle, float distance = 0);
        void reset_position() {m_angle = 0; m_distance = 0;}

        float get_volume();
        void set_volume(float factor);
        bool playing();
        bool paused();

        static float get_global_volume();
        static void set_global_volume(float factor);

        struct Deleter {
            void operator()(Mix_Chunk* p) {
                if(p != nullptr) {Mix_FreeChunk(p);}
            }
        };

        static SoundEffect* get_null_sound();

    private:
        bool active();

        void set_position();

        static const int MAX_DISTANCE = 255;
        static constexpr bool POSITIONAL_AUDIO = true;

        std::shared_ptr<Mix_Chunk> m_sound;

        int m_current_channel = -1;
        Uint8 m_distance = 0;
        Sint16 m_angle = 0;
};


#endif // SOUND_EFFECT_HPP_INCLUDED
