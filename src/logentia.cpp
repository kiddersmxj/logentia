#include "../inc/logentia.hpp"
#include "../inc/config.hpp"
#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <queue>
#include <sstream>
#include <fstream>
#include <mutex>
#include <filesystem>
#include <iostream>

namespace logentia {
    namespace {
        // ───────────────────── infrastructure ──────────────────────
        std::mutex log_mutex;                 // guards terminal + file writes
        std::ofstream file_stream;
        std::string file_path;
        bool file_initialised = false;

        // Async queue
        std::mutex q_mtx;
        std::condition_variable q_cv;
        std::queue<std::pair<std::string,int>> q;  // line + level
        std::thread worker;
        std::atomic<bool> running{false};
        std::once_flag start_flag;

        // ───────────────────── helpers ──────────────────────
        std::string timestamp() {
            auto now = std::chrono::system_clock::now();
            auto t   = std::chrono::system_clock::to_time_t(now);
            std::tm tm = *gmtime(&t);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
            return oss.str();
        }

        std::string level_tag(int lvl) {
            switch (lvl) {
                case 1: return "[ONE]"; case 2: return "[TWO]"; case 3: return "[THREE]";
                case 4: return "[FOUR]"; case 5: return "[FIVE]"; default: return "[LOG]";
            }
        }

        std::string build_line(std::string_view msg, std::string_view topic, int lvl,
                                const std::string *ts=nullptr,
                                const std::source_location *loc=nullptr) {
            std::ostringstream out;
            out << level_tag(lvl) << ' ';
            if (ts)  out << *ts << ' ';
            if (loc) {
                std::filesystem::path fp{loc->file_name()};
                out << ".../" << fp.parent_path().filename().string() << '/' << fp.filename().string()
                    << ':' << loc->line() << " (" << loc->function_name() << ") ";
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
            } catch (const std::exception &e) {
                std::cerr << "[LOGENTIA] Failed to create log file: " << e.what() << "\n";
                config::ToggleFile = false;
            }
        }

        void emit_line_internal(const std::string &line, int lvl) {
            // terminal sink
            if (config::ToggleTerminal) {
                if (config::ToggleColour) {
                    std::string col;
                    switch (lvl) {
                        case 1: col="\033[1;31m"; break; case 2: col="\033[1;35m"; break;
                        case 3: col="\033[1;33m"; break; case 4: col="\033[1;32m"; break;
                        case 5: col="\033[1;36m"; break; default: col="\033[0m"; break;
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

        // Background writer thread function
        void writer_loop() {
            while (running.load()) {
                std::unique_lock<std::mutex> lk(q_mtx);
                q_cv.wait(lk, []{ return !q.empty() || !running.load(); });
                while (!q.empty()) {
                    auto [line,lvl] = std::move(q.front());
                    q.pop();
                    lk.unlock();
                    {
                        std::lock_guard<std::mutex> g(log_mutex);
                        emit_line_internal(line,lvl);
                    }
                    lk.lock();
                }
            }
            // flush remaining
            std::lock_guard<std::mutex> g(log_mutex);
            while (!q.empty()) {
                auto [line,lvl] = std::move(q.front()); q.pop();
                emit_line_internal(line,lvl);
            }
        }

        void start_async() {
            if (!config::AsyncMode) return;
            running = true;
            worker = std::thread(writer_loop);
            std::atexit([]{
                if (running.load()) {
                    running = false;
                    q_cv.notify_all();
                    if (worker.joinable()) worker.join();
                }
            });
        }

        inline void enqueue(const std::string &line,int lvl) {
            std::lock_guard<std::mutex> lk(q_mtx);
            q.emplace(line,lvl);
            q_cv.notify_one();
        }

        bool topic_enabled(std::string_view topic) {
            if (!config::ToggleTopics) return true;
            if (config::TopicList.empty()) return true;
            for (const auto &t : config::TopicList) if (t == topic) return true;
            return false;
        }
    } // anonymous

    // ─────────────────────────────────────────────────────────────
    //  Public helpers
    // ─────────────────────────────────────────────────────────────
    void init_async_writer() { std::call_once(start_flag, start_async);}  
    void shutdown_async_writer() {
        if (running.load()) {
            running = false;
            q_cv.notify_all();
            if (worker.joinable()) worker.join();
        }
    }

    // ─────────────────────────────────────────────────────────────
    //  Front‑end API
    // ─────────────────────────────────────────────────────────────
    void log(std::string_view msg, std::string_view topic, int lvl) {
        if (lvl > config::MaxLevel || !topic_enabled(topic)) return;
        const std::string line = build_line(msg, topic, lvl);
        if (config::AsyncMode) {
            init_async_writer();
            enqueue(line,lvl);
        } else {
            std::lock_guard<std::mutex> g(log_mutex);
            emit_line_internal(line,lvl);
        }
    }

    void time_log(std::string_view msg, std::string_view topic, int lvl) {
        if (lvl > config::MaxLevel || !topic_enabled(topic)) return;
        const std::string ts = timestamp();
        const std::string line = build_line(msg, topic, lvl, &ts);
        if (config::AsyncMode) { init_async_writer(); enqueue(line,lvl);} else {
            std::lock_guard<std::mutex> g(log_mutex); emit_line_internal(line,lvl);
        }
    }

    void detailed_log(std::string_view msg, std::string_view topic, int lvl,
                      const std::source_location &loc) {
        if (lvl > config::MaxLevel || !topic_enabled(topic)) return;
        const std::string ts = timestamp();
        const std::string line = build_line(msg, topic, lvl, &ts, &loc);
        if (config::AsyncMode) {
            init_async_writer();
            enqueue(line, lvl);
        } else {
            std::lock_guard<std::mutex> g(log_mutex);
            emit_line_internal(line, lvl);
        }
    }
}

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

