#include "../inc/logentia.hpp"
#include "../inc/config.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <mutex>
#include <filesystem>
#include <iostream>

namespace logentia {
    namespace {
        std::mutex log_mutex;
        std::ofstream file_stream;
        std::string file_path;
        bool file_initialised = false;

        std::string timestamp() {
            auto now  = std::chrono::system_clock::now();
            auto t    = std::chrono::system_clock::to_time_t(now);
            std::tm tm = *gmtime(&t);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
            return oss.str();
        }

        std::string level_tag(int lvl) {
            switch (lvl) {
                case 1: return "[ONE]";   // critical
                case 2: return "[TWO]";   // error
                case 3: return "[THREE]"; // warn / info
                case 4: return "[FOUR]";  // detail
                case 5: return "[FIVE]";  // debug
                default: return "[LOG]";
            }
        }

        std::string build_line(std::string_view msg,
                                std::string_view topic,
                                int lvl,
                                const std::string* ts = nullptr,
                                const std::source_location* loc = nullptr) {
            std::ostringstream out;
            out << level_tag(lvl) << ' ';
            if (ts)  out << *ts << ' ';
            if (loc) {
                std::filesystem::path fp{loc->file_name()};
                std::string parent = fp.parent_path().filename().string();
                std::string file   = fp.filename().string();
                out << ".../" << parent << "/" << file << ':' << loc->line()
                    << " (" << loc->function_name() << ") ";
            }
            if (config::ToggleTopics) out << '<' << topic << "> ";
            out << msg << '\n';
            return out.str();
        }

        void init_file() {
            if (file_initialised || !config::ToggleFile) return;
            try {
                const std::string stamp = timestamp();
                std::filesystem::path dir = config::FilePath;
                dir /= config::ProjectName;
                std::filesystem::create_directories(dir);
                file_path = (dir / (stamp + "." + config::ProjectName + ".log")).string();
                file_stream.open(file_path, std::ios::out | std::ios::trunc);
                file_initialised = true;
            } catch (const std::exception& e) {
                std::cerr << "[LOGENTIA] Failed to create log file: " << e.what()
                          << ". Falling back to terminal‑only.\n";
                config::ToggleFile = false;
            }
        }

        void emit_line(const std::string& line, int lvl) {
            std::lock_guard<std::mutex> g(log_mutex);

            // terminal sink
            if (config::ToggleTerminal) {
                if (config::ToggleColour) {
                    std::string col;
                    switch (lvl) {
                        case 1: col = "\033[1;31m"; break; // red
                        case 2: col = "\033[1;35m"; break; // magenta
                        case 3: col = "\033[1;33m"; break; // yellow
                        case 4: col = "\033[1;32m"; break; // green
                        case 5: col = "\033[1;36m"; break; // cyan
                        default: col = "\033[0m";
                    }
                    std::cout << col << line.substr(0,6) << "\033[0m" << line.substr(6);
                } else {
                    std::cout << line;
                }
                std::cout.flush();
            }

            // file sink
            if (config::ToggleFile) {
                init_file();
                if (file_stream.is_open()) {
                    file_stream << line;
                    file_stream.flush();
                }
            }
        }

        bool topic_enabled(std::string_view topic) {
            if (!config::ToggleTopics) return true;
            if (config::TopicList.empty()) return true;
            for (const auto& t : config::TopicList) if (t == topic) return true;
            return false;
        }
    } // anonymous ns

    void log(std::string_view msg, std::string_view topic, int lvl) {
        if (lvl > config::MaxLevel || !topic_enabled(topic)) return;
        emit_line(build_line(msg, topic, lvl), lvl);
    }

    void time_log(std::string_view msg, std::string_view topic, int lvl) {
        if (lvl > config::MaxLevel || !topic_enabled(topic)) return;
        const std::string ts = timestamp();
        emit_line(build_line(msg, topic, lvl, &ts), lvl);
    }

    void detailed_log(std::string_view msg, std::string_view topic, int lvl,
                       const std::source_location& loc) {
        if (lvl > config::MaxLevel || !topic_enabled(topic)) return;
        const std::string ts = timestamp();
        emit_line(build_line(msg, topic, lvl, &ts, &loc), lvl);
    }
} // namespace logentia

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

