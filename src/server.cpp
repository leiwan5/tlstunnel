#include "server.h"
#include "pch.h"

#include "connection.h"

using namespace std;

Server::Server(Config& config):config(config)
{
//    loop = uv_loop_new();
    loop = uv_default_loop();
    uv_tcp_init(loop, (uv_tcp_t*)&socket);
    socket.data = this;
    uv_ip4_addr(config.host.c_str(), config.port, &addr);
    uv_tcp_bind((uv_tcp_t*)&socket, (sockaddr*)&addr);
    config.data = reinterpret_cast<void*>(this);
}

int Server::start()
{
    uv_listen(&socket, 128, Server::on_connection);
    return uv_run(loop, UV_RUN_DEFAULT);
}

void Server::on_connection(uv_stream_t* socket, int status)
{
    if(status == 0)
    {
        Server* server = reinterpret_cast<Server*>(socket->data);
        Connection* conn = new Connection(server->config);
        conn->accept();
        conn->start();
    }
}
