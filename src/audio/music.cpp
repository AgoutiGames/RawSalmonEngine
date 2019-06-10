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
#include "audio/music.hpp"

Music::Music(std::string path) :
m_sound(Mix_LoadMUS(path.c_str()), Music::Deleter())
{
    if(m_sound == nullptr) {
        std::cerr << "Failed to load music at: " << path << " ! SDL_mixer Error: " << Mix_GetError() << "\n";
    }
}

/**
 * @brief Plays the music indefinitely with a possible fade in
 */
void Music::play(int repetitions, int fade_in) const {
    Mix_FadeInMusic(m_sound.get(), repetitions, fade_in);
}
