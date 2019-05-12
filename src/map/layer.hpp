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
#ifndef LAYER_HPP_INCLUDED
#define LAYER_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <vector>
#include <list>
#include <string>

#include "util/tinyxml2.h"

class Actor;
class Camera;
class LayerCollection;
class MapData;

/**
 * @brief Parse, store and draw individual layers of the game map
 *
 * Can store all possible layer types
 */

class Layer {
    public:
        enum LayerType{
            map,
            object,
            image,
            undefinied
        };

        // Virtual deconstructor makes sure that the dtor of dervied is called when deleted through pointer to base
        virtual ~Layer();

        virtual bool render(const Camera& camera) const = 0;
        virtual void update() = 0;

        virtual bool collide(const SDL_Rect* rect, int& x_max, int& y_max, std::vector<Actor*>& collided, std::string type = "COLLIDE") = 0;
        virtual bool collide(const SDL_Rect* rect, std::vector<Actor*>& collided, std::string type = "COLLIDE") = 0;
        virtual bool collide(const SDL_Rect* rect, std::string type = "COLLIDE") = 0;

        virtual LayerType get_type() {return LayerType::undefinied;}

        static Layer* parse(tinyxml2::XMLElement* source, LayerCollection* layer_collection, tinyxml2::XMLError& eResult);

    protected:
        // This constructor can only be invoked in the initialiser list of derived class, not by itself!
        // If this class wouldn't be abstract, the protectedness of the constructor would make it "abstract like"
        Layer(std::string name, LayerCollection* layer_collection);

        LayerCollection* m_layer_collection;
        std::string m_name;
        int m_offset_x = 0; // Taken as position for "image"
        int m_offset_y = 0;
};

#endif // LAYER_HPP_INCLUDED
