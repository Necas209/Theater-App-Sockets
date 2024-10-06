#ifndef SOCKET_ERROR_H
#define SOCKET_ERROR_H

#include "message.h"

class socket_error final : public std::exception {
    code code_{};
    std::string message_;

public:
    socket_error(const code code, std::string message)
        : code_(code), message_(std::move(message)) {
    }

    [[nodiscard]] auto what() const noexcept -> const char * override {
        const auto fmt = std::format("{}: {}", codenames.at(code_), message_);
        const auto fmt_str = new char[fmt.size() + 1];
        std::ranges::copy(fmt, fmt_str);
        fmt_str[fmt.size()] = '\0';
        return fmt_str;
    }
};

#endif //SOCKET_ERROR_H
