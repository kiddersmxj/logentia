#include "../inc/config.hpp"

int InitConfig() {
    // Load configuration file using the singleton Config instance
    if (!k::config::Config::getInstance().load(ConfigFilePath)) {
        std::cerr << "Failed to load config file: " << ConfigFilePath << std::endl;
        // Handle error as needed
        return 1;
    }

    KCONFIG_ARRAY_REQUIRED(ExampleArray, "example.array")
    KCONFIG_VAR_REQUIRED(ExampleString, "example.string")
    KCONFIG_VAR_REQUIRED(ExampleBool, "example.bool")
    KCONFIG_VAR(ExampleInt, "example.int", 10)

    return 0;
}

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

