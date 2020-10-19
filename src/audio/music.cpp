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
#include "audio/music.hpp"
#include "util/logger.hpp"

using namespace salmon::internal;

Music::Music(std::string path) :
m_sound(Mix_LoadMUS(path.c_str()), Music::Deleter())
{
    if(m_sound == nullptr) {
        Logger(Logger::error) << "Failed to load music at: " << path << " ! SDL_mixer Error: " << Mix_GetError();
    }
}

Music* Music::get_null_music() {
    static Music empty_music;
    return &empty_music;
}

void Music::play(int loops) {
    Mix_PlayMusic(m_sound.get(), loops);
}
void Music::play_fadein(int fadein_ms, int loops) {
    Mix_FadeInMusic(m_sound.get(), loops, fadein_ms);
}
void Music::play_fadein_pos(double seconds, int fadein_ms, int loops) {
    Mix_FadeInMusicPos(m_sound.get(), loops, fadein_ms, seconds);
}

void Music::pause() {
    Mix_PauseMusic();
}
void Music::resume() {
    Mix_ResumeMusic();
}
void Music::halt() {
    Mix_HaltMusic();
}
void Music::halt_fadeout(int fadeout_ms) {
    Mix_FadeOutMusic(fadeout_ms);
}
void Music::rewind() {
    Mix_RewindMusic();
}
void Music::set_position(double seconds) {
    if(Mix_GetMusicType(m_sound.get()) == MUS_MP3) {rewind();}
    Mix_SetMusicPosition(seconds);
}

float Music::get_volume() {
    return static_cast<float>(Mix_VolumeMusic(-1)) / MIX_MAX_VOLUME;
}
void Music::set_volume(float factor) {
    if(factor > 1.0f) {factor = 1.0f;}
    else if(factor < 0.0f) {factor = 0.0f;}
    Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * factor));
}
bool Music::playing() {
    return Mix_PlayingMusic();
}
bool Music::paused() {
    return Mix_PausedMusic();
}

