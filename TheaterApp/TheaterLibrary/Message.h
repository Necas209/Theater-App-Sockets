#pragma once
#include <string>
#include <ctime>
#include "json.hpp"

using json = nlohmann::json;

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

const enum struct CODE : int {
	HELLO,
	GET_LOCATIONS,
	GET_GENRES,
	GET_SHOWS,
	BUY_TICKETS,
	QUIT
};

extern const std::map<CODE, const char*> codename;

class Message
{
public:
	static constexpr char fmt_str[] = "%Y-%m-%d %H:%M:%S";
	CODE code;
	std::string content;
	tm stamp;
	Message();
	Message(CODE code, std::string content);
	virtual ~Message();
	friend void to_json(json& j, const Message& m);
	friend void from_json(const json& j, Message& m);
};