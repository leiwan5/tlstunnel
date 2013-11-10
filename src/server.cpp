#include "pch.h"
#include "server.h"
#include "connection.h"

Server::Server(uv_loop_t* loop, const char* host, int port):_ssl_ready(false),_ssl_ctx(NULL)
{
	init_ssl();
	_loop = loop;
	uv_ip4_addr(host, port, (sockaddr_in*)&_addr);
	uv_tcp_init(loop, &_socket);
	uv_tcp_bind(&_socket, &_addr);
	_socket.data = this;
}

Server::~Server(void)
{
}

void Server::start()
{
	if(_ssl_ready) uv_listen((uv_stream_t*)&_socket, 10240, Server::on_connection);
}

void Server::init_ssl()
{
	SSL_library_init();
	SSL_load_error_strings();

	_ssl_ctx = SSL_CTX_new(SSLv23_server_method());

	int rc = SSL_CTX_load_verify_locations(_ssl_ctx, "cacert.pem", NULL);
	if(!rc) {
		std::cout << "Could not load cacert.\n";
		return;
	}
	
	rc = SSL_CTX_use_certificate_file(_ssl_ctx, "server.pem", SSL_FILETYPE_PEM);
	if(!rc) {
		std::cout << "Could not load cert.\n";
		return;
	}
	
	rc = SSL_CTX_use_PrivateKey_file(_ssl_ctx, "server.key.nopass.pem", SSL_FILETYPE_PEM);
	if(!rc) {
		std::cout << "Could not load key.\n";
		return;
	}

	SSL_CTX_set_options(_ssl_ctx, SSL_OP_NO_SSLv2); 
	SSL_CTX_set_verify(_ssl_ctx, SSL_VERIFY_PEER, NULL);

	_ssl_ready = true;
}

//////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////

void Server::on_connection(uv_stream_t* socket, int status)
{
	Server* server = (Server*)socket->data;
	if(status == 0)
	{
		uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
		uv_tcp_init(server->_loop, client);
		client->data = server;
		uv_accept(socket, (uv_stream_t*)client);
		Connection* conn = new Connection(server->_ssl_ctx, client);
	}
}

