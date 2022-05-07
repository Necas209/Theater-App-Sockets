#pragma once
#include <iostream>
#include <iomanip>
#include "json.hpp"

using json = nlohmann::json;

class show final
{
	// Example: "Sun Aug 08 12:30:00 2023"
	static constexpr char fmt_str[] = "%a %b %d %H:%M:%S %Y";
public:
	// attributes
	int id{};
	std::string name;
	std::string genre;
	tm datetime{};
	int capacity{};
	int available_seats{};
	// methods
	show();
	void write() const;
	// serializer and deserializer
	friend void to_json(json& j, const show& s);
	friend void from_json(const json& j, show& s);
};
