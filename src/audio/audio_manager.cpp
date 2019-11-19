/*
 * Copyright 2017-2019 Agouti Games Team (see the AUTHORS file)
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
#include "audio/audio_manager.hpp"


bool AudioManager::load_music(std::string path) {
    path = m_music_base_path + path;
    Music temp(path);
    if(temp.good()) {
        m_music[path] = temp;
        return true;
    }
    else {
        return false;
    }
}
bool AudioManager::load_sound(std::string path) {
    path = m_sounds_base_path + path;
    SoundEffect temp(path);
    if(temp.good()) {
        m_sounds[path] = temp;
        return true;
    }
    else {
        return false;
    }
}

Music& AudioManager::get_music(std::string path) {
    path = m_music_base_path + path;
    if(m_music.find(path) != m_music.end() || load_music(path)) {
        return m_music.at(path);
    }
    else {
        return m_dummy_music;
    }

}
SoundEffect& AudioManager::get_sound(std::string path) {
    path = m_sounds_base_path + path;
    if(m_sounds.find(path) != m_sounds.end() || load_sound(path)) {
        return m_sounds.at(path);
    }
    else {
        return m_dummy_sound;
    }
}
