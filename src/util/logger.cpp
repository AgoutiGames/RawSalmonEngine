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

#include "util/logger.hpp"

#include <iomanip>
#include <chrono>
#include <ctime>
#include <SDL.h>

namespace salmon { namespace internal {

#ifndef __EMSCRIPTEN__
    // Define static vars
    const char* Logger::s_log_filename = "log.txt";
    std::ofstream Logger::s_logfile = Logger::open_log();
#endif

/// If buffer isn't empty upon destruction, flush it
Logger::~Logger() {
    if(!m_buffer.str().empty()) {
        // Set linebreak and flush
        (*this) << '\n';
        flush();
    }
}

#ifndef __EMSCRIPTEN__
    std::ofstream Logger::open_log() {
        std::ofstream logfile;

        // Make logfile local to the executable
        // Wrapping the call to GetBasePath in SDL_Init and SDL_Quit is preferred method in mailinglist
        SDL_Init(0);
        char* base_path_char = SDL_GetBasePath();
        std::string base_path;
        // If base path is not available, the current directory is used instead
        if(base_path_char != nullptr) {
            base_path = base_path_char;
            SDL_free(base_path_char);
        }
        SDL_Quit();

        logfile.open (base_path + s_log_filename, std::ios::app);
        logfile << "--------Start Logging--------\n";
        return logfile;
    }
#endif

/// Handle endl, flush, setw, setfill, etc.
Logger& Logger::operator<<(ManipFn manip) {
    manip(m_buffer);

    if (manip == static_cast<ManipFn>(std::flush) || manip == static_cast<ManipFn>(std::endl) ) {
        flush();
    }
    return *this;
}

/// Handle setiosflags, resetiosflags
Logger& Logger::operator<<(FlagsFn manip) {
    manip(m_buffer);
    return *this;
}

/// Set the level of the following log
Logger& Logger::operator<<(LogLevel level) {
    m_log_level = level;
    return *this;
}

/// Annotate and write contents of buffer to terminal and file
void Logger::flush() {
    std::ostream* target = nullptr;

    switch(m_log_level) {
        case info : {target = &std::cout; break;}
        default : {target = &std::cerr; break;}
    }
    std::string time = timestamp();
    std::string level = log_level();

    // Windows has problems with ANSI color codes
    #ifdef _WIN32
        // Write log to terminal
        (*target) << time << level << ' ' << m_buffer.str();
    #else
        // Additionally annotate by color
        switch(m_log_level) {
            case error : {(*target) << "\u001b[31m" ;break;} // Red color
            case warning : {(*target) << "\u001b[33m";break;} // Yellow color
            case fatal : {(*target) << "\u001b[35m";break;} // Magenta Color
            default : {break;}
        }
        // Reset to default color after each message
        (*target) << time << level << ' ' << m_buffer.str() << "\u001b[0m";
    #endif

    #ifndef __EMSCRIPTEN__
        // Write log to log.txt
        s_logfile << time << level << ' ' << m_buffer.str();
    #endif

    // After each message reset the log level
    m_log_level = info;

    // Clear currently held message
    m_buffer.str(std::string());
    m_buffer.clear();
}

/// Returns string representation of current date and time up to microseconds
std::string Logger::timestamp() {
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    time_t start_time = std::chrono::system_clock::to_time_t(now);

    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto fraction = now - seconds;

    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(fraction);
    // Only show the first six digits
    int microseconds = ms.count();

    char timedisplay[40];
    const tm* buf = localtime(&start_time);
    std::stringstream container;
    if (strftime(timedisplay, sizeof(timedisplay), "%Y-%m-%d %H:%M:%S.", buf)) {
        container << "[" << timedisplay << std::setfill('0') << std::setw(6) << microseconds << "]";
    }
    return container.str();

}

/// Returns string representation of current LogLevel
std::string Logger::log_level() {
    switch(m_log_level) {
        case info : {return "[INFO ]";}
        case error : {return "[ERROR]";}
        case warning : {return "[WARN ]";}
        case fatal : {return "[FATAL]";}
        default : {return "[WHAT?]";}
    }
}

}} // namespace salmon::internal
