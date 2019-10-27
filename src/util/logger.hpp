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
#ifndef LOGGER_HPP_INCLUDED
#define LOGGER_HPP_INCLUDED

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

/**
 * @brief A simple logger which annotates log messages and writes log additionally to file
 *
 * The calling interface is usually: "Logger(Logger::LogLevel) << message_string;" or
 *                                   "Logger(Logger::LogLevel) << message_string << std::endl;"
 *
 * @note The log message gets either sent when passing std::endl or upon destruction
 *       of the usually temporary Logger object
 */
class Logger {
public:

    typedef std::ostream&  (*ManipFn)(std::ostream&);
    typedef std::ios_base& (*FlagsFn)(std::ios_base&);

    enum LogLevel {
        info,       /// mostly startup messages
        warning,    /// situations that may lead to errors further on
        error,      /// runtime errors
        fatal,      /// application must stop
    };

    Logger() = default;
    Logger(LogLevel level) : m_log_level{level} {}

    ~Logger();

    Logger(Logger&& other) = default;
    Logger& operator=(Logger&& other) = default;

    template <typename T>
    Logger& operator<<(const T& input) {
        m_buffer << input;
        return *this;
    }

    Logger& operator<<(ManipFn manip);
    Logger& operator<<(FlagsFn manip);
    Logger& operator<<(LogLevel level);

    void flush();
    std::string timestamp();

    std::string log_level();

private:
    LogLevel m_log_level = info;
    std::stringstream m_buffer;

    static std::ofstream open_log();
    static std::ofstream s_logfile;
    static const char* s_log_filename;
};


#endif // LOGGER_HPP_INCLUDED
