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
#include "core/font_manager.hpp"

#include <iostream>

void FontManager::clear() {
    for(auto& font : m_fonts) {
        for(auto& instance : font.second) {
            TTF_CloseFont(instance.second);
        }
    }
}


TTF_Font* FontManager::get_font(std::string name, int pt_size) {
    auto font_it = m_fonts.find(name);
    // Check if font already exists
    if(font_it != m_fonts.end()) {
        // Font exists with correct size
        if(font_it->second.find(pt_size) != font_it->second.end()) {
            return m_fonts.at(name).at(pt_size);
        }
        // Font exists but not the exact size
        else {
            std::string path = m_base_path + name + ".ttf";
            TTF_Font* new_font = TTF_OpenFont(path.c_str(), pt_size);
            if(new_font == nullptr) {
                // This shouldnt ever happen since font was already used before!
                std::cerr << "Failed loading font at: " << path << "SDL_ttf Error: " << TTF_GetError() << "\n";
                return nullptr;
            }
            else {
                m_fonts.at(name)[pt_size] = new_font;
                return new_font;
            }
        }
    }
    // Try loading font
    else {
        std::string path = m_base_path + name + ".ttf";
        TTF_Font* new_font = TTF_OpenFont(path.c_str(), pt_size);
        // Loading font succeeds
        if(new_font != nullptr) {
            m_fonts[name][pt_size] = new_font;
            return new_font;
        }
        // Use fallback font
        else {
            std::cerr << "Can't find font at " << path << ", use fallback font instead\n";
            return get_fallback(pt_size);
        }
    }
}

TTF_Font* FontManager::get_fallback(int pt_size) {
    auto font_it = m_fonts[m_default_font].find(pt_size);
    // Check if font already exists
    if(font_it == m_fonts[m_default_font].end()) {
        std::string path = m_base_path + m_default_font + ".ttf";
        TTF_Font* new_font = TTF_OpenFont(path.c_str(), pt_size);
        if(new_font == nullptr) {
            // This shouldnt ever happen since font is default fallback option!
            std::cerr << "Failed loading font at: " << path << "SDL_ttf Error: " << TTF_GetError() << "\n";
            return nullptr;
        }
        else {
            m_fonts[m_default_font][pt_size] = new_font;
        }
    }

    return m_fonts[m_default_font][pt_size];
}
