#ifndef SERVER_H
#define SERVER_H

#include "pch.h"

class Server
{
public:
	Server(uv_loop_t* loop, const char* host, int port);
	~Server();

	void start();

	uv_loop_t* _loop;

	static void on_connection(uv_stream_t* socket, int status);

private:
	struct sockaddr _addr;
	uv_tcp_t _socket;
	bool _ssl_ready;

	void init_ssl();
	SSL_CTX* _ssl_ctx;
};

#endif
