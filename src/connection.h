#ifndef CONNECTION_H
#define CONNECTION_H

#include "pch.h"
#include "server.h"

// typedef struct {
// 	uv_write_t req;
// 	uv_buf_t buf;
// } write_req;

class Connection
{
public:
	Connection(SSL_CTX* ssl_ctx, uv_tcp_t* socket);
	~Connection();
	static void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf);
	static void on_alloc(uv_handle_t* con, size_t size, uv_buf_t* buf);
	static void on_shutdown(uv_shutdown_t* req, int status);
	static void on_close(uv_handle_t* handle);
	static void on_write(uv_write_t* req, int status);

	void flush_write_bio();
	void write_to_socket(char* buf, unsigned int size);
	void procced();

	uv_loop_t* _loop;

	SSL_CTX* _ssl_ctx;
	SSL* _ssl;

	BIO* _read_bio;
	BIO* _write_bio;

	Server* _server;
	uv_tcp_t* _socket;

	bool _ssl_init_finished;
	bool _ssl_handshake_finished;

private:
	void close();
};

#endif