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

SoundRef::SoundRef() : m_impl{internal::SoundEffect::get_null_sound()} {}
SoundRef::SoundRef(internal::SoundEffect& impl) : m_impl{new internal::SoundEffect(impl)} {}

SoundRef::SoundRef(const SoundRef& sound) : SoundRef(*sound.m_impl) {}
SoundRef::SoundRef(SoundRef&& sound) : m_impl{sound.m_impl} {
    sound.m_impl = internal::SoundEffect::get_null_sound();
}
SoundRef& SoundRef::operator=(const SoundRef& sound) {
    if(m_impl != internal::SoundEffect::get_null_sound()) {delete m_impl;}
    m_impl = new internal::SoundEffect(*sound.m_impl);
    return *this;
}
SoundRef& SoundRef::operator=(SoundRef&& sound) {
    if(m_impl != internal::SoundEffect::get_null_sound()) {delete m_impl;}
    m_impl = sound.m_impl;
    sound.m_impl = internal::SoundEffect::get_null_sound();
    return *this;
}
SoundRef::~SoundRef() {if(m_impl != internal::SoundEffect::get_null_sound()) {delete m_impl;}}

bool SoundRef::good() const {return m_impl->good();}
void SoundRef::play(int loops, int length_ms) {m_impl->play(loops,length_ms);}
void SoundRef::play_fadein(int fadein_ms, int loops, int length_ms) {m_impl->play_fadein(fadein_ms,loops,length_ms);}
void SoundRef::pause() {m_impl->pause();}
void SoundRef::resume() {m_impl->resume();}
void SoundRef::halt() {m_impl->halt();}
void SoundRef::halt_fadeout(int fadeout_ms) {m_impl->halt_fadeout(fadeout_ms);}

void SoundRef::set_position(int angle, float distance) {m_impl->set_position(angle,distance);}
void SoundRef::reset_position() {m_impl->reset_position();}

float SoundRef::get_volume() {return m_impl->get_volume();}
void SoundRef::set_volume(float factor) {m_impl->set_volume(factor);}
bool SoundRef::playing() {return m_impl->playing();}
bool SoundRef::paused() {return m_impl->paused();}
