#ifndef K_LOGENTIA
#define K_LOGENTIA

#include "config.hpp"

#include <string_view>
#include <source_location>

namespace logentia {
    void log(std::string_view msg, std::string_view topic, int level);
    void time_log(std::string_view msg, std::string_view topic, int level);
    void detailed_log(std::string_view msg, std::string_view topic, int level,
                      const std::source_location& loc = std::source_location::current());
}

#endif

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

