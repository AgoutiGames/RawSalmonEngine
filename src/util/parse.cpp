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
 #include "util/parse.hpp"

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "graphics/texture.hpp"
#include "util/tinyxml2.h"

 /**
 * @brief Converts the xmlelement to a proper SDL_Rect with checking
 * @param source The @c XMLElement which holds the information
 * @param rect The rect which gets produced
 * @return @c XMLError Indicating success or failure
 * @note Only the first hitbox gets parsed, multiple hitboxes lead to an error
 */
tinyxml2::XMLError parse::hitbox(tinyxml2::XMLElement* source, SDL_Rect& rect) {
    using namespace tinyxml2;
    XMLError eResult;

    if(source->FirstChildElement("ellipse") != nullptr) {
        std::cerr << "Hitbox can't be an ellipse!\n";
        return XML_WRONG_ATTRIBUTE_TYPE;
    }
    if(source->FirstChildElement("polygon") != nullptr) {
        std::cerr << "Hitbox can't be a polygon!\n";
        return XML_WRONG_ATTRIBUTE_TYPE;
    }
    if(source->FirstChildElement("polyline") != nullptr) {
        std::cerr << "Hitbox can't be a polyline!\n";
        return XML_WRONG_ATTRIBUTE_TYPE;
    }
    if(source->NextSiblingElement("object") != nullptr) {
        std::cerr << "Multiple hitboxes are not supported!\n";
        return XML_WRONG_ATTRIBUTE_TYPE;
    }
    SDL_Rect temp_rec;
    float temp;

    eResult = source->QueryFloatAttribute("x", &temp);
    if(eResult != XML_SUCCESS) return eResult;
    temp_rec.x = static_cast<int>(temp);

    eResult = source->QueryFloatAttribute("y", &temp);
    if(eResult != XML_SUCCESS) return eResult;
    temp_rec.y = static_cast<int>(temp);

    eResult = source->QueryFloatAttribute("width", &temp);
    if(eResult != XML_SUCCESS) return eResult;
    temp_rec.w = static_cast<int>(temp);

    eResult = source->QueryFloatAttribute("height", &temp);
    if(eResult != XML_SUCCESS) return eResult;
    temp_rec.h = static_cast<int>(temp);

    rect = temp_rec;
    return XML_SUCCESS;
}

/**
 * @brief Converts the xmlelement to multiple proper SDL_Rects with checking
 * @param source The @c XMLElement which holds the information
 * @param rects The rects which get produced
 * @return @c XMLError Indicating success or failure
 */
tinyxml2::XMLError parse::hitboxes(tinyxml2::XMLElement* source, std::map<std::string, SDL_Rect>& rects) {
    using namespace tinyxml2;
    XMLError eResult;

    while(source != nullptr) {
        if(source->FirstChildElement("ellipse") != nullptr) {
            std::cerr << "Hitbox can't be an ellipse!\n";
            return XML_WRONG_ATTRIBUTE_TYPE;
        }
        if(source->FirstChildElement("polygon") != nullptr) {
            std::cerr << "Hitbox can't be a polygon!\n";
            return XML_WRONG_ATTRIBUTE_TYPE;
        }
        if(source->FirstChildElement("polyline") != nullptr) {
            std::cerr << "Hitbox can't be a polyline!\n";
            return XML_WRONG_ATTRIBUTE_TYPE;
        }

        SDL_Rect temp_rec;
        float temp;
        std::string name;
        const char* p_name = source->Attribute("name");
        if(p_name == nullptr) {name = "COLLIDE";}
        else{name = p_name;}

        eResult = source->QueryFloatAttribute("x", &temp);
        if(eResult != XML_SUCCESS) return eResult;
        temp_rec.x = static_cast<int>(temp);

        eResult = source->QueryFloatAttribute("y", &temp);
        if(eResult != XML_SUCCESS) return eResult;
        temp_rec.y = static_cast<int>(temp);

        eResult = source->QueryFloatAttribute("width", &temp);
        if(eResult != XML_SUCCESS) return eResult;
        temp_rec.w = static_cast<int>(temp);

        eResult = source->QueryFloatAttribute("height", &temp);
        if(eResult != XML_SUCCESS) return eResult;
        temp_rec.h = static_cast<int>(temp);

        if(rects.find(name) != rects.end()) {
            std::cerr << "Possible multiple definition of hitbox: " << name << " !\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }

        rects[name] = temp_rec;

        source = source->NextSiblingElement("object");
    }
    return XML_SUCCESS;
}

/**
 * @brief Set the blendmode of a texture according to XML information
 * @param source The @c XMLElement which holds the information
 * @param img The texture which blendmode gets set
 * @return @c XMLError Indicating success or failure
 */
tinyxml2::XMLError parse::blendmode(tinyxml2::XMLElement* source, Texture& img) {
    using namespace tinyxml2;
    const char* p_mode;
    p_mode = source->Attribute("value");
    std::string mode(p_mode);
    if(p_mode == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
    else if(mode == "NONE") img.setBlendMode(SDL_BLENDMODE_NONE);
    else if(mode == "ALPHA") img.setBlendMode(SDL_BLENDMODE_BLEND);
    else if(mode == "ADD") img.setBlendMode(SDL_BLENDMODE_ADD);
    else if(mode == "COLOR") img.setBlendMode(SDL_BLENDMODE_MOD);
    else {
        std::cerr << "Unknown blend mode specified\n";
        return XML_ERROR_PARSING_ATTRIBUTE;
    }
    return XML_SUCCESS;
}

/**
 * @brief Parse the background color of a map to a variable
 * @param source The @c XMLElement which holds the information
 * @param color The referencte where the resulting color will be stored
 * @return @c CMLError Indicating sucess or failure
 */
tinyxml2::XMLError parse::bg_color(tinyxml2::XMLElement* source, SDL_Color& color) {
    using namespace tinyxml2;

    const char* p_bg_color;
    p_bg_color = source->Attribute("backgroundcolor");
    if(p_bg_color == nullptr) {
        std::cerr << "Map is missing a custom backgroundcolor, will use white as default\n";
        color = {255,255,255,255};

        return XML_ERROR_PARSING_ATTRIBUTE;
    }
    else {
        std::string s_bg_color = p_bg_color;
        s_bg_color.erase(s_bg_color.begin()); // Delete leading # sign

        // Check for possible alpha value (since ARGB and RGB is possible)
        if(s_bg_color.length() > 6) {
            color.a = std::stoul(std::string(s_bg_color, 0, 2), nullptr, 16);
            s_bg_color.erase(s_bg_color.begin(), s_bg_color.begin() + 2);
        }
        // Set to fully opaque if no value is supplied
        else {color.a = 255;}

        color.r = std::stoul(std::string(s_bg_color, 0, 2), nullptr, 16);
        color.g = std::stoul(std::string(s_bg_color, 2, 2), nullptr, 16);
        color.b = std::stoul(std::string(s_bg_color, 4, 2), nullptr, 16);

        return XML_SUCCESS;
    }
}

tinyxml2::XMLError Parser::parse_int(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_int.find(name) == m_int.end()) {
        return XML_NO_ATTRIBUTE;
    }
    return source->QueryIntAttribute(name.c_str(), m_int.at(name));
}

tinyxml2::XMLError Parser::parse_float(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_float.find(name) == m_float.end()) {
        return XML_NO_ATTRIBUTE;
    }
    return source->QueryFloatAttribute(name.c_str(), m_float.at(name));
}

