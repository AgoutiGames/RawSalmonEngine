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
#include "music.hpp"

#include "audio/music.hpp"

namespace salmon {

Music::Music() : m_impl{internal::Music::get_null_music()} {}
Music::Music(internal::Music& impl) : m_impl{&impl} {}

bool Music::good() const {return m_impl->good();}
void Music::play(int loops) {m_impl->play(loops);}
void Music::play_fadein(int fadein_ms, int loops) {m_impl->play_fadein(fadein_ms,loops);}
void Music::play_fadein_pos(double seconds, int fadein_ms, int loops) {m_impl->play_fadein_pos(seconds,fadein_ms,loops);}
void Music::pause() {m_impl->pause();}
void Music::resume() {m_impl->resume();}
void Music::halt() {m_impl->halt();}
void Music::halt_fadeout(int fadeout_ms) {m_impl->halt_fadeout(fadeout_ms);}
void Music::rewind() {m_impl->rewind();}
void Music::set_position(double seconds) {m_impl->set_position(seconds);}

float Music::get_volume() {return m_impl->get_volume();}
void Music::set_volume(float factor) {m_impl->set_volume(factor);}
bool Music::playing() {return m_impl->playing();}
bool Music::paused() {return m_impl->paused();}

} // namespace salmon
