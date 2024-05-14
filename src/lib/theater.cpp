#include "theater.h"

theater::theater()
= default;

theater::theater(const std::string &name, const std::string &location) {
    this->name = name;
    this->location = location;
}

void to_json(json &j, const theater &t) {
    j = json{{"name",     t.name},
             {"location", t.location},
             {"shows",    t.shows}};
}

void from_json(const json &j, theater &t) {
    j.at("name").get_to(t.name);
    j.at("location").get_to(t.location);
    j.at("shows").get_to(t.shows);
}
