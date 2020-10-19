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
#ifndef PRELOADER_HPP_INCLUDED
#define PRELOADER_HPP_INCLUDED

#include <string>
#include <vector>
#include <SDL.h>

namespace salmon { namespace internal {

class GameInfo;

class Preloader {
    public:
        Preloader(GameInfo* game);

        void add_directory(std::string full_path);

        bool load_recursive(Uint32 milliseconds);
        bool load_file(std::string full_path);

        void clear();
    private:
        static bool ends_with(const std::string& str, const std::string& end);

        static const std::vector<std::string> m_image_formats;
        static const std::vector<std::string> m_sound_formats;
        static const std::vector<std::string> m_music_formats;

        std::vector<std::string> m_directories;
        std::vector<std::string> m_files;
        GameInfo* m_game;
};
}} // namespace salmon::internal

#endif // PRELOADER_HPP_INCLUDED
