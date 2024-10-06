#ifndef CLIENT_H
#define CLIENT_H

#include <unordered_map>
#include <vector>

#include "show.h"
#include "sockets.h"

/**
 * @brief Validates IP address
 * @param ip_address IP address
 * @return boolean: whether IP address is valid or not
*/
bool validate_ip(const std::string &ip_address);

class client {
    SOCKET server_{};
    std::unordered_map<std::string, std::vector<show> > shows_by_theater_{};

    /// @brief Constructor
    /// @param server server socket
    /// @throws std::runtime_error if socket creation fails
    explicit client(SOCKET server);

    /// @brief Picks a show
    /// @return optional: show picked by client or none
    [[nodiscard]] std::optional<std::pair<std::string, show> > pick_show() const;

    /// @brief Picks a number of tickets to buy for a show
    /// @param show show picked
    /// @return integer: number of tickets to buy
    [[nodiscard]] static int pick_no_tickets(const show &show);

public:
    ~client();

    /// @brief Creates a new client
    /// @return client object
    /// @throws std::runtime_error if socket creation fails
    [[nodiscard]] static client new_client();

    /// @brief Connects to server
    /// @param ip_addr IP address
    /// @throws std::runtime_error if connection fails
    /// @throws socket_error if HELLO message cannot be received
    void connect(const std::string &ip_addr) const;

    /// @brief Main menu
    /// @throws socket_error if message cannot be sent
    void menu();

    /// @brief Asks client for location
    /// @return string: location picked
    /// @throws socket_error if GET_LOCATIONS message cannot be sent or received
    [[nodiscard]] std::string pick_location() const;

    /// @brief Asks client for genre
    /// @param location location picked
    /// @return string: genre picked
    /// @throws socket_error if GET_GENRES message cannot be sent or received
    [[nodiscard]] std::string pick_genre(const std::string &location) const;

    /// @brief Allows client to buy tickets
    /// @throws socket_error if GET_LOCATIONS message cannot be sent or received
    /// @throws socket_error if GET_GENRES message cannot be sent or received
    /// @throws socket_error if GET_SHOWS message cannot be sent or received
    /// @throws socket_error if BUY_TICKETS message cannot be sent or received
    void buy_tickets();

    /// @brief Quits the client
    /// @throws socket_error if QUIT message cannot be sent
    void quit() const;
};

#endif // CLIENT_H
