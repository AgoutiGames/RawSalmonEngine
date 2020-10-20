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
#ifndef DATA_BLOCK_REF_HPP_INCLUDED
#define DATA_BLOCK_REF_HPP_INCLUDED

#include <string>

namespace salmon {

namespace internal{class DataBlock;}

class DataBlock {
    public:
        DataBlock(internal::DataBlock& impl);

        /// Set named bool property to supplied value
        void set_val(std::string name, bool val);
        /// Set named integer property to supplied value
        void set_val(std::string name, int val);
        /// Set named float property to supplied value
        void set_val(std::string name, float val);
        /// Set named string property to supplied value
        void set_val(std::string name, std::string val);

        /// Returns true if bool property with supplied name exists
        bool check_val_bool(std::string name) const;
        /// Returns true if integer property with supplied name exists
        bool check_val_int(std::string name) const;
        /// Returns true if float property with supplied name exists
        bool check_val_float(std::string name) const;
        /// Returns true if string property with supplied name exists
        bool check_val_string(std::string name) const;

        /// Return bool value by name
        bool get_val_bool(std::string name) const;
        /// Return integer value by name
        int get_val_int(std::string name) const;
        /// Return float value by name
        float get_val_float(std::string name) const;
        /// Return string value by name
        std::string get_val_string(std::string name) const;

        /// Clear DataBlock of all of its entries
        void clear();
    private:
        internal::DataBlock* m_impl;
};
}

#endif // DATA_BLOCK_REF_HPP_INCLUDED
