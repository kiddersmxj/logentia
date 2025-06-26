#include <logentia.hpp>

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <sstream>

void worker(std::size_t id, std::size_t iterations)
{
    const std::string topic = "THREAD";

    for (std::size_t i = 0; i < iterations; ++i)
    {
        logentia::time_log("Thread #" + std::to_string(id) +
                           " - Iteration " + std::to_string(i) + " starting", topic, 4);

        std::this_thread::sleep_for(std::chrono::milliseconds(100 + id * 50));

        logentia::detailed_log("Thread #" + std::to_string(id) +
                               " - Iteration " + std::to_string(i) + " done", topic, 3);
    }

    logentia::log("Thread #" + std::to_string(id) + " exiting", topic, 2);
}

void single_named_task()
{
    // Give this thread a custom name
    logentia::set_thread_name("Uploader");

    std::cout << "annoying output";

    const std::string topic = "SINGLE";
    logentia::log("Uploader thread started", topic, 3);

    for (int i = 0; i < 3; ++i)
    {
        logentia::time_log("Uploading chunk " + std::to_string(i + 1), topic, 4);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    logentia::detailed_log("Upload complete", topic, 2);
}

int main()
{
    logentia::Init();
    logentia::hook_standard_streams();

    logentia::log("System booting …", "CORE", 3);
    logentia::time_log("Initialising BLE stack", "BLE", 4);
    logentia::detailed_log("Sensor value out of range", "SENSOR", 2);
    logentia::log("Verbose trace line", "DEBUG", 5);

    logentia::log("Big Message:", "Very Long\nMulti-Paragraph\nMessage", "DEBUG", 3);

    constexpr std::size_t thread_count = 4;
    constexpr std::size_t iterations = 5;

    logentia::log("Spawning worker threads", "CORE", 3);

    std::vector<std::thread> pool;
    pool.reserve(thread_count);

    for (std::size_t t = 0; t < thread_count; ++t)
        pool.emplace_back(worker, t, iterations);

    // Start named thread for single action
    std::thread uploader(single_named_task);

    // Main thread log loop
    const std::string main_topic = "MAIN";
    for (std::size_t i = 0; i < iterations; ++i)
    {
        logentia::time_log("Main thread heartbeat " + std::to_string(i), main_topic, 4);
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
    }

    for (auto& th : pool) th.join();
    uploader.join();

    logentia::log("All threads joined; program terminating", "CORE", 3);
    return 0;
}
