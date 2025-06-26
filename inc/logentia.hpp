#ifndef K_LOGENTIA
#define K_LOGENTIA

#include "config.hpp"
#include <string_view>
#include <source_location>

namespace logentia {

    // ─────────── public log calls ───────────
    void log(std::string_view msg, std::string_view topic, int level);
    void time_log(std::string_view msg, std::string_view topic, int level);
    void detailed_log(std::string_view msg, std::string_view topic, int level,
                      const std::source_location& loc = std::source_location::current());

    // ─────── overloads with title + body ───────
    void log(std::string_view title, std::string_view body, std::string_view topic, int level);
    void time_log(std::string_view title, std::string_view body, std::string_view topic, int level);
    void detailed_log(std::string_view title, std::string_view body, std::string_view topic, int level,
                      const std::source_location& loc = std::source_location::current());

    // ─────────── optional helpers ───────────
    /// Give the *current* thread a human-friendly name (shown in every log line).
    void set_thread_name(const std::string& name);

    /// Start / stop the background writer explicitly (normally automatic).
    void init_async_writer();
    void shutdown_async_writer();
}

#endif /* K_LOGENTIA */

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

