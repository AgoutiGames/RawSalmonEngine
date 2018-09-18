/*
 * Copyright 2017-2018 Agouti Games Team (see the AUTHORS file)
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
#include "actor/data_block.hpp"

#include <string>
#include <iostream>
#include <map>

void DataBlock::set_val(std::string name, bool val) {m_data_bool[name] = val;}
void DataBlock::set_val(std::string name, int val) {m_data_int[name] = val;}
void DataBlock::set_val(std::string name, float val) {m_data_float[name] = val;}
void DataBlock::set_val(std::string name, std::string val) {m_data_string[name] = val;}
//void DataBlock::set_val(std::string name, Actor& val) {m_data_actor[name] = val;}

bool DataBlock::get_val_bool(std::string name) {
    if(m_data_bool.find(name) == m_data_bool.end()) {
        std::cerr << "Could not find boolean " << name << "\n";
        return false;
    }
    else{
        return m_data_bool[name];
    }
}

int DataBlock::get_val_int(std::string name) {
    if(m_data_int.find(name) == m_data_int.end()) {
        std::cerr << "Could not find integer " << name << "\n";
        return 0;
    }
    else{
        return m_data_int[name];
    }
}

float DataBlock::get_val_float(std::string name) {
    if(m_data_float.find(name) == m_data_float.end()) {
        std::cerr << "Could not find float " << name << "\n";
        return 0.0f;
    }
    else{
        return m_data_float[name];
    }
}

std::string DataBlock::get_val_string(std::string name) {
    if(m_data_string.find(name) == m_data_string.end()) {
        std::cerr << "Could not find string " << name << "\n";
        return "";
    }
    else{
        return m_data_string[name];
    }
}
