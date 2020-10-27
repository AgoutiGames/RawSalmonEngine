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
#include "mapdata.hpp"

#include <iostream>

#include "actor/actor.hpp"
#include "actor/primitive.hpp"
#include "actor/primitive_text.hpp"
#include "map/mapdata.hpp"
#include "map/layer_collection.hpp"
#include "map/object_layer.hpp"

namespace salmon {

MapData::MapData(internal::MapData& impl) : m_impl{&impl} {}
MapData::MapData(internal::MapData* impl) : m_impl{impl} {}

bool MapData::render() const {return m_impl->render();}
void MapData::update() {m_impl->update();}

std::vector<Actor> MapData::get_actors() {
    std::vector<Actor> temp;
    auto input = m_impl->get_layer_collection().get_actors();
    for(auto* a : input) {
        temp.emplace_back(*a);
    }
    return temp;
}
Camera MapData::get_camera() {return m_impl->get_camera();}

Actor MapData::add_actor(std::string actor_template_name, std::string layer_name, std::string actor_name) {
    if(m_impl->is_actor(actor_template_name)) {
        internal::Actor a = m_impl->get_actor(actor_template_name);
        return add_actor(&a, layer_name, actor_name);
    }
    else {
        std::cerr << "There is no actor template called: \"" << actor_template_name << "\"\n";
        return Actor(nullptr);
    }
}
Actor MapData::add_actor(Actor actor, std::string layer_name, std::string actor_name) {
    internal::Layer* dest_layer = m_impl->get_layer_collection().get_layer(layer_name);
    if(dest_layer == nullptr) {
        std::cerr << "There is no layer called: \"" << layer_name << "\"\n";
        return Actor(nullptr);
    }
    else if(dest_layer->get_type() != internal::Layer::object) {
        std::cerr << "The layer: \"" << layer_name << "\" is no object layer!\n";
        return Actor(nullptr);
    }
    else {
        internal::ObjectLayer* layer = static_cast<internal::ObjectLayer*>(dest_layer);
        internal::Actor* added = layer->add_actor(*actor.m_impl);
        added->set_name(actor_name);
        return Actor(added);
    }
}

bool MapData::remove_actor(Actor actor) {
    std::vector<internal::ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    for(internal::ObjectLayer* o : obj_layers) {
        if(o->erase_actor(actor.m_impl)) {return true;}
    }
    return false;
}

Text MapData::get_text(std::string name) {
    std::vector<internal::ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    internal::Primitive* text = nullptr;
    for(internal::ObjectLayer* l : obj_layers) {
        text = l->get_primitive(name);
        if(text == nullptr) {continue;}
        if(text->get_type() == internal::Primitive::text) {return static_cast<internal::PrimitiveText*>(text);}
    }
    return nullptr;
}
Text MapData::add_text(std::string text_name, std::string layer_name, Rect r, std::string text, Text::Attributes a) {
    internal::Layer* dest_layer = m_impl->get_layer_collection().get_layer(layer_name);
    if(dest_layer == nullptr) {
        std::cerr << "There is no layer called: \"" << layer_name << "\"\n";
        return Text(nullptr);
    }
    else if(dest_layer->get_type() != internal::Layer::object) {
        std::cerr << "The layer: \"" << layer_name << "\" is no object layer!\n";
        return Text(nullptr);
    }
    else {
        internal::ObjectLayer* layer = static_cast<internal::ObjectLayer*>(dest_layer);
        internal::PrimitiveText::Attributes temp = {a.font_family, {a.color.r,a.color.g,a.color.b,a.color.a}, a.pixel_size, a.wrap, a.bold, a.italic, a.underline, a.strikeout, a.kerning};
        internal::PrimitiveText* t_obj = new internal::PrimitiveText(r,text,temp, *m_impl,text_name);
        layer->add_primitive(t_obj);
        return Text(*t_obj);
    }
}
bool MapData::remove_text(Text text) {
    std::vector<internal::ObjectLayer*> obj_layers = m_impl->get_layer_collection().get_object_layers();
    for(internal::ObjectLayer* l : obj_layers) {
        if(l->erase_primitive(text.m_impl)) {return true;}
    }
    return false;
}

bool MapData::hide_layer(std::string layer_name) {
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
bool MapData::unhide_layer(std::string layer_name) {
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
PixelDimensions MapData::get_dimensions() const {return m_impl->get_dimensions();}
float MapData::get_delta_time() const {return m_impl->get_delta_time();}
std::string MapData::get_path() const {return m_impl->get_full_path();}
DataBlock MapData::get_data() {return m_impl->get_data();}
salmon::Transform* MapData::get_layer_transform(std::string layer_name) {return m_impl->get_layer_transform(layer_name);}

} // namespace salmon
