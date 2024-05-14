#include "utils.h"

std::list<show> shows;

bool validate_ip(const std::string &ip_address) {
    const std::regex re(R"(^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\.){3})"
                        "([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$");
    return std::regex_match(ip_address, re);
}

int pick_location(const SOCKET &server_socket, std::string &location) {
    // Send request for available locations
    const json j = message(code::get_locations, "");
    const auto msg = j.dump();
    int ret_val = send(server_socket, msg.data(), static_cast<int>(msg.length()) + 1, 0);
    if (ret_val <= 0) return SOCKET_ERROR;
    // Receive available locations
    char reply[2000];
    ret_val = recv(server_socket, reply, 2000, 0);
    if (ret_val <= 0) return SOCKET_ERROR;
    // Parse response
    auto msg2 = json::parse(reply).get<message>();
    std::set<std::string> locations;
    json::parse(msg2.content).get_to(locations);
    // Pick location
    const auto n = static_cast<int>(locations.size());
    int option = 0;
    do {
        std::cout << "Available locations:\n";
        int i = 0;
        std::ranges::for_each(locations,
                              [&](const auto &l) {
                                  std::cout << '\t' << i << " -> " << l << '\n';
                                  i++;
                              });
        std::cout << "Option: ";
        (std::cin >> option).ignore();
    } while (option < 0 || option >= n);
    location = *std::next(locations.begin(), option);
    return 0;
}

int pick_genre(const SOCKET &server_socket, const std::string &location, std::string &genre) {
    // Send request for available genres
    const json j = message(code::get_genres, location);
    const auto msg = j.dump();
    int ret_val = send(server_socket, msg.data(), static_cast<int>(msg.length()) + 1, 0);
    if (ret_val <= 0) return SOCKET_ERROR;
    // Receive available genres
    char reply[2000];
    ret_val = recv(server_socket, reply, 2000, 0);
    if (ret_val <= 0) return SOCKET_ERROR;
    // Parse response
    auto msg2 = json::parse(reply).get<message>();
    std::set<std::string> genres;
    json::parse(msg2.content).get_to(genres);
    // Pick genre
    const auto n = static_cast<int>(genres.size());
    int option = 0;
    do {
        std::cout << "Available genres:\n";
        int i = 0;
        std::ranges::for_each(genres, [&](const auto &g) {
            std::cout << '\t' << i << " -> " << g << '\n';
            i++;
        });
        std::cout << "Option: ";
        (std::cin >> option).ignore();
    } while (option < 0 || option >= n);
    genre = *std::next(genres.begin(), option);
    return 0;
}

std::pair<int, int> pick_show() {
    std::pair show_info(-1, -1);
    // Check no of shows
    if (shows.empty()) {
        std::cout << "No available shows.\n";
        return show_info;
    }
    // Pick show from available shows
    std::cout << "Available shows: \n";
    std::ranges::for_each(shows, [](const show &s) {
        s.write();
        std::cout << '\n';
    });
    int id;
    std::cout << "Choose show (id): ";
    (std::cin >> id).ignore();
    const auto it = std::ranges::find_if(shows,
                                         [&](const show &s) { return s.id == id; });
    if (it == shows.end()) {
        std::cout << "No show with id " << id << " is available.\n";
        return show_info;
    }
    show_info.first = id;
    // Ask how many tickets the client wants
    int no_tickets;
    std::cout << "How many tickets? ";
    (std::cin >> no_tickets).ignore();
    if (no_tickets <= 0) {
        if (no_tickets < 0)
            std::cout << "Invalid number of tickets.\n";
        return show_info;
    }
    if (no_tickets > (*it).available_seats) {
        std::cout << "Not enough tickets available.\n";
        return show_info;
    }
    show_info.second = no_tickets;
    return show_info;
}

int buy_tickets(const SOCKET &server_socket) {
    // Pick location
    std::string location;
    int ret_val = pick_location(server_socket, location);
    if (ret_val < 0) return ret_val;
    // Pick genre
    std::string genre;
    ret_val = pick_genre(server_socket, location, genre);
    if (ret_val < 0) return ret_val;
    // Ask for shows
    const json j = message(code::get_shows, json{{"location", location},
                                                 {"genre",    genre}}.dump());
    auto s = j.dump();
    ret_val = send(server_socket, s.data(), static_cast<int>(s.length()) + 1, 0);
    if (ret_val <= 0) return SOCKET_ERROR;
    // Parse response and get shows
    char reply[2000];
    ret_val = recv(server_socket, reply, 2000, 0);
    if (ret_val <= 0) return SOCKET_ERROR;
    auto msg = json::parse(reply).get<message>();
    json::parse(msg.content).get_to(shows);
    // Pick show
    auto [id, no_tickets] = pick_show();
    if (id == -1 || no_tickets == -1) {
        std::cout << "Error occurred during show pick.\n";
    }
    // Send show/ticket info
    shows.clear(); // Make sure shows is cleared
    const json k = message(code::buy_tickets, json{{"id", id},
                                                   {"no_tickets", no_tickets}}.dump());
    s = k.dump();
    ret_val = send(server_socket, s.data(), static_cast<int>(s.length()) + 1, 0);
    if (ret_val <= 0) return SOCKET_ERROR;
    return 0;
}

int main_call(const SOCKET &server_socket) {
    // Receive HELLO message
    char reply[2000];
    int ret_val = recv(server_socket, reply, 2000, 0);
    if (ret_val <= 0) {
        closesocket(server_socket);
        return ret_val;
    }
    const auto msg = json::parse(reply).get<message>();
    std::cout << msg.content << "\n\n";
    // Main menu
    while (ret_val != SOCKET_ERROR) {
        int option;
        std::cout << "Menu:\n";
        std::cout << "\t1 -> Buy tickets\n";
        std::cout << "\t2 -> Quit\n";
        std::cout << "Option: ";
        (std::cin >> option).ignore();
        switch (option) {
            case 1:
                ret_val = buy_tickets(server_socket);
                break;
            case 2:
                ret_val = quit_call(server_socket);
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
                break;
        }
    }
    // Close the socket
    closesocket(server_socket);
    return ret_val;
}

int quit_call(const SOCKET &server_socket) {
    // Confirm quit
    std::string option;
    std::cout << "Do you wish to quit? (y/n)\n";
    std::cout << "Option: ";
    getline(std::cin, option);
    if (option == "y") {
        // Send request to end call
        const json j = message(code::quit, "QUIT");
        const auto s = j.dump();
        int ret_val = send(server_socket, s.data(), static_cast<int>(s.length()) + 1, 0);
        if (ret_val <= 0) return SOCKET_ERROR;
        // Receive response to end call
        char reply[2000];
        ret_val = recv(server_socket, reply, 2000, 0);
        if (ret_val <= 0) return SOCKET_ERROR;
        const auto msg = json::parse(reply).get<message>();
        std::cout << msg.content << '\n';
        return -1;
    }
    return 0;
}