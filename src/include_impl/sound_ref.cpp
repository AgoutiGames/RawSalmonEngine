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
#include "sound_ref.hpp"

#include "audio/sound_effect.hpp"

using namespace salmon;

Sound::Sound() : m_impl{internal::SoundEffect::get_null_sound()} {}
Sound::Sound(internal::SoundEffect& impl) : m_impl{new internal::SoundEffect(impl)} {}

Sound::Sound(const Sound& sound) : Sound(*sound.m_impl) {}
Sound::Sound(Sound&& sound) : m_impl{sound.m_impl} {
    sound.m_impl = internal::SoundEffect::get_null_sound();
}
Sound& Sound::operator=(const Sound& sound) {
    if(m_impl != internal::SoundEffect::get_null_sound()) {delete m_impl;}
    m_impl = new internal::SoundEffect(*sound.m_impl);
    return *this;
}
Sound& Sound::operator=(Sound&& sound) {
    if(m_impl != internal::SoundEffect::get_null_sound()) {delete m_impl;}
    m_impl = sound.m_impl;
    sound.m_impl = internal::SoundEffect::get_null_sound();
    return *this;
}
Sound::~Sound() {if(m_impl != internal::SoundEffect::get_null_sound()) {delete m_impl;}}

bool Sound::good() const {return m_impl->good();}
void Sound::play(int loops, int length_ms) {m_impl->play(loops,length_ms);}
void Sound::play_fadein(int fadein_ms, int loops, int length_ms) {m_impl->play_fadein(fadein_ms,loops,length_ms);}
void Sound::pause() {m_impl->pause();}
void Sound::resume() {m_impl->resume();}
void Sound::halt() {m_impl->halt();}
void Sound::halt_fadeout(int fadeout_ms) {m_impl->halt_fadeout(fadeout_ms);}

void Sound::set_position(int angle, float distance) {m_impl->set_position(angle,distance);}
void Sound::reset_position() {m_impl->reset_position();}

float Sound::get_volume() {return m_impl->get_volume();}
void Sound::set_volume(float factor) {m_impl->set_volume(factor);}
bool Sound::playing() {return m_impl->playing();}
bool Sound::paused() {return m_impl->paused();}
