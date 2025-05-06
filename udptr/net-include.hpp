#pragma once

#ifdef _MSC_VER
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>

    #define LAST_NET_ERR() static_cast<int>(WSAGetLastError())
#else
    #include <cerrno> // errno
    #include <netinet/in.h> // struct sockaddr_in, struct in6_addr, INET_ADDRSTRLEN, INADDR_ANY, ...
    #include <arpa/inet.h> // inet_ntop(), inet_pton(), htons(), htonl(), setsockopt(), ...
    #include <sys/socket.h> // struct sockaddr_in6, struct sockaddr, socket(), bind(), sendto(), getsockopt(), socklen_t, ...
    #include <unistd.h> // close()
    #include <sys/select.h> // selct(), FD_CLR, FD_ISSET, ...
    #include <sys/time.h> // struct timeval

    #define LAST_NET_ERR() static_cast<int>(errno)
#endif

#include <sys/types.h> // time_t, suseconds_t
