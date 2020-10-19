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
#include "util/preloader.hpp"

#include <experimental/filesystem>

#include "core/gameinfo.hpp"

namespace fs = std::experimental::filesystem;

using namespace salmon::internal;

#ifdef __EMSCRIPTEN__
    const std::vector<std::string> Preloader::m_image_formats = {".png",".PNG"};
    const std::vector<std::string> Preloader::m_music_formats = {"_M.ogg", "_M.OGG"};
    const std::vector<std::string> Preloader::m_sound_formats = {".wav", ".WAV", ".ogg", ".OGG"};
#else
    const std::vector<std::string> Preloader::m_image_formats = {".png",".PNG",".jpg",".JPG",".jpeg",".JPEG",".tif", ".TIF", ".tiff", ".TIFF"};
    const std::vector<std::string> Preloader::m_music_formats = {".flac", ".FLAC", ".mp3",".MP3", "_M.ogg", "_M.OGG"};
    const std::vector<std::string> Preloader::m_sound_formats = {".wav", ".WAV", ".ogg", ".OGG"};
#endif

Preloader::Preloader(GameInfo* game) : m_game{game} {}


void Preloader::add_directory(std::string full_path) {
    m_directories.push_back(full_path);
}

bool Preloader::load_recursive(Uint32 milliseconds) {
    if(m_files.empty()) {
        for(std::string dir : m_directories) {
            for(const fs::directory_entry& p: fs::recursive_directory_iterator(dir)) {
                m_files.push_back(p.path().string());
            }
        }
        m_directories.clear();
    }
    Uint32 start = SDL_GetTicks();
    while(!m_files.empty()) {
        load_file(m_files.back());
        m_files.pop_back();
        if((SDL_GetTicks() - start) >= milliseconds) {
            return false;
        }
    }
    return true;
}

bool Preloader::load_file(std::string full_path) {
    for(std::string format : m_image_formats) {
        if(ends_with(full_path,format)) {
            return m_game->get_texture_cache().load(full_path);
        }
    }
    for(std::string format : m_sound_formats) {
        if(ends_with(full_path,format)) {
            return m_game->get_audio_manager().load_sound(full_path,true);
        }
    }
    for(std::string format : m_music_formats) {
        if(ends_with(full_path,format)) {
            return m_game->get_audio_manager().load_music(full_path,true);
        }
    }
    return false;
}

bool Preloader::ends_with(const std::string& str, const std::string& end) {
    for(size_t i = 1; i <= end.size(); i++) {
        if(str[str.size() - i] != end[end.size() - i]) {return false;}
    }
    return true;
}

void Preloader::clear() {
    m_files.clear();
    m_directories.clear();
}
