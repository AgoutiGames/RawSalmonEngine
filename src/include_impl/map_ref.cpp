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
#include "map_ref.hpp"

#include <iostream>

#include "actor/actor.hpp"
#include "map/mapdata.hpp"
#include "map/layer_collection.hpp"
#include "map/object_layer.hpp"

using salmon::MapRef;

MapRef::MapRef(MapData& impl) : m_impl{&impl} {}
MapRef::MapRef(MapData* impl) : m_impl{impl} {}

bool MapRef::render() const {return m_impl->render();}
void MapRef::update() {m_impl->update();}

std::vector<salmon::ActorRef> MapRef::get_actors() {
    std::vector<salmon::ActorRef> temp;
    auto input = m_impl->get_layer_collection().get_actors();
    for(auto* a : input) {
        temp.emplace_back(*a);
    }
    return temp;
}
salmon::CameraRef MapRef::get_camera() {return m_impl->get_camera();}

salmon::ActorRef MapRef::add_actor(std::string actor_template_name, std::string layer_name) {
    if(m_impl->is_actor(actor_template_name)) {
        Actor a = m_impl->get_actor(actor_template_name);
        return add_actor(&a, layer_name);
    }
    else {
        std::cerr << "There is no actor template called: \"" << actor_template_name << "\"\n";
        return salmon::ActorRef(nullptr);
    }
}
salmon::ActorRef MapRef::add_actor(salmon::ActorRef actor, std::string layer_name) {
    Layer* dest_layer = m_impl->get_layer_collection().get_layer(layer_name);
    if(dest_layer == nullptr) {
        std::cerr << "There is no layer called: \"" << layer_name << "\"\n";
        return salmon::ActorRef(nullptr);
    }
    else if(dest_layer->get_type() != Layer::object) {
        std::cerr << "The layer: \"" << layer_name << "\" is no object layer!\n";
        return salmon::ActorRef(nullptr);
    }
    else {
        ObjectLayer* layer = static_cast<ObjectLayer*>(dest_layer);
        Actor* added = layer->add_actor(*actor.m_impl);
        return salmon::ActorRef(added);
    }
}

bool MapRef::remove_actor(salmon::ActorRef actor) {
    std::vector<ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    bool success = false;
    for(ObjectLayer* o : obj_layers) {
        success = success || o->erase_actor(actor.m_impl);
    }
    return success;
}


unsigned MapRef::get_w() const {return m_impl->get_w();}
unsigned MapRef::get_h() const {return m_impl->get_h();}
float MapRef::get_delta_time() const {return m_impl->get_delta_time();}
std::string MapRef::get_path() const {return m_impl->get_full_path();}
salmon::DataBlockRef MapRef::get_data() {return m_impl->get_data();}
