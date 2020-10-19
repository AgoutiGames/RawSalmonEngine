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
#ifndef MAP_REF_HPP_INCLUDED
#define MAP_REF_HPP_INCLUDED

#include <string>
#include <vector>

#include "./actor_ref.hpp"
#include "./text_ref.hpp"
#include "./data_block_ref.hpp"
#include "./camera_ref.hpp"

namespace salmon {

namespace internal{class MapData;}

class MapData {
    public:
        MapData(internal::MapData& impl);
        MapData(internal::MapData* impl);
        virtual ~MapData() = default;

        /// Renders this map to screen
        bool render() const;
        /// Updates the maps content (sorts actors for proper rendering order)
        void update();

        /// Returns a vector of references to all actors on all map layers
        std::vector<Actor> get_actors();
        /// Returns reference to the camera controlling rendering frame
        Camera get_camera();

        /**
         * @brief Generate a new actor from a template
         * @param actor_template_name The name of the actor template
         * @param layer_name The name of the layer for the actor to reside in
         * @param actor_name The name of the newly generated actor used to identify him
         * @return ActorRef object which may be invalid if actor_template or layer doesn't exist
         * @warning Always check if returned ActorRef returns true on good() method
         */
        Actor add_actor(std::string actor_template_name, std::string layer_name, std::string actor_name = "GENERATED");

        /**
         * @brief Generate a new actor from existing actor
         * @param actor The actor to be duplicated
         * @param layer_name The name of the layer for the actor to reside in
         * @param actor_name The name of the newly generated actor used to identify him
         * @return ActorRef object which may be invalid if actor is invalid or layer doesn't exist
         * @warning Always check if returned ActorRef returns true on good() method
         */
        Actor add_actor(Actor actor, std::string layer_name, std::string actor_name = "GENERATED");

        /// Removes actor from this map. Returns true if removal worked
        bool remove_actor(Actor actor);

        /**
         * @brief Retrieve text object by name
         * @param name The name of the text object
         * @return TextRef object which may be invalid if no text object with the supplied name exists
         * @warning Always check if returned TextRef returns true on good() method
         */
        Text get_text(std::string name);

        /**
         * @brief Add text object by to map
         * @param text_name The name of the text object
         * @param layer_name The name of the layer to add the text to
         * @param x_pos, y_pos The position of the text object in world coords relative to upper left corner of text
         * @param text The text to be displayed on screen
         * @param a The attribute object describing the appearance of the text
         * @return TextRef object which may be invalid if layer doesn't exist
         * @warning Always check if returned TextRef returns true on good() method
         */
        Text add_text(std::string text_name, std::string layer_name, float x_pos, float y_pos, std::string text, Text::Attributes a);

        /// Removes text object from this map. Returns true if removal worked
        bool remove_text(Text text);

        /**
         * @brief Supress rendering of a layer
         * @param layer_name The name of the to be hidden layer
         * @return True if layer exists
         */
        bool hide_layer(std::string layer_name);
        /**
         * @brief Reenable rendering of a layer
         * @param layer_name The name of the to be hidden layer
         * @return True if layer exists
         */
        bool unhide_layer(std::string layer_name);

        /// Returns map width in pixels (not taking layer offset into account)
        unsigned get_w() const;
        /// Returns map height in pixels (not taking layer offset into account)
        unsigned get_h() const;
        /// Returns time in seconds which has passed since last update
        float get_delta_time() const;
        /// Returns the full path to the directory of this .tmx map file
        std::string get_path() const;
        /// Returns reference to DataBlock of this map which holds all property values supplied via tiled
        DataBlock get_data();
    private:
        internal::MapData* m_impl;
};
}

#endif // MAP_REF_HPP_INCLUDED
