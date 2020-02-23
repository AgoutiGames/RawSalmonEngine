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
#ifndef DATA_BLOCK_HPP_INCLUDED
#define DATA_BLOCK_HPP_INCLUDED

#include <string>
#include <map>

/**
 * @brief A class for holding user values by string
 */
class DataBlock{
    public:
        // Getters and setters for the custom data blocks
        void set_val(std::string name, bool val);
        void set_val(std::string name, int val);
        void set_val(std::string name, float val);
        void set_val(std::string name, std::string val);
        //void set_val(std::string name, Actor& val);

        bool check_val_bool(std::string name) const;
        bool check_val_int(std::string name) const;
        bool check_val_float(std::string name) const;
        bool check_val_string(std::string name) const;
        //Actor& check_val_actor(std::string name);

        bool get_val_bool(std::string name) const;
        int get_val_int(std::string name) const;
        float get_val_float(std::string name) const;
        std::string get_val_string(std::string name) const;
        //Actor& get_val_actor(std::string name);

        void clear();

    private:
        std::map<std::string, bool> m_data_bool;
        std::map<std::string, int> m_data_int;
        std::map<std::string, float> m_data_float;
        std::map<std::string, std::string> m_data_string;
};


#endif // DATA_BLOCK_HPP_INCLUDED
