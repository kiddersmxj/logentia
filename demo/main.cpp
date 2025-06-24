#include <logentia.hpp>

#include <iostream>

int main()
{
    // One-liner to read logentia.conf (falls back to defaults if absent)
    logentia::Init();

    // std::cerr << "Terminal   : " << logentia::config::ToggleTerminal << '\n'
    //           << "Colour     : " << logentia::config::ToggleColour   << '\n'
    //           << "MaxLevel   : " << logentia::config::MaxLevel       << '\n'
    //           << "Topics ON  : " << logentia::config::ToggleTopics   << '\n'
    //           << "Whitelist  : ";
    // for (auto& t : logentia::config::TopicList) std::cerr << t << ' ';
    // std::cerr << '\n';

    // Will pass (level ≤ MaxLevel && topic enabled)
    logentia::log("System booting …", "CORE", 3);

    // Shows timestamp
    logentia::time_log("Initialising BLE stack", "BLE", 4);

    // Shows timestamp + file + line + function
    logentia::detailed_log("Sensor value out of range", "SENSOR", 2);

    // Debug spam (level 5) – only appears if MaxLevel ≥ 5
    logentia::log("Verbose trace line", "DEBUG", 5);

    return 0;
}
