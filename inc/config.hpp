#ifndef K_CONFIG_LOGENTIA
#define K_CONFIG_LOGENTIA

#include <std-k.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace logentia {
    namespace config {
        extern bool ToggleTopics;
        extern bool ToggleColour;
        extern bool ToggleTerminal;
        extern bool ToggleFile;
        extern bool AsyncMode;

        extern int MaxLevel;
        extern int  DetailLevel;
        extern std::string FilePath;

        extern int IndentSpaces;

        extern std::string ProjectName;
        extern std::vector<std::string> TopicList;
    }

    int Init();
}

#endif

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

