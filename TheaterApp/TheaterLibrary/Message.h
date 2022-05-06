#pragma once
#include <string>
#include <ctime>
#include "json.hpp"

using json = nlohmann::json;

enum struct code {
	hello,
	get_locations,
	get_genres,
	get_shows,
	buy_tickets,
	quit
};

extern const std::map<code, const char*> codename;
/**
* Type of Message:
*
* -> GET_LOCATIONS: returns set of locations
* -> GET_GENRES: returns set of genres
* -> GET_SHOWS: returns list of shows
* -> BUY_TICKETS: updates show and client's information, to reflect ticket purchase
* -> QUIT: quits call
*
* Message structure:
*
* {
*	"code": GET_LOCATIONS, // Integer value corresponding to given code
*	"content": "", // or [ "Vila Real", "Fafe" ]
* }
*/
class message final
{
public:
	static constexpr char fmt_str[] = "%Y-%m-%d %H:%M:%S";
	code code{};
	std::string content;
	tm stamp{};
	message();
	message(::code code, const std::string& content);
	friend void to_json(json& j, const message& m);
	friend void from_json(const json& j, message& m);
};