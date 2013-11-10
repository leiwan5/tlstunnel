#include "pch.h"
#include "connection.h"

void reportError(SSL* ssl, int result)
{
	if (result <= 0)
	{
		int error = SSL_get_error(ssl, result);

		switch (error)
		{
		case SSL_ERROR_ZERO_RETURN:
			std::cout << "SSL_ERROR_ZERO_RETURN" << std::endl;
			break;
		case SSL_ERROR_NONE:
			std::cout << "SSL_ERROR_NONE" << std::endl;
			break;
		case SSL_ERROR_WANT_READ:
			std::cout << "SSL_ERROR_WANT_READ" << std::endl;
			break;
		default:
			{
				char buffer[256];

				while (error != 0)
				{
					ERR_error_string_n(error, buffer, sizeof(buffer));

					std::cout << "Error: " << error << " - " << buffer << std::endl;

					error = ERR_get_error();
				}
			}

			break;
		}
	}
}

Connection::Connection(SSL_CTX* ssl_ctx, uv_tcp_t* socket):_ssl_init_finished(false),_ssl_handshake_finished(false)
{
	_ssl_ctx = ssl_ctx;
	_ssl = SSL_new(_ssl_ctx);
	_server = (Server*)socket->data;
	_socket = socket;
	_socket->data = this;
	_read_bio = BIO_new(BIO_s_mem());
	_write_bio = BIO_new(BIO_s_mem());
	SSL_set_bio(_ssl, _read_bio, _write_bio);
	SSL_set_accept_state(_ssl);
	uv_read_start((uv_stream_t*)_socket, Connection::on_alloc, Connection::on_read);
}

Connection::~Connection()
{
	BIO_free(_write_bio);
	BIO_free(_read_bio);
	SSL_free(_ssl);
	std::cout << "client closed" << std::endl;
}

void Connection::close()
{
	uv_shutdown_t* req = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
	uv_shutdown(req, (uv_stream_t*)_socket, Connection::on_shutdown);
}

void Connection::write_to_socket(char* buf, unsigned int size)
{
	uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_buf_t b = uv_buf_init(buf, size);

	uv_write(req, (uv_stream_t*)_socket, &b, 1, Connection::on_write);
}

void Connection::flush_write_bio()
{
	char buf[65536];
	memset(buf, 0, sizeof(buf));
	int bytes = 0;
	while((bytes = BIO_read(_write_bio, buf, sizeof(buf))) > 0)
	{
		write_to_socket(buf, bytes);
	}
}

void Connection::procced()
{
	if(!SSL_is_init_finished(_ssl))
	{
		std::cout << "SSL_is_init_finished x" << std::endl;
		int r = SSL_accept(_ssl);
		if(r <= 0)
		{
			std::cout << "SSL_accept x" << std::endl;
			if(SSL_get_error(_ssl, r) == SSL_ERROR_WANT_READ)
			{
				flush_write_bio();
			}
		}
		else
		{
			std::cout << "SSL_accept o" << std::endl;
			//SSL_write(_ssl, "hello", 5);
			flush_write_bio();
		}

	}
	else
	{
		std::cout << "SSL_is_init_finished o" << std::endl;
		int nread = 0;
		char* buf = (char*)malloc(10240);
		while((nread = SSL_read(_ssl, buf, sizeof(buf))) > 0)
		{
			std::copy(buf, buf+nread, std::ostream_iterator<char>(std::cout));
		}
		free(buf);
	}
}

//////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////

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
		int nwritten = 0;
		while(nwritten < nread) {
			std::cout << "written bytes: " << nwritten <<  " of " << nread << std::endl;
			nwritten += BIO_write(conn->_read_bio, buf->base + nwritten, nread - nwritten);
		}

		conn->procced();
	}

	if(buf->base) free(buf->base);
}

void Connection::on_alloc(uv_handle_t* con, size_t size, uv_buf_t* buf)
{
	buf->base = (char*)malloc(size);
	buf->len = size;
}

void Connection::on_shutdown(uv_shutdown_t* req, int status)
{
	uv_close((uv_handle_t*)req->handle, Connection::on_close);
	free(req);
}

void Connection::on_close(uv_handle_t* handle)
{
	Connection* conn = (Connection*)handle->data;
	delete conn;
	free(handle);
}

void Connection::on_write(uv_write_t* req, int status)
{
	//write_req* wr = (write_req*)req->data;
	// if(wr->buf.base) free(wr->buf.base);
	free(req);
}