#ifndef MESSAGES_H
#define MESSAGES_H

#include <string>
#include <unordered_map>
#include <vector>

#include "message.h"
#include "show.h"

using json = nlohmann::json;

class hello_request final : public message {
public:
    hello_request() : message(code::hello) {
    }
};

class hello_reply final : public message {
    std::string content_;

public:
    hello_reply() = default;

    explicit hello_reply(std::string content)
        : message(code::hello), content_(std::move(content)) {
    }

    [[nodiscard]] auto &content() const { return content_; }

    friend void to_json(json &j, const hello_reply &m) {
        j = json{
            {"code", m.code_},
            {"stamp", m.stamp_},
            {"content", m.content_}
        };
    }

    friend void from_json(const json &j, hello_reply &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
        j.at("content").get_to(m.content_);
    }
};

class quit_request final : public message {
public:
    quit_request() : message(code::quit) {
    }
};

class quit_reply final : public message {
    std::string content_{};

public:
    quit_reply() = default;

    explicit quit_reply(std::string content)
        : message(code::quit), content_(std::move(content)) {
    }

    [[nodiscard]] auto &content() const { return content_; }

    friend void to_json(json &j, const quit_reply &m) {
        j = json{
            {"code", m.code()},
            {"stamp", m.stamp()},
            {"content", m.content()}
        };
    }

    friend void from_json(const json &j, quit_reply &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
        j.at("content").get_to(m.content_);
    }
};

class locations_request final : public message {
public:
    locations_request()
        : message(code::get_locations) {
    }
};

class locations_reply final : public message {
    std::vector<std::string> locations_{};

public:
    locations_reply() = default;

    explicit locations_reply(std::vector<std::string> &&locations)
        : message(code::get_locations), locations_(std::move(locations)) {
    }

    [[nodiscard]] auto &locations() const { return locations_; }

    friend void to_json(json &j, const locations_reply &m) {
        j = json{
            {"code", m.code()},
            {"stamp", m.stamp()},
            {"locations", m.locations()}
        };
    }

    friend void from_json(const json &j, locations_reply &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
        j.at("locations").get_to(m.locations_);
    }
};

class genres_request final : public message {
    std::string location_;

public:
    genres_request() = default;

    explicit genres_request(std::string location)
        : message(code::get_genres), location_(std::move(location)) {
    }

    [[nodiscard]] auto &location() const { return location_; }

    friend void to_json(json &j, const genres_request &m) {
        j = json{
            {"code", m.code()},
            {"stamp", m.stamp()},
            {"location", m.location()}
        };
    }

    friend void from_json(const json &j, genres_request &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
        j.at("location").get_to(m.location_);
    }
};

class genres_reply final : public message {
    std::vector<std::string> genres_{};

public:
    genres_reply() = default;

    explicit genres_reply(std::vector<std::string> &&genres)
        : message(code::get_genres), genres_(std::move(genres)) {
    }

    [[nodiscard]] auto &genres() const { return genres_; }

    friend void to_json(json &j, const genres_reply &m) {
        j = json{
            {"code", m.code()},
            {"stamp", m.stamp()},
            {"genres", m.genres()}
        };
    }

    friend void from_json(const json &j, genres_reply &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
        j.at("genres").get_to(m.genres_);
    }
};

class shows_request final : public message {
    std::string location_{};
    std::string genre_{};

public:
    shows_request() = default;

    shows_request(std::string location, std::string genre)
        : message(code::get_shows), location_(std::move(location)), genre_(std::move(genre)) {
    }

    [[nodiscard]] auto &location() const { return location_; }

    [[nodiscard]] auto &genre() const { return genre_; }

    friend void to_json(json &j, const shows_request &m) {
        j = json{
            {"code", m.code()},
            {"stamp", m.stamp()},
            {"location", m.location()},
            {"genre", m.genre()}
        };
    }

    friend void from_json(const json &j, shows_request &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
        j.at("location").get_to(m.location_);
        j.at("genre").get_to(m.genre_);
    }
};

class shows_reply final : public message {
    std::unordered_map<std::string, std::vector<show> > shows_{};

public:
    shows_reply() = default;

    explicit shows_reply(std::unordered_map<std::string, std::vector<show> > &shows)
        : message(code::get_shows), shows_(std::move(shows)) {
    }

    [[nodiscard]] auto &shows() const { return shows_; }

    friend void to_json(json &j, const shows_reply &m) {
        j = json{
            {"code", m.code()},
            {"stamp", m.stamp()},
            {"shows", m.shows()}
        };
    }

    friend void from_json(const json &j, shows_reply &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
        j.at("shows").get_to(m.shows_);
    }
};

class tickets_request final : public message {
    std::string theater_{};
    int show_id_{};
    int no_tickets_{};

public:
    tickets_request() = default;

    tickets_request(std::string theater, const int show_id, const int no_tickets)
        : message(code::buy_tickets), theater_(std::move(theater)), show_id_(show_id), no_tickets_(no_tickets) {
    }

    [[nodiscard]] auto &theater() const { return theater_; }

    [[nodiscard]] auto show_id() const { return show_id_; }

    [[nodiscard]] auto no_tickets() const { return no_tickets_; }

    friend void to_json(json &j, const tickets_request &m) {
        j = json{
            {"code", m.code()},
            {"stamp", m.stamp()},
            {"show_id", m.show_id()},
            {"no_tickets", m.no_tickets()}
        };
    }

    friend void from_json(const json &j, tickets_request &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
        j.at("show_id").get_to(m.show_id_);
        j.at("no_tickets").get_to(m.no_tickets_);
    }
};

class tickets_reply final : public message {
    std::string error_{};

public:
    tickets_reply() = default;

    explicit tickets_reply(std::string error)
        : message(code::buy_tickets), error_(std::move(error)) {
    }

    [[nodiscard]] auto success() const { return error_.empty(); }

    [[nodiscard]] auto &error() const { return error_; }

    friend void to_json(json &j, const tickets_reply &m) {
        j = json{
            {"code", m.code()},
            {"stamp", m.stamp()},
            {"error", m.error()}
        };
    }

    friend void from_json(const json &j, tickets_reply &m) {
        j.at("code").get_to(m.code_);
        j.at("stamp").get_to(m.stamp_);
        j.at("error").get_to(m.error_);
    }
};

#endif //MESSAGES_H
