#include "../inc/config.hpp"
#include <iostream>
#include <std-k.hpp>

namespace logentia {
    namespace config {

    // —— compile‑time defaults ——
    bool ToggleTopics   = true;
    bool ToggleColour   = true;
    bool ToggleTerminal = true;
    bool ToggleFile     = true;
    bool AsyncMode      = true;

    int MaxLevel = 3;
    std::string FilePath    = "/log";                // root directory
    std::string ProjectName = "logentia_project";     // used if conf missing
    std::vector<std::string> TopicList;               // empty ⇒ all topics
    // ————————————————

    } // namespace config

    int Init() {
        const std::string ProjectConfig = "logentia.conf";
        auto& cfg = k::config::Config::getInstance();

        if (!cfg.load(ProjectConfig)) {
            std::cerr << "[LOGENTIA] Could not load '" << ProjectConfig
                      << "'; using built‑in defaults.\n";
            return 1;
        }

        // [toggle]
        KCONFIG_VAR(config::ToggleTopics,   "toggle.topics",   config::ToggleTopics);
        KCONFIG_VAR(config::ToggleColour,   "toggle.colour",   config::ToggleColour);
        KCONFIG_VAR(config::ToggleTerminal, "toggle.terminal", config::ToggleTerminal);
        KCONFIG_VAR(config::ToggleFile,     "toggle.file",     config::ToggleFile);
        KCONFIG_VAR(config::AsyncMode,      "toggle.async",    config::AsyncMode);

        // [general]
        KCONFIG_VAR(config::MaxLevel,  "general.max_level", config::MaxLevel);
        KCONFIG_VAR(config::FilePath,  "general.file_path", config::FilePath);

        // [project]
        KCONFIG_VAR(config::ProjectName, "project.name", config::ProjectName);

        // [list]
        KCONFIG_ARRAY(config::TopicList, "list.topics", config::TopicList);

        return 0;
    }
} // namespace logentia

void Usage() {
    std::cout << UsageNotes << std::endl;
}

void Usage(std::string Message) {
    std::cout << Message << std::endl;
    std::cout << UsageNotes << std::endl;
}

void PrintVersion() {
    std::cout << ProgramName << ": version " << Version << std::endl;
}

