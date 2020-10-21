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

#ifndef MAP_LAYER_HPP_INCLUDED
#define MAP_LAYER_HPP_INCLUDED

#include <vector>
#include <map>
#include <string>
#include <tuple>

#include "map/layer.hpp"
#include "map/tile.hpp"

namespace salmon { namespace internal {

class TilesetCollection;

/**
 * @brief A map layer which consist of 2D tile indices
 */
class MapLayer : public Layer{
    public:

        bool render(const Camera& camera) const override;

        std::vector< std::tuple<Uint32, int, int> > clip(Rect rect) const;
        std::vector<TileInstance> get_clip(Rect rect) const;

        LayerType get_type() override {return LayerType::map;}

        static MapLayer* parse(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult);

        MapLayer(const MapLayer& other) = delete;
        MapLayer& operator=(const MapLayer& other) = delete;

        MapLayer(MapLayer&& other) = default;
        MapLayer& operator=(MapLayer&& other) = default;

    protected:
        MapLayer(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult);

    private:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source);

        std::vector< std::tuple<Uint32, int, int> > clip_ortho(Rect rect) const;
        std::vector< std::tuple<Uint32, int, int> > clip_y_stagger(Rect rect) const;
        std::vector< std::tuple<Uint32, int, int> > clip_x_stagger(Rect rect) const;
        void calc_tile_range(Rect src_rect, int tile_w, int tile_h, int& x_from, int& x_to, int& y_from, int& y_to, int& x_start, int& y_start) const;

        TilesetCollection* m_ts_collection;
        unsigned m_width;   // Measured in tiles
        unsigned m_height;

        std::vector<std::vector<Uint32> > m_map_grid; ///< The actual map layer information
};
}} // namespace salmon::internal

#endif // MAP_LAYER_HPP_INCLUDED
