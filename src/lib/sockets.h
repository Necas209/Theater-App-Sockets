#ifndef SOCKETS_H
#define SOCKETS_H

#if defined(_WIN32) || defined(_WIN64)

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#else

// ReSharper disable CppUnusedIncludeDirective
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// ReSharper restore CppUnusedIncludeDirective

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define SOCKET int

#endif

constexpr size_t buffer_size = 2048;

#endif //SOCKETS_H
