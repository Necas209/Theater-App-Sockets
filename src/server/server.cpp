#include "server.h"

#include <fstream>
#include <iostream>
#include <mutex>
#include <print>
#include <vector>

#include "handler.h"
#include "sockets.h"
#include "message.h"

namespace rv = std::ranges::views;

thread_local server_client *server::client_;

server::~server() {
#  if defined(_WIN32) || defined(_WIN64)
        closesocket(listening_);
        WSACleanup();
#elif defined(__linux__) || defined(__APPLE__)
    close(listening_);
#endif
}

server server::new_server() {
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
    const auto listening = socket(AF_INET, SOCK_STREAM, 0);
    std::println("Socket created.");
    return server{listening};
}

void server::setup(unsigned port) const {
    // Bind the socket (ip address and port)
    sockaddr_in hint{};
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
#  if defined(_WIN32) || defined(_WIN64)
        hint.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    hint.sin_addr.s_addr = INADDR_ANY;
#endif
    if (bind(listening_, reinterpret_cast<sockaddr *>(&hint), sizeof hint) == SOCKET_ERROR) {
        throw std::runtime_error("Address binding failed!");
    }
    // Set up the socket for listening
    if (listen(listening_, SOMAXCONN) == SOCKET_ERROR) {
        throw std::runtime_error("Listening failed!");
    }
}

server_client &server::accept_call() {
    sockaddr_in client_addr{};
    socklen_t client_size = sizeof client_addr;
    const auto client_socket = accept(listening_, reinterpret_cast<sockaddr *>(&client_addr), &client_size);
    if (client_socket == INVALID_SOCKET) {
        throw std::runtime_error("Accept failed!");
    }
    // Get client's IP address
    char buf[20];
    inet_ntop(client_addr.sin_family, &client_addr.sin_addr, buf, sizeof buf);
    const auto ip_addr = std::string(buf);
    if (!clients_.contains(ip_addr)) {
        clients_.emplace(ip_addr, server_client{client_socket, ip_addr});
    } else {
        clients_[ip_addr].set_socket(client_socket);
    }
    return clients_[ip_addr];
}

void server::main_call(server_client &client) {
    client_ = &client;
    std::println("Hello, client!\n");
    while (true) {
        // Receive next message and parse it
        char reply[buffer_size];
        if (recv(client.socket(), reply, 2000, 0) <= 0) {
            throw std::runtime_error("Receive failed!");
        }
        // Process message based on code
        switch (const auto j = json::parse(reply); j.at("code").get<code>()) {
            case code::get_locations: {
                const auto request = j.get<locations_request>();
                log_message(request, sender::client);
                get_locations();
                break;
            }
            case code::get_genres: {
                const auto request = j.get<genres_request>();
                log_message(request, sender::client);
                get_genres(request);
                break;
            }
            case code::get_shows: {
                const auto request = j.get<shows_request>();
                log_message(request, sender::client);
                get_shows(request);
                break;
            }
            case code::quit: {
                const auto request = j.get<quit_request>();
                log_message(request, sender::client);
                quit_client();
                return;
            }
            case code::buy_tickets: {
                const auto request = j.get<tickets_request>();
                log_message(request, sender::client);
                buy_tickets(request);
                break;
            }
            case code::hello: {
                const auto request = j.get<hello_request>();
                log_message(request, sender::client);
                // Send 200 HELLO message to client
                const auto msg = hello_reply{"200 HELLO"};
                handler::send(client.socket(), msg);
                log_message(msg, sender::server);
                break;
            }
        }
    }
}

void server::read_data() {
    // Read theaters and clients from files
    std::ifstream ifs{"shows.json"};
    json j;
    ifs >> j;
    j.get_to(theaters_);
    std::ifstream ifs2{"clients.json"};
    json j2;
    ifs2 >> j2;
    j2.get_to(clients_);
}

void server::save_data() const {
    std::ofstream ofs{"shows.json"};
    ofs << std::setw(2) << json{theaters_};
    std::ofstream ofs2{"clients.json"};
    ofs2 << std::setw(2) << json{clients_};
}

void server::log_message(const message &msg, const sender sender) {
    std::lock_guard guard(log_mutex);
    // Get log file path
    const auto filename = std::format(R"(.\theater_logs\{}.log)", client_->ip_address());
    // Log message
    std::ofstream ofs{filename, std::ios_base::app};
    const auto direction = sender == sender::client ? "From" : "To";
    const auto log_msg = std::format("{},{}:{},{}\n",
                                     msg.stamp(),
                                     direction,
                                     client_->ip_address(),
                                     codenames.at(msg.code()));
    ofs << log_msg;
}

void server::get_locations() {
    // Make set of (non-repeated) locations
    std::vector<std::string> locations;
    for (const auto &theater: theaters_) {
        if (const auto location = theater.location();
            std::ranges::find(locations, location) == locations.end()) {
            locations.emplace_back(location);
        }
    }
    // Send locations to client
    const auto msg = locations_reply{std::move(locations)};
    handler::send(client_->socket(), msg);
    log_message(msg, sender::server); // Log message
}

void server::get_genres(const genres_request &msg) {
    // Find theater in given location
    const auto theater = std::ranges::find_if(theaters_, [&](const auto &t) {
        return t.location() == msg.location();
    });
    if (theater == theaters_.end()) {
        return;
    }
    // Make set of (non-repeated) genres
    std::vector<std::string> genres;
    for (const auto &show: theater->shows()) {
        if (std::ranges::find(genres, show.genre()) == genres.end()) {
            genres.push_back(show.genre());
        }
    }
    const auto reply = genres_reply{std::move(genres)};
    handler::send(client_->socket(), reply);
    log_message(reply, sender::server); // Log message
}

void server::get_shows(const shows_request &msg) {
    // Find theaters in given location
    const auto &location = msg.location();
    auto theaters = theaters_
                    | rv::filter([location](const auto &t) {
                        return t.location() == location;
                    });
    // Filter shows
    const auto &genre = msg.genre();
    std::unordered_map<std::string, std::vector<show> > shows{};
    for (const auto &theater: theaters) {
        auto filtered_shows = theater.shows()
                              | rv::filter([genre](const auto &s) {
                                  return s.genre() == genre;
                              });
        shows.emplace(theater.name(), std::ranges::to<std::vector<show> >(filtered_shows));
    }
    // Send shows to client
    const auto reply = shows_reply(shows);
    handler::send(client_->socket(), reply);
    log_message(reply, sender::server); // Log message
}

void server::buy_tickets(const tickets_request &msg) {
    // Lock other threads access to shows
    std::lock_guard guard(tickets_mutex);
    // Update available seats
    const auto theater = std::ranges::find_if(theaters_, [&](const auto &t) {
        return t.name() == msg.theater();
    });
    const auto show = theater->get_show(msg.show_id());
    if (show == theater->shows().end()) {
        handler::send(client_->socket(), tickets_reply("404 NOT FOUND"));
    }
    if (show->available_seats() < msg.no_tickets()) {
        handler::send(client_->socket(), tickets_reply("406 NOT ENOUGH SEATS"));
    }
    show->buy_tickets(msg.no_tickets());
    // Send 200 OK message to client
    handler::send(client_->socket(), tickets_reply{});
    // Update client's seen shows
    client_->add_seen(msg);
}

void server::quit_client() {
    // Send 400 BYE message to client
    const auto msg = quit_reply("400 BYE");
    handler::send(client_->socket(), msg);
    log_message(msg, sender::server); // Log message
    std::println("Bye, client '{}'\n", client_->ip_address());
}
