#ifndef CLIENT_H
#define CLIENT_H

#include "json.hpp"
#include "messages.h"
#include <string>
#include <map>
#include <utility>

using json = nlohmann::json;

class server_client final {
    SOCKET socket_{};
    std::string ip_address_;
    /**
     * @brief map of purchases: show id -> number of tickets
    */
    std::map<int, int> shows_seen;

public:
    server_client() = default;

    explicit server_client(const SOCKET socket, std::string ip_address)
        : socket_(socket), ip_address_(std::move(ip_address)) {
    }

    [[nodiscard]] auto has_seen(const int show_id) const -> bool { return shows_seen.contains(show_id); }

    auto add_seen(const tickets_request &msg) { shows_seen[msg.show_id()] = msg.no_tickets(); }

    void set_socket(const SOCKET socket) { socket_ = socket; }

    [[nodiscard]] auto socket() const { return socket_; }

    [[nodiscard]] auto &ip_address() const { return ip_address_; }

    friend void to_json(json &j, const server_client &c) {
        j = json{
            {"ip_addr", c.ip_address_},
            {"shows_seen", c.shows_seen},
        };
    }

    friend void from_json(const json &j, server_client &c) {
        j.at("ip_addr").get_to(c.ip_address_);
        j.at("shows_seen").get_to(c.shows_seen);
    }
};

#endif //CLIENT_H
