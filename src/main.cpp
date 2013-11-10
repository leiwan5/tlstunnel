#include "pch.h"
#include "server.h"

int main()
{
	Server s(uv_default_loop(), "127.0.0.1", 4433);
	s.start();
	
	return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}



//
//const char* client_key_file = "client.key.nopass.pem";
//
//void on_written_callback(uv_write_t* req, int status) { }
//
//void write_to_socket(Client* c, char* buf, size_t len) {
//  if(len <= 0) {
//    return;
//  }
//  uv_buf_t uvbuf;
//  uvbuf.base = buf;
//  uvbuf.len = len;
//  int r = uv_write(&c->write_req, (uv_stream_t*)&c->socket, &uvbuf, 1, on_written_callback);
//  if(r < 0) {
//    printf("ERROR: write_to_socket error: %s\n", uv_err_name(r));
//  }
//}
// uh  
//void flush_read_bio(Client* c) {
//  char buf[1024*16];
//  int bytes_read = 0;
//  while((bytes_read = BIO_read(c->write_bio, buf, sizeof(buf))) > 0) {
//    write_to_socket(c, buf, bytes_read);
//  }
//}
//
//void handle_error(Client* c, int result) {
//  int error = SSL_get_error(c->ssl, result);
//  if(error == SSL_ERROR_WANT_READ) {
//    flush_read_bio(c);
//  }
//}
//
//void check_outgoing_application_data(Client* c) {    
//  if(SSL_is_init_finished(c->ssl)) {
//    if(c->buffer_out.size() > 0) {
//
//      //std::copy(c->buffer_out.begin(), c->buffer_out.end(), std::ostream_iterator<char>(std::cout,""));
//      int r = SSL_write(c->ssl, &c->buffer_out[0], c->buffer_out.size());
//      c->buffer_out.clear();
//      handle_error(c, r);
//      flush_read_bio(c);
//    }
//  }
//}
//
//void on_event(Client* c) {
//	int a = SSL_is_init_finished(c->ssl);
//	if(!SSL_is_init_finished(c->ssl)) {
//		std::cout << "SSL_is_init_finished x" << std::endl;
//		int r = SSL_connect(c->ssl);
//		if(r < 0) {
//			handle_error(c, r);
//		}
//		check_outgoing_application_data(c);
//	}else{
//		char buf[1024 * 10];
//		int bytes_read = 0;
//		// connect, check if there is encrypted data, or we need to send app data
//		int r = SSL_read(c->ssl, buf, sizeof(buf));
//		if(r < 0) {
//		  handle_error(c, r);
//		}
//		else if(r > 0) {
//		  std::copy(buf, buf+r, std::back_inserter(c->buffer_in));
//		  std::copy(c->buffer_in.begin(), c->buffer_in.end(), std::ostream_iterator<char>(std::cout));
//		  c->buffer_in.clear();
//		}
//		check_outgoing_application_data(c);
//	}
//};
//void on_read_callback(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf) {
//  Client* c = static_cast<Client*>(tcp->data);
//  if(nread == -1) { // disconnected (?)
//    char plain_buf[1024*10];
//    int r = SSL_read(c->ssl, plain_buf, sizeof(plain_buf));
//    if(r < 0) {
//      handle_error(c, r);
//    }
//    else if(r > 0) {
//      std::copy(plain_buf, plain_buf+r, std::back_inserter(c->buffer_in));
//    }
//
//	std::cout << "got size:" << c->buffer_in.size() << std::endl;
//    std::copy(c->buffer_in.begin(), c->buffer_in.end(), std::ostream_iterator<char>(std::cout));
//    ::exit(0);
//  } else if(nread == 0) {
//	  std::cout << "read done" << std::endl;
//  }
//
//  int written = BIO_write(c->read_bio, buf->base, nread);     
//  on_event(c);
//}
//
//void on_alloc_callback(uv_handle_t* con, size_t size, uv_buf_t* buf) {
//  static char slab[65536];
//  buf->base = slab;
//  buf->len = sizeof(slab);
//}
//
//void on_connect_callback(uv_connect_t* con, int status) {
//	Client* c = static_cast<Client*>(con->data);
//	if(status != 0) {
//		std::cout << "on_connect_callback error" << std::endl;
//		exit(1);
//	}
//	int r = uv_read_start((uv_stream_t*)&c->socket, on_alloc_callback, on_read_callback);
//	  if(r < 0) {
//		std::cout << "ERROR: uv_read_start error: " << uv_err_name(r) << std::endl;
//		exit(1);
//	  }
//
//	c->addAppData("hello");
//	std::cout << "app data size: " << c->buffer_out.size() << std::endl;
//	c->ssl = SSL_new(c->ssl_ctx);
//	c->read_bio = BIO_new(BIO_s_mem());
//	c->write_bio = BIO_new(BIO_s_mem());
//	SSL_set_bio(c->ssl, c->read_bio, c->write_bio);
//	SSL_set_connect_state(c->ssl);
//	r = SSL_do_handshake(c->ssl);
//	on_event(c);
//}
//int main(){
//	SSL_library_init();
//	SSL_load_error_strings();
//
//	BIO* bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
//	SSL_CTX* ssl_ctx = SSL_CTX_new(SSLv23_client_method());
//
//	int rc = SSL_CTX_load_verify_locations(ssl_ctx, "cacert.pem", NULL);
//	if(!rc) {
//		std::cout << "Could not load cacert.\n";
//		return 1;
//	}
//	
//	rc = SSL_CTX_use_certificate_file(ssl_ctx, "client.pem", SSL_FILETYPE_PEM);
//	if(!rc) {
//		std::cout << "Could not load cert.\n";
//		return 1;
//	}
//	
//	rc = SSL_CTX_use_PrivateKey_file(ssl_ctx, client_key_file, SSL_FILETYPE_PEM);
//	if(!rc) {
//		std::cout << "Could not load key.\n";
//		return 1;
//	}
//
//	SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2); 
//	SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);
//	//SSL_CTX_set_info_callback(ssl_ctx, NULL);
//	//SSL_CTX_set_msg_callback(ssl_ctx, NULL);
//
//	uv_loop_t* loop = uv_loop_new();
//	Client c;
//	c.loop = loop;
//	c.ssl_ctx = ssl_ctx;
//
//	struct sockaddr_in addr;
//	rc = uv_ip4_addr("127.0.0.1", 4433, &addr);
//	std::cout << rc << std::endl;
//
//	uv_tcp_init(c.loop, &c.socket);
//	uv_tcp_connect(&c.connect_req, &c.socket, (sockaddr*)&addr, on_connect_callback);
//	return uv_run(loop, UV_RUN_DEFAULT);
//}
