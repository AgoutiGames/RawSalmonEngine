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
#ifndef MUSIC_HPP_INCLUDED
#define MUSIC_HPP_INCLUDED

#include <iostream>
#include <string>
#include <memory>
#include <SDL_mixer.h>

namespace salmon { namespace internal {

/**
 * @brief Load and play sounds
 */
class Music {

    public:
        Music() = default;
        Music(std::string path);
        bool good() const {return (m_sound.get() == nullptr) ? false : true ;}
        void play(int loops = -1);
        void play_fadein(int fadein_ms, int loops = -1);
        void play_fadein_pos(double seconds, int fadein_ms, int loops = -1);
        void pause();
        void resume();
        void halt();
        void halt_fadeout(int fadeout_ms);
        void rewind();
        void set_position(double seconds);

        float get_volume();
        void set_volume(float factor);
        bool playing();
        bool paused();

        struct Deleter {
            void operator()(Mix_Music* p) {
                if(p != nullptr) {Mix_FreeMusic(p);}
            }
        };

        static Music* get_null_music();

    private:
        std::shared_ptr<Mix_Music> m_sound;
};
}} // namespace salmon::internal

#endif // MUSIC_HPP_INCLUDED
