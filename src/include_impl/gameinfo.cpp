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
#include "gameinfo.hpp"

#include <SDL.h>

#include "core/gameinfo.hpp"
#include "actor/data_block.hpp"

salmon::GameInfo::GameInfo(unsigned screen_w, unsigned screen_h) : m_impl{std::make_unique<::GameInfo>(screen_w,screen_h)} {}
salmon::GameInfo::~GameInfo() {}

bool salmon::GameInfo::init() {return m_impl->init();}
void salmon::GameInfo::close() {return m_impl->close();}

void salmon::GameInfo::render() {m_impl->render();}
bool salmon::GameInfo::update() {return m_impl->update();}

bool salmon::GameInfo::load_map(std::string mapfile, bool absolute) {return m_impl->load_map(mapfile, absolute);}
void salmon::GameInfo::close_map() {m_impl->close_map();}
salmon::MapRef salmon::GameInfo::get_map() {return m_impl->get_map();}

salmon::DataBlockRef salmon::GameInfo::get_data() {return m_impl->get_data();}
salmon::InputCacheRef salmon::GameInfo::get_input_cache() {return m_impl->get_input_cache();}
