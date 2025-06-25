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
        extern std::string FilePath;
        extern std::string ProjectName;
        extern std::vector<std::string> TopicList;
    }

    int Init();
}

const std::string ProgramName = "logentia";
const std::string Version = "0.1.0";
const std::string UsageNotes = R"(usage: logentia [ -h/-v ]
options:
    -h / --help         show help and usage notes
    -v / --version      print version and exit)";

void Usage();
void Usage(std::string Message);
void PrintVersion();

#endif

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

