/*
 * Copyright 2017-2020 Agouti Games Team (see the AUTHORS file)
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

using namespace salmon;

MapRef::MapRef(internal::MapData& impl) : m_impl{&impl} {}
MapRef::MapRef(internal::MapData* impl) : m_impl{impl} {}

bool MapRef::render() const {return m_impl->render();}
void MapRef::update() {m_impl->update();}

std::vector<ActorRef> MapRef::get_actors() {
    std::vector<ActorRef> temp;
    auto input = m_impl->get_layer_collection().get_actors();
    for(auto* a : input) {
        temp.emplace_back(*a);
    }
    return temp;
}
CameraRef MapRef::get_camera() {return m_impl->get_camera();}

ActorRef MapRef::add_actor(std::string actor_template_name, std::string layer_name, std::string actor_name) {
    if(m_impl->is_actor(actor_template_name)) {
        internal::Actor a = m_impl->get_actor(actor_template_name);
        return add_actor(&a, layer_name, actor_name);
    }
    else {
        std::cerr << "There is no actor template called: \"" << actor_template_name << "\"\n";
        return ActorRef(nullptr);
    }
}
ActorRef MapRef::add_actor(ActorRef actor, std::string layer_name, std::string actor_name) {
    internal::Layer* dest_layer = m_impl->get_layer_collection().get_layer(layer_name);
    if(dest_layer == nullptr) {
        std::cerr << "There is no layer called: \"" << layer_name << "\"\n";
        return ActorRef(nullptr);
    }
    else if(dest_layer->get_type() != internal::Layer::object) {
        std::cerr << "The layer: \"" << layer_name << "\" is no object layer!\n";
        return ActorRef(nullptr);
    }
    else {
        internal::ObjectLayer* layer = static_cast<internal::ObjectLayer*>(dest_layer);
        internal::Actor* added = layer->add_actor(*actor.m_impl);
        added->set_name(actor_name);
        return ActorRef(added);
    }
}

bool MapRef::remove_actor(ActorRef actor) {
    std::vector<internal::ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    for(internal::ObjectLayer* o : obj_layers) {
        if(o->erase_actor(actor.m_impl)) {return true;}
    }
    return false;
}

TextRef MapRef::get_text(std::string name) {
    std::vector<internal::ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    internal::Primitive* text = nullptr;
    for(internal::ObjectLayer* l : obj_layers) {
        text = l->get_primitive(name);
        if(text == nullptr) {continue;}
        if(text->get_type() == internal::Primitive::text) {return static_cast<internal::PrimitiveText*>(text);}
    }
    return nullptr;
}
TextRef MapRef::add_text(std::string text_name, std::string layer_name, float x_pos, float y_pos, std::string text, TextRef::Attributes a) {
    internal::Layer* dest_layer = m_impl->get_layer_collection().get_layer(layer_name);
    if(dest_layer == nullptr) {
        std::cerr << "There is no layer called: \"" << layer_name << "\"\n";
        return TextRef(nullptr);
    }
    else if(dest_layer->get_type() != internal::Layer::object) {
        std::cerr << "The layer: \"" << layer_name << "\" is no object layer!\n";
        return TextRef(nullptr);
    }
    else {
        internal::ObjectLayer* layer = static_cast<internal::ObjectLayer*>(dest_layer);
        internal::PrimitiveText::Attributes temp = {a.font_family, {a.color.r,a.color.g,a.color.b,a.color.a}, a.pixel_size, a.wrap, a.bold, a.italic, a.underline, a.strikeout, a.kerning};
        internal::PrimitiveText* t_obj = new internal::PrimitiveText(x_pos,y_pos,text,temp, *m_impl,text_name);
        layer->add_primitive(t_obj);
        return TextRef(*t_obj);
    }
}
bool MapRef::remove_text(TextRef text) {
    std::vector<internal::ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    for(internal::ObjectLayer* l : obj_layers) {
        if(l->erase_primitive(text.m_impl)) {return true;}
    }
    return false;
}

bool MapRef::hide_layer(std::string layer_name) {
    internal::Layer* temp = m_impl->get_layer_collection().get_layer(layer_name);
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
    internal::Layer* temp = m_impl->get_layer_collection().get_layer(layer_name);
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
DataBlockRef MapRef::get_data() {return m_impl->get_data();}
