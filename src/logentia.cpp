#include "../inc/logentia.hpp"

int main(int argc, char** argv) {
    InitConfig();

    int opt;
    int HelpFlag = 0;
    int VersionFlag = 0;
    std::string Type = "";

    // Get opt option defenitions
    struct option Opts[] = {
        { "help", no_argument, &HelpFlag, 1 },
        { "version", no_argument, &VersionFlag, 1 },
        { "type", required_argument, NULL, 't' },
        { 0 }
    };

    // Infinite loop, to be broken when we are done parsing options
    while (1) {
        opt = getopt_long(argc, argv, "hvt:", Opts, 0);

        // A return value of -1 indicates that there are no more options
        if (opt == -1) {
            if(HelpFlag && VersionFlag) {
                Usage();;
                return EXIT_FAILURE;
            }
            break;
        } 

        switch (opt) {
        case 'h':
            HelpFlag = 1;
            break;
        case 'v':
            VersionFlag = 1;
            break;
        case 't':
            Type = optarg;
            break;
        case '?':
            Usage();
            return EXIT_FAILURE;
        default:
            Usage();
            break;
        }
    }

    // Act on flags
    if(HelpFlag) {
        Usage();
        return EXIT_SUCCESS;
    }
    if(VersionFlag) {
        PrintVersion();
        return EXIT_SUCCESS;
    }

    return 0;
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

