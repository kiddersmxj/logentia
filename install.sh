#!/bin/bash

if [[ $1 != "" ]]; then
    ARGS="-D$1"
fi

mkdir -p build
script -q -c "cmake -B build $ARGS && cmake --build build && sudo cmake --install build" | tee build/build.log


# Copyright (c) 2024, Maxamilian Kidd-May
# All rights reserved.

# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree. 

