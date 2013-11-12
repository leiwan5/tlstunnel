#ifndef CONNECTION_H
#define CONNECTION_H

#include "server.h"
#include "config.h"

class Connection
{
public:
    Connection(Config& config);

    Config& config;
    uv_stream_t* socket;
    uv_stream_t* dst_socket;
    sockaddr_in addr;
    uv_connect_t req;
    bool closed;
    bool dst_closed;

    static void on_connect(uv_connect_t* req, int status);
    static void on_alloc(uv_handle_t* con, size_t size, uv_buf_t* buf);
    static void on_shutdown(uv_shutdown_t* req, int status);
    static void on_close(uv_handle_t* handle);
    static void on_read(uv_stream_t* socket, ssize_t nread, const uv_buf_t* buf);
    static void on_dst_read(uv_stream_t* socket, ssize_t nread, const uv_buf_t* buf);
    static void on_write(uv_write_t* req, int status);

    int accept();
    void start();
    void close();
    int write_to_peer(uv_stream_t* peer, char* buf, size_t size);

private:
    void connect_to_dst();
};

#endif // CONNECTION_H
