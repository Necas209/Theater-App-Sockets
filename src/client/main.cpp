#include <iostream>
#include <print>

#include "client.h"
#include "socket_error.h"

int main() {
    // Create a socket
    auto client = client::new_client();
    // Ask client for server IP address
    std::string ip_addr;
    std::print("Please enter the server's IP address: ");
    std::getline(std::cin, ip_addr);

    if (!validate_ip(ip_addr)) {
        ip_addr = "172.30.217.49";
        std::println("Invalid IP address.");
        std::println("IP address will default to {}.", ip_addr);
    }
    // Connect to server
    client.connect(ip_addr);
    // Main menu
    while (true) {
        try {
            client.menu();
        } catch (const socket_error &e) {
            std::println("An error occurred: {}.", e.what());
            break;
        }
    }
    return 0;
}
