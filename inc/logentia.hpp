#ifndef K_LOGENTIA
#define K_LOGENTIA

#include "config.hpp"
#include <atomic>
#include <string_view>
#include <source_location>

namespace logentia {

    void start();

    // ─────────── public log calls ───────────
    void log(std::string_view msg, std::string_view topic, int level);
    void time_log(std::string_view msg, std::string_view topic, int level);
    void detailed_log(std::string_view msg, std::string_view topic, int level,
                      const std::source_location& loc = std::source_location::current());

    // ─────── overloads with title + body ───────
    void log(std::string_view title, std::string_view body, std::string_view topic, int level);
    void time_log(std::string_view title, std::string_view body, std::string_view topic, int level);
    void detailed_log(std::string_view title, std::string_view body, std::string_view topic, int level,
                      const std::source_location& loc = std::source_location::current());

    // ─────────── optional helpers ───────────
    /// Give the *current* thread a human-friendly name (shown in every log line).
    void set_thread_name(const std::string& name);

    /// Start / stop the background writer explicitly (normally automatic).
    void init_async_writer();
    void shutdown_async_writer();
    class tapbuf : public std::streambuf {
        std::streambuf*  upstream_;
    public:
        explicit tapbuf(std::streambuf* real) : upstream_(real) {}
        static std::atomic<bool> last_was_nl;

    protected:
        int overflow(int ch) override {
            if (ch != EOF) {
                if (ch == '\n' || ch == '\r') last_was_nl = true;
                else            last_was_nl = false;
                upstream_->sputc(static_cast<char>(ch));
            }
            return ch;
        }
        int sync() override { return upstream_->pubsync(); }
    };

    inline std::atomic<bool> tapbuf::last_was_nl{true};

    /// Replace cout/cerr buffers with tapped ones (call once at program start).
    void hook_standard_streams();
}

#endif /* K_LOGENTIA */

// Copyright (c) 2024, Maxamilian Kidd-May
// All rights reserved.

// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree. 

