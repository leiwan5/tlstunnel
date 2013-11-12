#ifndef SERVER_H
#define SERVER_H

#include "pch.h"
#include "config.h"

class Server
{
public:
    Server(Config& config);

    Config& config;

    int start();

    uv_loop_t* loop;
    uv_stream_t socket;

    sockaddr_in addr;

    static void on_connection(uv_stream_t* socket, int status);

};

#endif // SERVER_H
