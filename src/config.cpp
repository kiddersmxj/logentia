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
    int  DetailLevel   = 0;

    int IndentSpaces = 4;

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
        KCONFIG_VAR(config::DetailLevel, "general.detail_level", config::DetailLevel);
        KCONFIG_VAR(config::FilePath,  "general.file_path", config::FilePath);

        // [formatting]
        KCONFIG_VAR(config::IndentSpaces, "formatting.indents", config::IndentSpaces);

        // [project]
        KCONFIG_VAR(config::ProjectName, "project.name", config::ProjectName);

        // [list]
        KCONFIG_ARRAY(config::TopicList, "list.topics", config::TopicList);

        return 0;
    }
} // namespace logentia
