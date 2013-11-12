#include "connection.h"
#include "server.h"

using namespace std;

Connection::Connection(Config& config):
    config(config), closed(false), dst_closed(false)
{
    Server* server = reinterpret_cast<Server*>(config.data);
    socket = (uv_stream_t*)malloc(sizeof(uv_tcp_t));
    dst_socket = (uv_stream_t*)malloc(sizeof(uv_tcp_t));

    uv_tcp_init(server->loop, (uv_tcp_t*)socket);
    uv_tcp_init(server->loop, (uv_tcp_t*)dst_socket);
    socket->data = this;
    dst_socket->data = this;
    uv_ip4_addr(config.dst_host.c_str(), config.dst_port, &addr);
}

int Connection::accept()
{
    Server* server = reinterpret_cast<Server*>(config.data);
    return uv_accept(&server->socket, socket);
}

void Connection::start()
{
    connect_to_dst();
}

void Connection::close()
{
    uv_shutdown((uv_shutdown_t*)malloc(sizeof(uv_shutdown_t)), socket, Connection::on_shutdown);
    uv_shutdown((uv_shutdown_t*)malloc(sizeof(uv_shutdown_t)), dst_socket, Connection::on_shutdown);
}

int Connection::write_to_peer(uv_stream_t* peer, char* buf, size_t size)
{
    uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
    uv_buf_t b = uv_buf_init(buf, size);
    return uv_write(req, peer, &b, 1, Connection::on_write);
}

void Connection::connect_to_dst()
{
    int r = uv_tcp_connect(&req, (uv_tcp_t*)dst_socket, (sockaddr*)&addr, Connection::on_connect);
    if(r < 0) {
        cout << uv_err_name(r) << endl;
    }
}

void Connection::on_connect(uv_connect_t* req, int status)
{
    if(status == 0)
    {
        Connection* conn = reinterpret_cast<Connection*>(req->handle->data);
        uv_read_start(conn->socket, on_alloc, on_read);
        uv_read_start(conn->dst_socket, on_alloc, on_dst_read);
    }
}

void Connection::on_alloc(uv_handle_t* /*con*/, size_t size, uv_buf_t* buf)
{
    buf->base = (char*)malloc(size);
    buf->len = size;
}

void Connection::on_shutdown(uv_shutdown_t* req, int status)
{
    if(status == 0)
    {
        uv_close((uv_handle_t*)req->handle, Connection::on_close);
        free(req);
    }
}

void Connection::on_close(uv_handle_t* handle)
{
    Connection* conn = reinterpret_cast<Connection*>(handle->data);
    if((uv_handle_t*)conn->dst_socket == handle) conn->dst_closed = true;
    if((uv_handle_t*)conn->socket == handle) conn->closed = true;
    free(handle);
    if(conn->dst_closed && conn->closed)
        delete conn;
}

void Connection::on_read(uv_stream_t* socket, ssize_t nread, const uv_buf_t* buf)
{
    Connection* conn = static_cast<Connection*>(socket->data);
    if(nread < 0)
    {
        conn->close();
    }
    else if(nread == 0)
    {
        conn->close();
        // really?
    }
    else
    {
        conn->write_to_peer(conn->dst_socket, buf->base, nread);
    }

    if(buf->base) free(buf->base);
}

void Connection::on_dst_read(uv_stream_t* socket, ssize_t nread, const uv_buf_t* buf)
{
    Connection* conn = static_cast<Connection*>(socket->data);
    if(nread < 0)
    {
        conn->close();
    }
    else if(nread == 0)
    {
        conn->close();
        // really?
    }
    else
    {
        conn->write_to_peer(conn->socket, buf->base, nread);
    }

    if(buf->base) free(buf->base);
}

void Connection::on_write(uv_write_t* req, int status)
{
    if(status == 0)
        free(req);
}
