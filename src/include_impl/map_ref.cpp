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
#include "actor/primitive.hpp"
#include "actor/primitive_text.hpp"
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

salmon::ActorRef MapRef::add_actor(std::string actor_template_name, std::string layer_name, std::string actor_name) {
    if(m_impl->is_actor(actor_template_name)) {
        Actor a = m_impl->get_actor(actor_template_name);
        return add_actor(&a, layer_name, actor_name);
    }
    else {
        std::cerr << "There is no actor template called: \"" << actor_template_name << "\"\n";
        return salmon::ActorRef(nullptr);
    }
}
salmon::ActorRef MapRef::add_actor(salmon::ActorRef actor, std::string layer_name, std::string actor_name) {
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
        added->set_name(actor_name);
        return salmon::ActorRef(added);
    }
}

bool MapRef::remove_actor(salmon::ActorRef actor) {
    std::vector<ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    for(ObjectLayer* o : obj_layers) {
        if(o->erase_actor(actor.m_impl)) {return true;}
    }
    return false;
}

salmon::TextRef MapRef::get_text(std::string name) {
    std::vector<ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    Primitive* text = nullptr;
    for(ObjectLayer* l : obj_layers) {
        text = l->get_primitive(name);
        if(text == nullptr) {continue;}
        if(text->get_type() == Primitive::text) {return static_cast<PrimitiveText*>(text);}
    }
    return nullptr;
}
salmon::TextRef MapRef::add_text(std::string text_name, std::string layer_name, float x_pos, float y_pos, std::string text, TextRef::Attributes a) {
    Layer* dest_layer = m_impl->get_layer_collection().get_layer(layer_name);
    if(dest_layer == nullptr) {
        std::cerr << "There is no layer called: \"" << layer_name << "\"\n";
        return salmon::TextRef(nullptr);
    }
    else if(dest_layer->get_type() != Layer::object) {
        std::cerr << "The layer: \"" << layer_name << "\" is no object layer!\n";
        return salmon::TextRef(nullptr);
    }
    else {
        ObjectLayer* layer = static_cast<ObjectLayer*>(dest_layer);
        PrimitiveText::Attributes temp = {a.font_family, {a.color.r,a.color.g,a.color.b,a.color.a}, a.pixel_size, a.wrap, a.bold, a.italic, a.underline, a.strikeout, a.kerning};
        PrimitiveText* t_obj = new PrimitiveText(x_pos,y_pos,text,temp, *m_impl,text_name);
        layer->add_primitive(t_obj);
        return salmon::TextRef(*t_obj);
    }
}
bool MapRef::remove_text(salmon::TextRef text) {
    std::vector<ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    for(ObjectLayer* l : obj_layers) {
        if(l->erase_primitive(text.m_impl)) {return true;}
    }
    return false;
}

bool MapRef::hide_layer(std::string layer_name) {
    Layer* temp = m_impl->get_layer_collection().get_layer(layer_name);
    if(temp == nullptr) {
        std::cerr << "There is no layer called: \"" << layer_name << "\"\n";
        return false;
    }
    else {
        temp->hide();
        return true;
    }
}
bool MapRef::unhide_layer(std::string layer_name) {
    Layer* temp = m_impl->get_layer_collection().get_layer(layer_name);
    if(temp == nullptr) {
        std::cerr << "There is no layer called: \"" << layer_name << "\"\n";
        return false;
    }
    else {
        temp->unhide();
        return true;
    }
}

unsigned MapRef::get_w() const {return m_impl->get_w();}
unsigned MapRef::get_h() const {return m_impl->get_h();}
float MapRef::get_delta_time() const {return m_impl->get_delta_time();}
std::string MapRef::get_path() const {return m_impl->get_full_path();}
salmon::DataBlockRef MapRef::get_data() {return m_impl->get_data();}