tinyxml2::XMLError Parser::parse_double(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_double.find(name) == m_double.end()) {
        return XML_NO_ATTRIBUTE;
    }
    return source->QueryDoubleAttribute(name.c_str(), m_double.at(name));
}

tinyxml2::XMLError Parser::parse_string(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_string.find(name) == m_string.end()) {
        return XML_NO_ATTRIBUTE;
    }

    const char* p_value = source->Attribute("value");
    if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
    *m_string.at(name) = std::string(p_value);
    return XML_SUCCESS;
}

tinyxml2::XMLError Parser::parse_priority(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_priority.find(name) == m_priority.end()) {
        return XML_NO_ATTRIBUTE;
    }

    const char* p_value = source->Attribute("value");
    if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

    std::string value(p_value);
    Priority prio = str_to_priority(value);
    if(prio == Priority::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
    *m_priority.at(name) = prio;
    return XML_SUCCESS;
}

tinyxml2::XMLError Parser::parse_signal(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    std::string name(source->Attribute("name"));

    if(m_signal.find(name) == m_signal.end()) {
        return XML_NO_ATTRIBUTE;
    }

    const char* p_value = source->Attribute("value");
    if(p_value == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;

    std::string value(p_value);
    EventSignal sig = str_to_event_signal(value);
    if(sig == EventSignal::invalid) {return XML_ERROR_PARSING_ATTRIBUTE;}
    *m_signal.at(name) = sig;
    return XML_SUCCESS;
}


Parser::Parser(MapData& base_map) : m_base_map{&base_map} {
    parsers[0] = &Parser::parse_int;
    parsers[1] = &Parser::parse_float;
    parsers[2] = &Parser::parse_double;
    parsers[3] = &Parser::parse_string;
    parsers[4] = &Parser::parse_priority;
    parsers[5] = &Parser::parse_signal;
    /// ! Don't forget adding new parsing member function here!
}

tinyxml2::XMLError Parser::parse(tinyxml2::XMLElement* source) {
    using namespace tinyxml2;

    for(int counter = 0; source != nullptr; source = source->NextSiblingElement("property")) {
        bool no_attribute = true;

        if(source->Attribute("name") == nullptr) {
            std::cerr << "Name of event property \"" << source->Attribute("name") << "\" number: " << counter << " yields null!\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }

        for(ParsePointer p : parsers) {
            XMLError result = (this->*p)(source);
            if(result == XML_SUCCESS) {
                no_attribute = false;
                break;
            }
            else if(result == XML_NO_ATTRIBUTE) {
                continue;
            }
            else {
                std::cerr << "Failed parsing event property \"" << source->Attribute("name") << "\" number: " << counter << "\n";
                return result;
            }
        }

        if(no_attribute) {
            std::cerr << "Unknown event property \"" << source->Attribute("name") << "\" number: " << counter << "\n";
            return XML_ERROR_PARSING_ATTRIBUTE;
        }
    }
    return XML_SUCCESS;
}

