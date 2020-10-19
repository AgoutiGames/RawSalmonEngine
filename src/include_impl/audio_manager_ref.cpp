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
#include "audio_manager_ref.hpp"

#include "audio/audio_manager.hpp"

using namespace salmon;

AudioManagerRef::AudioManagerRef(internal::AudioManager& impl) : m_impl{&impl} {}

bool AudioManagerRef::load_music(std::string path, bool absolute) {return m_impl->load_music(path,absolute);}
bool AudioManagerRef::load_sound(std::string path, bool absolute) {return m_impl->load_sound(path,absolute);}

MusicRef AudioManagerRef::get_music(std::string path, bool absolute) {return m_impl->get_music(path,absolute);}
SoundRef AudioManagerRef::get_sound(std::string path, bool absolute) {return m_impl->get_sound(path,absolute);}

void AudioManagerRef::set_master_sound_volume(float factor) {m_impl->set_master_sound_volume(factor);}
float AudioManagerRef::get_master_sound_volume() {return m_impl->get_master_sound_volume();}

void AudioManagerRef::clear_music() {m_impl->clear_music();}
void AudioManagerRef::clear_sounds() {m_impl->clear_sounds();}
void AudioManagerRef::clear() {m_impl->clear();}
