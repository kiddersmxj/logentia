#include "../inc/logentia.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>

namespace logentia {

// ─────────────────────────────────────────────────────────────
//  Anonymous namespace – internal state
// ─────────────────────────────────────────────────────────────
namespace {

    // ── global sinks & mutexes
    std::mutex              sink_mtx;        // guards terminal + file writes
    std::ofstream           file_stream;
    bool                    file_ready = false;
    std::string             file_path;

    // ── async queue
    std::mutex                      q_mtx;
    std::condition_variable         q_cv;
    std::queue<std::pair<std::string,int>> q;   // line + level
    std::thread                     worker;
    std::atomic<bool>               running{false};
    std::once_flag                  start_flag;

    // ── thread-naming
    thread_local std::string tl_name;
    thread_local int         tl_numeric_id = 0;
    std::atomic<int>         numeric_counter{1};        // T1, T2, …

    // ───────────────────── helpers ──────────────────────
    std::string timestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto t   = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *gmtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }

    std::string level_tag(int lvl)
    {
        switch (lvl) {
            case 1: return "[ONE]";   case 2: return "[TWO]";
            case 3: return "[THREE]"; case 4: return "[FOUR]";
            case 5: return "[FIVE]";  default: return "[LOG]";
        }
    }

    std::string thread_label()
    {
        if (!tl_name.empty()) return tl_name;
        if (tl_numeric_id == 0) tl_numeric_id = numeric_counter.fetch_add(1);
        return "T" + std::to_string(tl_numeric_id);
    }

    void ensure_file_sink()
    {
        if (file_ready || !config::ToggleFile) return;
        try {
            std::filesystem::path dir = config::FilePath;
            dir /= config::ProjectName;
            std::filesystem::create_directories(dir);
            std::string stamp = timestamp();
            file_path = (dir / (stamp + "." + config::ProjectName + ".log")).string();
            file_stream.open(file_path, std::ios::out | std::ios::trunc);
            file_ready = true;
        } catch (const std::exception& e) {
            std::cerr << "[LOGENTIA] Unable to open file sink: " << e.what() << '\n';
            config::ToggleFile = false;
        }
    }

    std::string build_line(std::string_view msg,
                           std::string_view topic,
                           int              lvl,
                           const std::string* ts  = nullptr,
                           const std::source_location* loc = nullptr)
    {
        std::ostringstream out;
        out << level_tag(lvl) << ' ';
        if (ts) out << *ts << ' ';
        out << '[' << thread_label() << "] ";
        if (loc) {
            std::filesystem::path fp{loc->file_name()};
            out << ".../" << fp.parent_path().filename().string() << '/'
                << fp.filename().string() << ':' << loc->line()
                << " (" << loc->function_name() << ") ";
        }
        if (config::ToggleTopics) out << '<' << topic << "> ";
        out << msg << '\n';
        return out.str();
    }

    void emit_to_sinks(const std::string& line, int lvl)
    {
        // ── terminal
        if (config::ToggleTerminal) {
            if (config::ToggleColour) {
                const char* col =
                    (lvl==1) ? "\033[1;31m" : (lvl==2) ? "\033[1;35m" :
                    (lvl==3) ? "\033[1;33m" : (lvl==4) ? "\033[1;32m" :
                    (lvl==5) ? "\033[1;36m" : "\033[0m";
                std::cout << col << line.substr(0,6) << "\033[0m" << line.substr(6);
            } else {
                std::cout << line;
            }
            std::cout.flush();
        }

        // ── file
        if (config::ToggleFile) {
            ensure_file_sink();
            if (file_stream.is_open()) {
                file_stream << line;
                file_stream.flush();
            }
        }
    }

    // Async writer thread
    void writer_loop()
    {
        while (running.load()) {
            std::unique_lock<std::mutex> lk(q_mtx);
            q_cv.wait(lk, []{ return !q.empty() || !running.load(); });
            while (!q.empty()) {
                auto [ln,lvl] = std::move(q.front()); q.pop();
                lk.unlock();
                {
                    std::lock_guard<std::mutex> guard(sink_mtx);
                    emit_to_sinks(ln,lvl);
                }
                lk.lock();
            }
        }
        // flush leftovers
        std::lock_guard<std::mutex> guard(sink_mtx);
        while (!q.empty()) {
            auto [ln,lvl] = std::move(q.front()); q.pop();
            emit_to_sinks(ln,lvl);
        }
    }

