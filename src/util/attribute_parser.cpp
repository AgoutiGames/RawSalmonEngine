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
#include "util/attribute_parser.hpp"

#include <iostream>

#include "util/logger.hpp"

tinyxml2::XMLError AttributeParser::parse(tinyxml2::XMLElement* source, bool ignore_missing) {
    using namespace tinyxml2;
    XMLError eResult;
    for(auto& entry : m_bool) {
        eResult = source->QueryBoolAttribute(entry.first.c_str(), entry.second);
        if(eResult != XML_SUCCESS && (!ignore_missing || eResult != XML_NO_ATTRIBUTE)) {
            Logger(Logger::error) << "Failed at loading bool attribute: " << entry.first;
            return eResult;
        }
    }
    for(auto& entry : m_unsigned) {
        eResult = source->QueryUnsignedAttribute(entry.first.c_str(), entry.second);
        if(eResult != XML_SUCCESS && (!ignore_missing || eResult != XML_NO_ATTRIBUTE)) {
            Logger(Logger::error) << "Failed at loading unsigned int attribute: " << entry.first;
            return eResult;
        }
    }
    for(auto& entry : m_int) {
        eResult = source->QueryIntAttribute(entry.first.c_str(), entry.second);
        if(eResult != XML_SUCCESS && (!ignore_missing || eResult != XML_NO_ATTRIBUTE)) {
            Logger(Logger::error) << "Failed at loading int attribute: " << entry.first;
            return eResult;
        }
    }
    for(auto& entry : m_float) {
        eResult = source->QueryFloatAttribute(entry.first.c_str(), entry.second);
        if(eResult != XML_SUCCESS && (!ignore_missing || eResult != XML_NO_ATTRIBUTE)) {
            Logger(Logger::error) << "Failed at loading float attribute: " << entry.first;
            return eResult;
        }
    }
    for(auto& entry : m_string) {
        const char* p_name = source->Attribute(entry.first.c_str());
        if(p_name == nullptr && !ignore_missing) {
            Logger(Logger::error) << "Failed at loading string attribute: " << entry.first;
            return XML_NO_ATTRIBUTE;
        }
        if(p_name != nullptr) {
            *entry.second = p_name;
        }
    }
    return XML_SUCCESS;
}
