#ifndef SHOW_H
#define SHOW_H

#include <chrono>
#include <print>
#include <sstream>
#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include "extensions.h"
#include "json.hpp"

using json = nlohmann::json;

class show final {
    int id_{};
    std::string name_;
    std::string genre_;
    std::chrono::system_clock::time_point datetime_{};
    int capacity_{};
    int available_seats_{};

public:
    show() = default;

    [[nodiscard]] auto id() const { return id_; }

    [[nodiscard]] auto name() const { return name_; }

    [[nodiscard]] auto datetime() const { return datetime_; }

    [[nodiscard]] auto capacity() const { return capacity_; }

    [[nodiscard]] auto available_seats() const { return available_seats_; }

    [[nodiscard]] auto genre() const { return genre_; }

    [[nodiscard]] auto buy_tickets(const int no_tickets) {
        if (no_tickets > available_seats_) {
            throw std::runtime_error("Not enough tickets available.");
        }
        available_seats_ -= no_tickets;
    }

    auto write() const {
        const auto datetime_c = std::chrono::system_clock::to_time_t(datetime_);
        std::ostringstream dt;
        dt << std::put_time(std::localtime(&datetime_c), "%Y-%m-%d %H:%M:%S");;
        std::println("({}) -> {} | {} | {}", id_, name_, genre_, dt.str());
        std::println("Capacity: {}", capacity_);
        std::println("Available seats: {}", available_seats_);
    }

    friend void to_json(json &j, const show &s) {
        j = json{
            {"id", s.id_},
            {"name", s.name_},
            {"genre", s.genre_},
            {"datetime", s.datetime_},
            {"capacity", s.capacity_},
            {"available_seats", s.available_seats_}
        };
    }

    friend void from_json(const json &j, show &s) {
        j.at("id").get_to(s.id_);
        j.at("name").get_to(s.name_);
        j.at("genre").get_to(s.genre_);
        j.at("datetime").get_to(s.datetime_);
        j.at("capacity").get_to(s.capacity_);
        j.at("available_seats").get_to(s.available_seats_);
    }
};

#endif //SHOW_H
