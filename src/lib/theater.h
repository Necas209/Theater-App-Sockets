#ifndef THEATER_H
#define THEATER_H

#include "json.hpp"
#include "show.h"
#include <list>
#include <vector>

using json = nlohmann::json;

class theater final {
    std::string name_;
    std::string location_;
    std::vector<show> shows_;

public:
    theater() = default;

    theater(std::string &&name, std::string &&location)
        : name_{std::move(name)}, location_{std::move(location)} {
    }

    [[nodiscard]] auto &name() const { return name_; }

    [[nodiscard]] auto &location() const { return location_; }

    [[nodiscard]] auto &shows() const { return shows_; }

    [[nodiscard]] auto get_show(const int id) {
        return std::ranges::find_if(shows_, [&](const show &s) { return s.id() == id; });
    }

    friend void to_json(json &j, const theater &t) {
        j = json{
            {"name", t.name_},
            {"location", t.location_},
            {"shows", t.shows_}
        };
    }

    friend void from_json(const json &j, theater &t) {
        j.at("name").get_to(t.name_);
        j.at("location").get_to(t.location_);
        j.at("shows").get_to(t.shows_);
    }
};

#endif //THEATER_H
