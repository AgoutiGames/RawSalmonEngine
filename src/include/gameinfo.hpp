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
#ifndef GAMEINFO_LIB_HPP_INCLUDED
#define GAMEINFO_LIB_HPP_INCLUDED

#include <string>
#include <memory>

#include "./data_block_ref.hpp"
#include "./input_cache_ref.hpp"

class GameInfo;

namespace salmon {
    class GameInfo{
        public:
            GameInfo(unsigned screen_w, unsigned screen_h);
            ~GameInfo();

            bool init();
            void close();

            void render();
            bool update();

            bool load_map(std::string mapfile, bool absolute = false);
            void close_map();

            DataBlockRef get_data();
            InputCacheRef get_input_cache();

        private:
            std::unique_ptr<::GameInfo> m_impl;
    };
}


#endif // GAMEINFO_LIB_HPP_INCLUDED
