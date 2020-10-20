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
#ifndef TRANSFORM_LIB_HPP_INCLUDED
#define TRANSFORM_LIB_HPP_INCLUDED

#include "./types.hpp"

namespace salmon {

namespace internal{class Transform;}

class Transform {
    public:
        Transform(internal::Transform& t);

    private:
        internal::Transform* m_impl;
};
}

#endif // TRANSFORM_LIB_HPP_INCLUDED
