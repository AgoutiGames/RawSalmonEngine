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
#ifndef IMAGE_LAYER_HPP_INCLUDED
#define IMAGE_LAYER_HPP_INCLUDED

#include <vector>
#include <map>
#include <string>

#include "map/layer.hpp"
#include "graphics/texture.hpp"

namespace salmon { namespace internal {

/**
 * @brief A map layer which consist of an image
 */
class ImageLayer : public Layer{
    public:

        bool render(const Camera& camera) const override;

        LayerType get_type() override {return LayerType::image;}

        static ImageLayer* parse(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult);

        ImageLayer(const ImageLayer& other) = delete;
        ImageLayer& operator=(const ImageLayer& other) = delete;

        ImageLayer(ImageLayer&& other) = default;
        ImageLayer& operator=(ImageLayer&& other) = default;

    protected:
        ImageLayer(tinyxml2::XMLElement* source, std::string name, LayerCollection* layer_collection, tinyxml2::XMLError& eresult);

    private:
        tinyxml2::XMLError init(tinyxml2::XMLElement* source);

        std::string m_img_src;
        Texture m_img;
        float m_opacity = 1.0f; ///< @warning value only works with image layers!
};
}} // namespace salmon::internal

#endif // IMAGE_LAYER_HPP_INCLUDED
