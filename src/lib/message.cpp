#include "message.h"

const std::map<code, const char *> message::codename{
        {code::hello,         "HELLO"},
        {code::get_locations, "GET_LOCATIONS"},
        {code::get_genres,    "GET_GENRES"},
        {code::get_shows,     "GET_SHOWS"},
        {code::buy_tickets,   "BUY_TICKETS"},
        {code::quit,          "QUIT"}};

message::message()
= default;

message::message(const ::code code, const std::string &content) {
    this->code = code;
    this->content = content;
    const time_t t = time(nullptr);
    localtime_s(&stamp, &t);
}

void to_json(json &j, const message &m) {
    std::ostringstream ss;
    ss << std::put_time(&m.stamp, message::fmt_str);
    j = json{{"code",    m.code},
             {"content", m.content},
             {"stamp",   ss.str()}};
}

void from_json(const json &j, message &m) {
    j.at("code").get_to(m.code);
    j.at("content").get_to(m.content);
    std::istringstream ss{j.at("stamp").get<std::string>()};
    ss >> std::get_time(&m.stamp, message::fmt_str);
}
