#include "client.h"

#include <iostream>
#include <regex>

#include "handler.h"
#include "messages.h"

bool validate_ip(const std::string &ip_address) {
    const auto re = std::regex(
        R"(^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$)");
    return std::regex_match(ip_address, re);
}

client::client(const int server): server_(server) {
    if (server_ == INVALID_SOCKET) {
        throw std::runtime_error("Could not create socket.");
    }
}

client::~client() {
#if defined(_WIN32) || defined(_WIN64)
     closesocket(server_);
     WSACleanup();
#elif defined(__linux__) || defined(__APPLE__)
    close(server_);
#endif
}

client client::new_client() {
#  if defined(_WIN32) || defined(_WIN64)
        WSADATA wsa;
        SetConsoleCP(CP_UTF8);
        std::println("Initializing Winsock...");
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            std::cerr << "Failed. Error Code : " << WSAGetLastError() << '\n';
            exit(EXIT_FAILURE);
        }
        std::println("Initialized.");
#endif
    const auto server_socket = socket(AF_INET, SOCK_STREAM, 0);
    std::println("Socket created.");
    return client{server_socket};
}

void client::connect(const std::string &ip_addr) const {
    // create the socket address (IP address and port)
    constexpr auto test_port = 68000u;
    auto server_addr = sockaddr_in{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(test_port);
    inet_pton(AF_INET, ip_addr.c_str(), &server_addr.sin_addr.s_addr);
    // Connect to remote server
    if (::connect(server_, reinterpret_cast<sockaddr *>(&server_addr), sizeof server_addr) == SOCKET_ERROR) {
        throw std::runtime_error("Could not connect to server.");
    }
    std::println("Connected to server.\n");
    // Send HELLO message
    handler::send(server_, hello_request{});
    // Receive HELLO message
    const auto reply = handler::receive<hello_reply>(server_);
    std::println("Server said {}.", reply.content());
}

std::string client::pick_location() const {
    // Send request for available locations
    handler::send(server_, locations_request{});
    // Receive available locations
    const auto reply = handler::receive<locations_reply>(server_);
    const auto &locations = reply.locations();
    // Pick location
    int option;
    do {
        std::println("Available locations:");
        for (const auto [i, location]: enumerate(locations)) {
            std::println("\t{} -> {}", i, location);
        }
        std::print("Please choose a location: ");
        std::cin >> option;
        std::cin.ignore();
    } while (option < 0 || option >= locations.size());
    return locations[option];
}

std::string client::pick_genre(const std::string &location) const {
    // Send request for available genres
    handler::send(server_, genres_request{location});
    // Receive available genres
    const auto reply = handler::receive<genres_reply>(server_);
    const auto &genres = reply.genres();
    // Pick genre
    int option;
    do {
        std::println("Available genres:");
        for (const auto [i, genre]: enumerate(genres)) {
            std::println("\t{} -> {}", i, genre);
        }
        std::cout << "Option: ";
        std::cin >> option;
        std::cin.ignore();
    } while (option >= 0 && option < genres.size());
    return genres[option];
}

std::optional<std::pair<std::string, show> > client::pick_show() const {
    // Check no of shows
    if (shows_by_theater_.empty()) {
        std::println("No available shows.");
        return std::nullopt;
    }
    // Pick show from available shows
    std::println("Available shows:");
    for (const auto &[theater, shows]: shows_by_theater_) {
        std::println("Theater: {}", theater);
        for (const auto &show: shows) {
            show.write();
        }
        std::cout << std::endl;
    }

    std::println("Pick a theater to buy tickets.");
    std::print("Theater: ");
    std::string theater_name;
    std::getline(std::cin, theater_name);
    if (!shows_by_theater_.contains(theater_name)) {
        std::println("Invalid theater name.");
        return std::nullopt;
    }
    std::println("What show would you like to buy tickets for?");
    std::print("Show ID: ");
    int id;
    std::cin >> id;
    std::cin.ignore();

    const auto show_iter = std::ranges::find_if(shows_by_theater_.at(theater_name),
                                                [&](const show &s) { return s.id() == id; });
    if (show_iter == shows_by_theater_.at(theater_name).end()) {
        std::println("No show with id {} is available.", id);
        return std::nullopt;
    }
    return std::make_pair(theater_name, *show_iter);
}

int client::pick_no_tickets(const show &show) {
    // Ask how many tickets the client wants
    int no_tickets = 0;
    std::println("Available seats: {}", show.available_seats());
    std::print("How many tickets? ");
    (std::cin >> no_tickets).ignore();
    if (no_tickets <= 0) {
        std::println("Invalid number of tickets.");
        return no_tickets;
    }
    if (no_tickets > show.available_seats()) {
        std::println("Not enough tickets available.");
        return no_tickets;
    }
    return no_tickets;
}

void client::buy_tickets() {
    // Pick location
    const auto location = pick_location();
    // Pick genre
    const auto genre = pick_genre(location);
    // Ask for shows
    handler::send(server_, shows_request{location, genre});
    // Parse response and get shows
    const auto reply = handler::receive<shows_reply>(server_);
    shows_by_theater_ = reply.shows();
    // Pick show
    const auto pair = pick_show();
    if (!pair.has_value()) {
        return;
    }
    const auto [theater, show] = pair.value();
    const auto no_tickets = pick_no_tickets(show);
    if (no_tickets <= 0) {
        return;
    }
    // Send show/ticket info
    shows_by_theater_.clear(); // Make sure shows is cleared
    handler::send(server_, tickets_request{theater, show.id(), no_tickets});
    // Receive response
    if (const auto response = handler::receive<tickets_reply>(server_); !response.success()) {
        std::println("Failed to buy tickets. Reason: {}", response.error());
        return;
    }
    std::println("\"{}\" at {} has been successfully bought.", show.name(), theater);
    std::println("Number of tickets bought: {}", no_tickets);
}

void client::menu() {
    // Main menu
    int option;
    std::println("Welcome to the ticket buying system.");
    std::println("Please choose an option:");
    std::println("\t1 -> Buy tickets");
    std::println("\t2 -> Quit");
    std::print("Option: ");
    std::cin >> option;
    std::cin.ignore();
    switch (option) {
        case 1:
            buy_tickets();
            break;
        case 2:
            quit();
        default:
            std::println("Invalid option. Please try again.");
            break;
    }
}

void client::quit() const {
    // Confirm quit
    std::println("Are you sure you want to quit?");
    std::println("All unsaved data will be lost.");
    std::print("Please choose an option (y/n): ");
    char option;
    std::cin >> option;
    std::cin.ignore();
    if (option != 'y') {
        return;
    }
    std::println("Thank you for using the ticket buying system.");
    std::println("Goodbye.");
    // Send request to end call
    handler::send(server_, quit_request{});
    // Receive response to end call
    const auto reply = handler::receive<quit_reply>(server_);
    std::println("Server: {}", reply.content());
    exit(EXIT_SUCCESS);
}
