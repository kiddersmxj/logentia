#ifndef K_CONFIG_LOGENTIA
#define K_CONFIG_LOGENTIA

#include <iostream>
#include <std-k.hpp>

// Define configuration variables with or without default values using inline
inline std::vector<std::string> ExampleArray = {};
inline std::string ExampleString;
inline bool ExampleBool;
inline int ExampleInt;

const std::string ConfigFilePath = "config.conf";
// Function to initialize global configuration variables
int InitConfig();

const std::string ProgramName = "logentia";
const std::string Version = "0.0.0";
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

