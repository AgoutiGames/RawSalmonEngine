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
#ifndef AUDIO_MANAGER_HPP_INCLUDED
#define AUDIO_MANAGER_HPP_INCLUDED

#include <map>
#include <vector>

#include "audio/music.hpp"
#include "audio/sound_effect.hpp"

namespace salmon { namespace internal {

class AudioManager {
public:
    bool load_music(std::string path, bool absolute = false);
    bool load_sound(std::string path, bool absolute = false);

    Music& get_music(std::string path, bool absolute = false);
    SoundEffect& get_sound(std::string path, bool absolute = false);

    void set_music_path(std::string path) {m_music_base_path = path;}
    void set_sound_path(std::string path) {m_sounds_base_path = path;}

    void set_master_sound_volume(float factor) {SoundEffect::set_global_volume(factor);}
    float get_master_sound_volume() {return SoundEffect::get_global_volume();}

    void clear_music() {m_music.clear();}
    void clear_sounds() {m_sounds.clear();}
    void clear() {clear_music();clear_sounds();}
private:
    std::map<std::string,Music> m_music;
    std::map<std::string,SoundEffect> m_sounds;

    std::string m_music_base_path = "";
    std::string m_sounds_base_path = "";

    Music m_dummy_music = Music();
    SoundEffect m_dummy_sound = SoundEffect();
};
}} // namespace salmon::internal

#endif // AUDIO_MANAGER_HPP_INCLUDED