    void start_async()
    {
        if (!config::AsyncMode) return;
        running = true;
        worker  = std::thread(writer_loop);
        std::atexit([]{
            if (running.load()) {
                running = false;
                q_cv.notify_all();
                if (worker.joinable()) worker.join();
            }
        });
    }

    void enqueue(const std::string& ln, int lvl)
    {
        {
            std::lock_guard<std::mutex> lk(q_mtx);
            q.emplace(ln,lvl);
        }
        q_cv.notify_one();
    }

    bool topic_allowed(std::string_view topic)
    {
        if (!config::ToggleTopics)          return true;
        if (config::TopicList.empty())      return true;
        for (const auto& t : config::TopicList)
            if (t == topic) return true;
        return false;
    }

} // anon

// ─────────────────────────────────────────────────────────────
//  Public helpers
// ─────────────────────────────────────────────────────────────
void init_async_writer()          { std::call_once(start_flag, start_async); }
void shutdown_async_writer()
{
    if (running.load()) {
        running = false;
        q_cv.notify_all();
        if (worker.joinable()) worker.join();
    }
}

// helper to decide what details to add
enum class Req { None, Time, Full };

inline bool want_time(Req r) {
    return r == Req::Time || r == Req::Full || config::DetailLevel >= 1;
}
inline bool want_loc(Req r) {
    return r == Req::Full || config::DetailLevel >= 2;
}

std::string format_body(std::string_view title, std::string_view body) {
    std::ostringstream oss;
    oss << title << '\n';
    std::string indent(config::IndentSpaces, ' ');
    std::istringstream iss((std::string(body)));
    std::string line;
    while (std::getline(iss, line)) {
        oss << indent << line << '\n';
    }
    return oss.str();
}

// ─────────────────────────────────────────────────────────────
//  Front-end API
// ─────────────────────────────────────────────────────────────

void set_thread_name(const std::string& name) { tl_name = name; }

// ─────────────────────────────────────────────────────────────
//  Overloads with title + body
// ─────────────────────────────────────────────────────────────

void log(std::string_view msg, std::string_view topic, int lvl) {
    if (lvl > config::MaxLevel || !topic_allowed(topic)) return;

    const bool want_ts  = config::DetailLevel >= 1;
    const bool want_loc = config::DetailLevel >= 2;

    std::string ts;
    if (want_ts) ts = timestamp();

    std::source_location loc = std::source_location::current();
    const std::source_location* loc_ptr = want_loc ? &loc : nullptr;

    const std::string line = build_line(msg, topic, lvl,
                                        want_ts ? &ts : nullptr,
                                        loc_ptr);

    if (config::AsyncMode) {
        init_async_writer();
        enqueue(line, lvl);
    } else {
        std::lock_guard<std::mutex> g(sink_mtx);
        emit_to_sinks(line, lvl);
    }
}

void time_log(std::string_view msg, std::string_view topic, int lvl) {
    if (lvl > config::MaxLevel || !topic_allowed(topic)) return;

    const bool want_loc = config::DetailLevel >= 2;

    const std::string ts = timestamp();

    std::source_location loc = std::source_location::current();
    const std::source_location* loc_ptr = want_loc ? &loc : nullptr;

    const std::string line = build_line(msg, topic, lvl, &ts, loc_ptr);

    if (config::AsyncMode) {
        init_async_writer();
        enqueue(line, lvl);
    } else {
        std::lock_guard<std::mutex> g(sink_mtx);
        emit_to_sinks(line, lvl);
    }
}

void detailed_log(std::string_view msg, std::string_view topic, int lvl,
                  const std::source_location &loc) {
    if (lvl > config::MaxLevel || !topic_allowed(topic)) return;

    const std::string ts = timestamp();
    const std::string line = build_line(msg, topic, lvl, &ts, &loc);

    if (config::AsyncMode) {
        init_async_writer();
        enqueue(line, lvl);
    } else {
        std::lock_guard<std::mutex> g(sink_mtx);
        emit_to_sinks(line, lvl);
    }
}

void log(std::string_view title, std::string_view body, std::string_view topic, int lvl) {
    const std::string full = format_body(title, body);
    log(full, topic, lvl);  // reuse single-line version
}

void time_log(std::string_view title, std::string_view body, std::string_view topic, int lvl) {
    const std::string full = format_body(title, body);
    time_log(full, topic, lvl);
}

void detailed_log(std::string_view title, std::string_view body, std::string_view topic, int lvl,
                  const std::source_location& loc) {
    const std::string full = format_body(title, body);
    detailed_log(full, topic, lvl, loc);
}

} // namespace logentia

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

