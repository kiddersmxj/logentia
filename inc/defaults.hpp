#pragma once

#include <string_view>
#include <array>

namespace logentia::defaults {
    constexpr bool ToggleTopics   = true;
    constexpr bool ToggleColour   = true;
    constexpr bool ToggleTerminal = true;
    constexpr bool ToggleFile     = true;

    constexpr int MaxLevel = 3;
    constexpr std::string_view ProjectName = "hamza_assistant";

    constexpr std::string_view FilePath = "/log/hamza_assistant";

    constexpr std::array<std::string_view, 3> TopicList = {
        "BLE",
        "SENSOR",
        "INIT"
    };
}
