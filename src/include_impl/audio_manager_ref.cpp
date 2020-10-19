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

AudioManager::AudioManager(internal::AudioManager& impl) : m_impl{&impl} {}

bool AudioManager::load_music(std::string path, bool absolute) {return m_impl->load_music(path,absolute);}
bool AudioManager::load_sound(std::string path, bool absolute) {return m_impl->load_sound(path,absolute);}

Music AudioManager::get_music(std::string path, bool absolute) {return m_impl->get_music(path,absolute);}
Sound AudioManager::get_sound(std::string path, bool absolute) {return m_impl->get_sound(path,absolute);}

void AudioManager::set_master_sound_volume(float factor) {m_impl->set_master_sound_volume(factor);}
float AudioManager::get_master_sound_volume() {return m_impl->get_master_sound_volume();}

void AudioManager::clear_music() {m_impl->clear_music();}
void AudioManager::clear_sounds() {m_impl->clear_sounds();}
void AudioManager::clear() {m_impl->clear();}
