#ifndef SERVER_H
#define SERVER_H

#include "sockets.h"
#include "server_client.h"
#include "messages.h"
#include "theater.h"
#include <mutex>
#include <vector>

enum class sender { client, server };

class server final {
    SOCKET listening_{};
    std::vector<theater> theaters_{};
    std::map<std::string, server_client> clients_{};

    std::mutex tickets_mutex;
    std::mutex log_mutex;
    thread_local static server_client *client_;

    explicit server(const SOCKET listening)
        : listening_(listening) {
    }

public:
    ~server();

    [[nodiscard]] static server new_server();

    void setup(unsigned port) const;

    server_client& accept_call();

    /**
     * @brief Sends available locations to client
     * @return integer: value of send
    */
    void get_locations();

    /**
     * @brief Sends available genres to client
     * @param msg genres request message
    */
    void get_genres(const genres_request &msg);

    /**
     * @brief Sends available shows to client, given location and genre
     * @param msg shows request message
    */
    void get_shows(const shows_request &msg);

    /**
     * @brief Processes client's ticket purchase
     * @param msg tickets request message
    */
    void buy_tickets(const tickets_request &msg);

    /**
    * @brief Main client call function
    * @param client
    */
    void main_call(server_client &client);

    /**
     * @brief Quits call with client
     * @return integer: send return value
    */
    void quit_client();

    /**
     * @brief Logs message
     * @param msg message send/received
     * @param sender CLIENT or SERVER
    */
    void log_message(const message &msg, sender sender);

    void read_data();

    void save_data() const;
};

#endif //SERVER_H
