#include "server.h"
#include <thread>
#include <vector>

int main() {
    // Create threads
    std::vector<std::thread> threads;
    // Create server
    auto server = server::new_server();
    // Read data from files
    server.read_data();
    // Setup server
    server.setup(68000u);
    // Wait for connections
    constexpr auto max_connections = 3;
    for (auto i = 0; i < max_connections; ++i) {
        auto &client = server.accept_call();
        threads.emplace_back([&server, &client] { server.main_call(client); });
    }
    // Join threads
    for (auto &thread: threads) {
        thread.join();
    }
    // Save data to files
    server.save_data();
    return 0;
}
