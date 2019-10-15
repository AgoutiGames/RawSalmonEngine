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

#include "util/logger.hpp"

#include <iomanip>
#include <chrono>
#include <ctime>

std::ofstream Logger::m_logfile = Logger::open_log();

Logger::~Logger() {
    if(!m_buffer.str().empty()) {
        std::cerr << "!!Log message not sent!! : " << m_buffer.str() << "\n";
    }
}

std::ofstream Logger::open_log() {
    std::ofstream logfile;
    logfile.open ("log.txt", std::ios::app);
    logfile << "--------Start Logging--------\n";
    return logfile;
}

void Logger::close_log() {
    m_logfile.close();
}

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

void Logger::flush() {
    std::ostream* target = nullptr;

    switch(m_log_level) {
        case info : {target = &std::cout; break;}
        default : {target = &std::cerr; break;}
    }
    std::string time = timestamp();
    std::string level = log_level();
    // Write log to terminal
    (*target) << time << level << ' ' << m_buffer.str();
    // Write log to log.txt
    m_logfile << time << level << ' ' << m_buffer.str();

    // After each message reset the log level
    m_log_level = info;

    // Clear currently held message
    m_buffer.str(std::string());
    m_buffer.clear();
}

std::string Logger::timestamp() {
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    time_t start_time = std::chrono::system_clock::to_time_t(now);

    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto fraction = now - seconds;

    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(fraction);
    // Only show the first five digits
    int microseconds = ms.count();

    char timedisplay[40];
    const tm* buf = localtime(&start_time);
    std::stringstream container;
    if (strftime(timedisplay, sizeof(timedisplay), "%F %H:%M:%S.", buf)) {
        container << "[" << timedisplay << std::setfill('0') << std::setw(6) << microseconds << "]";
    }
    return container.str();

}

std::string Logger::log_level() {
    switch(m_log_level) {
        case info : {return "[INFO ]";}
        case error : {return "[ERROR]";}
        case warning : {return "[WARN ]";}
        case fatal : {return "[FATAL]";}
        default : {return "[WHAT?]";}
    }
}
