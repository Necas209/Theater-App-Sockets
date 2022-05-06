#include "utils.h"

std::list<theater> theaters;
std::map<std::string, client> clients;
std::mutex tickets_mutex;
std::mutex log_mutex;
thread_local std::string ip_addr;

void log_message(const message& msg, const sender sender)
{
	// Lock access to logs
	std::lock_guard guard(log_mutex);
	// Log message
	const theater_log l(msg, sender, ip_addr);
	const std::string path = log_path(l);
	std::ofstream ofs{ path, std::ios_base::app };
	ofs << l;
}

void read_theaters(const char* filename)
{
	std::ifstream ifs{ filename };
	json j;
	ifs >> j;
	j.get_to(theaters);
}

void write_theaters(const char* filename)
{
	std::ofstream ofs{ filename };
	const json j = theaters;
	ofs << std::setw(4) << j;
}

void read_clients(const char* filename)
{
	std::ifstream ifs{ filename };
	json j;
	ifs >> j;
	j.get_to(clients);
}

void write_clients(const char* filename)
{
	std::ofstream ofs{ filename };
	const json j = clients;
	ofs << std::setw(4) << j;
}

int get_locations(const SOCKET& client_socket)
{
	// Make set of (non-repeated) locations
	std::set<std::string> locations;
	for (auto& theater : theaters)
	{
		locations.insert(theater.location);
	}
	// Send locations to client
	const json j = locations;
	message msg(code::get_locations, j.dump());
	const json k = msg;
	const auto s = k.dump();
	const int ret_val = send(client_socket, s.data(), static_cast<int>(s.length()) + 1, 0);
	log_message(msg, sender::server); // Log message
	return ret_val;
}

int get_genres(const SOCKET& client_socket, message& msg)
{
	// Find theater in given location
	const auto& location = msg.content;
	const auto it = std::ranges::find_if(theaters, [&](const theater& t)
		{ return t.location == location; });
	if (it == theaters.end()) return 0;
	// Make set of (non-repeated) genres
	std::set<std::string> genres;
	for (auto& show : (*it).shows)
	{
		genres.insert(show.genre);
	}
	// Send genres to client
	const json j = genres;
	msg = message(code::get_genres, j.dump());
	const json k = msg;
	const auto s = k.dump();
	const int ret_val = send(client_socket, s.data(), static_cast<int>(s.length()) + 1, 0);
	log_message(msg, sender::server); // Log message
	return ret_val;
}

int get_shows(const SOCKET& client_socket, message& msg, std::list<theater>::iterator& it)
{
	// Parse message content for location and genre
	json j = json::parse(msg.content);
	std::string location, genre;
	j.at("location").get_to(location);
	j.at("genre").get_to(genre);
	// Find theater in given location
	it = std::ranges::find_if(theaters, [&](const theater& t) { return t.location == location; });
	if (it != theaters.end())
	{
		// Filter shows
		std::list<show> shows;
		std::ranges::copy_if((*it).shows, std::back_inserter(shows),
			[&](const show& s) {
				return s.available_seats > 0
					&& s.genre == genre
					&& !clients[ip_addr].has_seen(s.id);
			});
		// Send shows to client
		const json js = shows;
		message message(code::get_shows, js.dump());
		const json k = message;
		const auto s = k.dump();
		const int ret_val = send(client_socket, s.data(), static_cast<int>(s.length()) + 1, 0);
		log_message(message, sender::server); // Log message
		if (ret_val <= 0) return SOCKET_ERROR;
	}
	return 0;
}

int buy_tickets(const SOCKET& client_socket, message& msg)
{
	// Lock access to shows
	std::lock_guard guard(tickets_mutex);
	// Send available shows to client
	std::list<theater>::iterator it;
	int ret_val = get_shows(client_socket, msg, it);
	if (ret_val < 0) return SOCKET_ERROR;
	// Get ticket info from client
	char reply[2000];
	ret_val = recv(client_socket, reply, 2000, 0);
	if (ret_val <= 0) return SOCKET_ERROR;
	// Parse ticket info
	msg = json::parse(reply).get<message>();
	log_message(msg, sender::client); // Log message
	json j = json::parse(msg.content);
	int id, no_tickets;
	j.at("id").get_to(id);
	j.at("no_tickets").get_to(no_tickets);
	if (id != -1 && no_tickets != -1)
	{
		// Update client's seen shows
		clients[ip_addr].shows_seen.emplace_back(id, no_tickets);
		// Update available seats
		const auto show_it = std::ranges::find_if((*it).shows,
			[&](const show& s) { return s.id == id; });
		(*show_it).available_seats -= no_tickets;
	}
	return ret_val;
}

int main_call(const SOCKET client_socket, const sockaddr_in client_addr)
{
	// Get client's IP address
	char buf[20];
	inet_ntop(client_addr.sin_family, &client_addr.sin_addr, buf, 20);
	ip_addr = std::string(buf);
	if (!clients.contains(ip_addr))
	{
		clients.insert(std::make_pair(ip_addr, client(ip_addr)));
	}
	// Send HELLO message
	message m(code::hello, "100 OK");
	const json j = m;
	const auto s = j.dump();
	int ret_val = send(client_socket, s.data(), static_cast<int>(s.length()) + 1, 0);
	log_message(m, sender::server); // Log message
	std::cout << "Hello, client!\n\n";
	while (ret_val > 0)
	{
		// Receive next message and parse it
		char reply[2000];
		ret_val = recv(client_socket, reply, 2000, 0);
		if (ret_val <= 0) break;
		auto msg = json::parse(reply).get<message>();
		log_message(msg, sender::client); // Log message
		// Call proper function, according to message code
		switch (msg.code)
		{
		case code::get_locations:
			ret_val = get_locations(client_socket);
			break;
		case code::get_genres:
			ret_val = get_genres(client_socket, msg);
			break;
		case code::get_shows:
			ret_val = buy_tickets(client_socket, msg);
			break;
		case code::quit:
			ret_val = quit_call(client_socket);
			break;
		case code::hello:
			std::cout << msg.content << "\n\n";
			break;
		case code::buy_tickets:
			break;
		}
	}
	// Close the socket
	closesocket(client_socket);
	return ret_val;
}

int quit_call(const SOCKET& client_socket)
{
	// Send 400 BYE message to client
	message msg(code::quit, "400 BYE");
	const json j = msg;
	const auto s = j.dump();
	send(client_socket, s.data(), static_cast<int>(s.length()) + 1, 0);
	log_message(msg, sender::server); // Log message
	std::cout << "Bye, client...\n\n";
	return 0;
}