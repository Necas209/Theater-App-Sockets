#pragma once

#include <list>
#include "show.h"

class theater final {
public:
    // attributes
    std::string name;
    std::string location;
    std::list<show> shows;

    // methods
    theater();

    explicit theater(const std::string &name, const std::string &location);

    // serializer and deserializer
    friend void to_json(json &j, const theater &t);

    friend void from_json(const json &j, theater &t);
};
