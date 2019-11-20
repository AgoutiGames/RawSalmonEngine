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
#include <memory>
#include <SDL.h>

#include "map/layer.hpp"
#include "util/game_types.hpp"
#include "util/smart.hpp"

class Primitive;

/**
 * @brief A map layer which consist of a list of actors
 */
class ObjectLayer : public Layer{
    public:

        bool render(const Camera& camera) const override;
        void update(bool late = false);

        LayerType get_type() override {return LayerType::object;}

        Actor* add_actor(Actor a);
        std::vector<Actor*> get_actors();
        std::vector<Actor*> get_actors(std::string name);
        Actor* get_actor(std::string name);
        bool erase_actor(std::string name);
        bool erase_actor(Actor* pointer);

        /// @note Takes ownership of the supplied pointer
        void add_primitive(Primitive* primitive);
        Primitive* get_primitive(std::string name) const;
        std::vector<Primitive*> get_primitives();
        bool erase_primitive(std::string name);

        bool get_suspended() const {return m_suspended;}
        void suspend() {m_suspended = true;}
        void unsuspend() {m_suspended = false;}

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
        std::list<Smart<Primitive>> m_primitives;
        bool m_suspended = false;

        static unsigned next_object_id;
};


#endif // OBJECT_LAYER_HPP_INCLUDED
