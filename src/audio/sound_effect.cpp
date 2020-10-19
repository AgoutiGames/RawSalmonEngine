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
#include "audio/sound_effect.hpp"
#include "util/logger.hpp"

using namespace salmon::internal;

SoundEffect::SoundEffect(std::string path) :
m_sound(Mix_LoadWAV(path.c_str()), SoundEffect::Deleter())
{
    if(m_sound == nullptr) {
        Logger(Logger::error) << "Failed to load sound effect at: " << path << " ! SDL_mixer Error: " << Mix_GetError();
    }
}

SoundEffect* SoundEffect::get_null_sound() {
    static SoundEffect empty_sound;
    return &empty_sound;
}

/**
 * @brief Plays the sound once
 */
void SoundEffect::play(int loops, int length_ms) {
    m_current_channel = Mix_PlayChannelTimed(-1, m_sound.get(), loops, length_ms);
    set_position();
}

void SoundEffect::play_fadein(int fadein_ms, int loops, int length_ms) {
    m_current_channel = Mix_FadeInChannelTimed(-1, m_sound.get(), loops, fadein_ms, length_ms);
    set_position();
}

void SoundEffect::pause() {
    if(playing()) {Mix_Pause(m_current_channel);}
}

void SoundEffect::resume() {
    if(paused()) {Mix_Resume(m_current_channel);}
}

void SoundEffect::halt() {
    if(playing()) {Mix_HaltChannel(m_current_channel);}
    m_current_channel = -1;
}

void SoundEffect::halt_fadeout(int fadeout_ms) {
    if(playing()) {Mix_FadeOutChannel(m_current_channel, fadeout_ms);}
    m_current_channel = -1;
}

void SoundEffect::set_volume(float factor) {
    if(factor > 1.0f) {factor = 1.0f;}
    else if(factor < 0.0f) {factor = 0.0f;}
    Mix_VolumeChunk(m_sound.get(), static_cast<int>(MIX_MAX_VOLUME * factor));
}

void SoundEffect::set_global_volume(float factor) {
    if(factor > 1.0f) {factor = 1.0f;}
    else if(factor < 0.0f) {factor = 0.0f;}
    Mix_Volume(-1, static_cast<int>(MIX_MAX_VOLUME * factor));
}

float SoundEffect::get_volume() {
    return static_cast<float>(Mix_VolumeChunk(m_sound.get(), -1)) / MIX_MAX_VOLUME;
}

float SoundEffect::get_global_volume() {
    return static_cast<float>(Mix_Volume(-1, -1)) / MIX_MAX_VOLUME;
}

bool SoundEffect::active() {
    if(m_current_channel < 0) {return false;}
    else if(Mix_GetChunk(m_current_channel) == m_sound.get()) {
        return true;
    }
    else {
        m_current_channel = -1;
        return false;
    }
}

bool SoundEffect::playing() {
    return (active() && Mix_Playing(m_current_channel));
}

bool SoundEffect::paused() {
    return (active() && Mix_Paused(m_current_channel));
}

void SoundEffect::set_position() {
    if(POSITIONAL_AUDIO) {Mix_SetPosition(m_current_channel, m_angle, m_distance);}
}

void SoundEffect::set_position(int angle, float distance) {
    m_angle = static_cast<Sint16>(angle);
    if(distance > 1.0f) {distance = 1.0f;}
    else if(distance < 0.0f) {distance = 0.0f;}
    m_distance = static_cast<Uint8>(distance * MAX_DISTANCE);
    if(active()) {set_position();}
}

