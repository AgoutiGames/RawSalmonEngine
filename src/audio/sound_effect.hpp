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
#ifndef SOUND_EFFECT_HPP_INCLUDED
#define SOUND_EFFECT_HPP_INCLUDED

#include <iostream>
#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

/**
 * @brief Load and play sounds
 */
class SoundEffect {

    public:
        SoundEffect() {}
        SoundEffect(std::string path);
        bool good() const {return (m_sound.get() == nullptr) ? false : true ;}
        void play() const;

        struct Deleter {
            void operator()(Mix_Chunk* p) {
                if(p != nullptr) {Mix_FreeChunk(p);}
            }
        };

    private:
        std::shared_ptr<Mix_Chunk> m_sound;
};


#endif // SOUND_EFFECT_HPP_INCLUDED
