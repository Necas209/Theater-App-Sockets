#ifndef MESSAGE_H
#define MESSAGE_H

#include <chrono>

#include "json.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "extensions.h"

using json = nlohmann::json;

enum class code {
    hello,
    get_locations,
    get_genres,
    get_shows,
    buy_tickets,
    quit
};

const std::map<code, const char *> codenames{
    {code::hello, "HELLO"},
    {code::get_locations, "GET_LOCATIONS"},
    {code::get_genres, "GET_GENRES"},
    {code::get_shows, "GET_SHOWS"},
    {code::buy_tickets, "BUY_TICKETS"},
    {code::quit, "QUIT"}
};

class message {
protected:
    code code_{};
    std::chrono::system_clock::time_point stamp_{};

public:
    message() = default;

    explicit message(const code code)
        : code_(code), stamp_(std::chrono::system_clock::now()) {
    }

    [[nodiscard]] auto code() const { return code_; }

    [[nodiscard]] auto stamp() const { return stamp_; }

    friend void to_json(json &j, const message &m) {
        j = json{
            {"code", m.code_},
            {"stamp", m.stamp_}
        };
    }

    friend void from_json(const json &j, message &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
    }
};

#endif //MESSAGE_H
