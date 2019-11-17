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
#include "data_block_ref.hpp"

#include "actor/data_block.hpp"

using salmon::DataBlockRef;

DataBlockRef::DataBlockRef(DataBlock& impl) : m_impl{&impl} {}

void DataBlockRef::set_val(std::string name, bool val) {m_impl->set_val(name,val);}
void DataBlockRef::set_val(std::string name, int val) {m_impl->set_val(name,val);}
void DataBlockRef::set_val(std::string name, float val) {m_impl->set_val(name,val);}
void DataBlockRef::set_val(std::string name, std::string val) {m_impl->set_val(name,val);}

bool DataBlockRef::check_val_bool(std::string name) const {return m_impl->check_val_bool(name);}
bool DataBlockRef::check_val_int(std::string name) const {return m_impl->check_val_int(name);}
bool DataBlockRef::check_val_float(std::string name) const {return m_impl->check_val_float(name);}
bool DataBlockRef::check_val_string(std::string name) const {return m_impl->check_val_string(name);}

bool DataBlockRef::get_val_bool(std::string name) const {return m_impl->get_val_bool(name);}
int DataBlockRef::get_val_int(std::string name) const {return m_impl->get_val_int(name);}
float DataBlockRef::get_val_float(std::string name) const {return m_impl->get_val_float(name);}
std::string DataBlockRef::get_val_string(std::string name) const {return m_impl->get_val_string(name);}
