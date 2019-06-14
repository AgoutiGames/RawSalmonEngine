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
#ifndef OBJECT_LAYER_HPP_INCLUDED
#define OBJECT_LAYER_HPP_INCLUDED

#include <vector>
#include <map>
#include <string>
#include <SDL2/SDL.h>

#include "map/layer.hpp"
#include "util/game_types.hpp"

/**
 * @brief A map layer which consist of a list of actors
 */
class ObjectLayer : public Layer{
    public:

        bool render(const Camera& camera) const override;
        void update();

        LayerType get_type() override {return LayerType::object;}

        std::vector<Actor*> get_actors();
        std::vector<Actor*> get_actors(std::string name);
        Actor* get_actor(std::string name);

        std::vector<Actor*> get_clip(const SDL_Rect& rect);
        std::vector<const Actor*> get_clip(const SDL_Rect& rect) const;

        static ObjectLayer* parse(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult);

        ObjectLayer(const ObjectLayer& other) = delete;
        ObjectLayer& operator=(const ObjectLayer& other) = delete;

        ObjectLayer(ObjectLayer&& other) = default;
        ObjectLayer& operator=(ObjectLayer&& other) = default;

    protected:
        ObjectLayer(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult);

    private:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source);

        std::list<Actor> m_obj_grid;
};


#endif // OBJECT_LAYER_HPP_INCLUDED
