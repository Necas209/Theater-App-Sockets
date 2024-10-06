#ifndef HANDLER_H
#define HANDLER_H

#include "json.hpp"
#include "sockets.h"
#include "socket_error.h"
#include "message.h"

using json = nlohmann::json;

class handler {
public:
    template<typename T = message>
    static void send(const SOCKET socket, const T &msg) {
        const auto j = json(msg);
        const std::string message = j.dump();
        if (::send(socket, message.c_str(), message.size() + 1, 0) == SOCKET_ERROR) {
            throw socket_error(msg.code(), "Could not send message.");
        }
    }

    template<typename T = message>
    static T receive(const SOCKET socket) {
        char reply[buffer_size];
        if (recv(socket, reply, buffer_size, 0) <= 0) {
            throw socket_error({}, "Could not receive message.");
        }
        const auto j = json::parse(reply);
        return j.get<T>();
    }
};

#endif //HANDLER_H
