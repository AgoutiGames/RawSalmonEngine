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

#include "core/gameinfo.hpp"

salmon::GameInfo::GameInfo() : m_impl{std::make_unique<::GameInfo>()} {}
salmon::GameInfo::~GameInfo() {}

bool salmon::GameInfo::init() {return m_impl->init();}
void salmon::GameInfo::close() {return m_impl->close();}

void salmon::GameInfo::render() {m_impl->render();}
bool salmon::GameInfo::update() {return m_impl->update();}

bool salmon::GameInfo::load_map(std::string mapfile, bool absolute) {return m_impl->load_map(mapfile, absolute);}
void salmon::GameInfo::close_map() {m_impl->close_map();}
salmon::MapRef salmon::GameInfo::get_map() {return m_impl->get_map();}

void salmon::GameInfo::set_window_size(unsigned width, unsigned height) {m_impl->set_window_size(width,height);}
bool salmon::GameInfo::set_fullscreen(bool mode) {return m_impl->set_fullscreen(mode);}
bool salmon::GameInfo::set_game_resolution(unsigned width, unsigned height) {return m_impl->set_game_resolution(width,height);}
bool salmon::GameInfo::set_linear_filtering(bool mode) {return m_impl->set_linear_filtering(mode);}
void salmon::GameInfo::set_window_resizable(bool mode) {m_impl->set_window_resizable(mode);}

unsigned salmon::GameInfo::get_game_x_resolution() const {return m_impl->get_game_x_resolution();}
unsigned salmon::GameInfo::get_game_y_resolution() const {return m_impl->get_game_y_resolution();}

unsigned salmon::GameInfo::get_window_x_resolution() const {return m_impl->get_window_x_resolution();}
unsigned salmon::GameInfo::get_window_y_resolution() const {return m_impl->get_window_y_resolution();}

unsigned salmon::GameInfo::get_screen_x_resolution() const {return m_impl->get_screen_x_resolution();}
unsigned salmon::GameInfo::get_screen_y_resolution() const {return m_impl->get_screen_y_resolution();}

bool salmon::GameInfo::window_minimized() const {return m_impl->window_minimized();}
bool salmon::GameInfo::window_active() const {return m_impl->window_active();}

void salmon::GameInfo::add_preload_directory(std::string dir) {
    m_impl->get_preloader().add_directory(m_impl->get_resource_path() + dir);
}
bool salmon::GameInfo::preload(float seconds) {
    return m_impl->get_preloader().load_recursive(static_cast<Uint32>(seconds * 1000));
}

salmon::AudioManagerRef salmon::GameInfo::get_audio_manager() {return m_impl->get_audio_manager();}
salmon::DataBlockRef salmon::GameInfo::get_data() {return m_impl->get_data();}
salmon::InputCacheRef salmon::GameInfo::get_input_cache() {return m_impl->get_input_cache();}
