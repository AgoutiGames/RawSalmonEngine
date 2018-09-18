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
#include "audio/sound_effect.hpp"

#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

SoundEffect::SoundEffect(std::string path) :
m_sound(Mix_LoadWAV(path.c_str()), SoundEffect::Deleter())
{
    if(m_sound == nullptr) {
        std::cerr << "Failed to load sound effect at: " << path << " ! SDL_mixer Error: " << Mix_GetError() << "\n";
    }
}

/**
 * @brief Plays the sound once
 */
void SoundEffect::play() const {
    Mix_PlayChannel(-1, m_sound.get(), 0);
}
