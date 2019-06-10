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
#ifndef MUSIC_HPP_INCLUDED
#define MUSIC_HPP_INCLUDED

#include <iostream>
#include <string>
#include <memory>
#include <SDL2/SDL_mixer.h>

/**
 * @brief Load and play sounds
 */
class Music {

    public:
        Music() = default;
        Music(std::string path);
        bool good() const {return (m_sound.get() == nullptr) ? false : true ;}
        void play(int repetitions = -1, int fade_in = 0) const;

        struct Deleter {
            void operator()(Mix_Music* p) {
                if(p != nullptr) {Mix_FreeMusic(p);}
            }
        };

    private:
        std::shared_ptr<Mix_Music> m_sound;
};

#endif // MUSIC_HPP_INCLUDED