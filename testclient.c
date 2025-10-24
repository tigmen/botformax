#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "errno.h"
#include "trace/trace.h"

#define TOKEN "7515996637:AAGz4Me9uTw2K-vXwK5SvD7oQ4iU_ZtG18w"

#define IP_HOST "api.telegram.org"
#define IP_PATH "/bot" TOKEN "/getMe"

#define PORT "443"

int main() {
	trace_init("log", INFO);

	SSL_library_init();
	SSL_CTX* ctx = SSL_CTX_new(TLS_method());
	if (ctx == NULL) {
		trace(ERROR, "%s %d: Error creating SSL context\n", __FILE__,
		      __LINE__);
	}

	struct addrinfo host_info, *res;
	host_info.ai_family = AF_INET;
	host_info.ai_socktype = SOCK_STREAM;
	host_info.ai_protocol = IPPROTO_TCP;
	host_info.ai_flags = 0;

	int status;
	status = getaddrinfo(IP_HOST, PORT, &host_info, &res);
	if (status) {
		trace(ERROR, "%s %d: Error looking up for host %s\n", __FILE__,
		      __LINE__, IP_HOST);
		exit(0);
	}

	int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sd < 0) {
		trace(ERROR, "%s %d: Error creating socket: %s\n", __FILE__,
		      __LINE__, strerror(errno));
		exit(0);
	}

	status = connect(sd, res->ai_addr, res->ai_addrlen);
	if (status) {
		trace(ERROR, "%s %d: Error connecting to host %s port %s: %s\n",
		      __FILE__, __LINE__, IP_HOST, PORT, strerror(errno));
		exit(0);
	}
	freeaddrinfo(res);

	SSL* ssl = SSL_new(ctx);
	if (ssl == NULL) {
		trace(ERROR, "%s %d: Error init SSL\n", __FILE__, __LINE__);
	}

	SSL_set_fd(ssl, sd);
	status = SSL_connect(ssl);
	if (status < 0) {
		trace(ERROR, "%s %d: Error connecting SSL handshake\n",
		      __FILE__, __LINE__);
	}

	char* request = "GET " IP_PATH " HTTP/1.1\r\nHost:" IP_HOST "\r\n\r\n";

	status = SSL_write(ssl, request, strlen(request));
	if (status < 0) {
		trace(ERROR, "%s %d: Error sending request: %d\n", __FILE__,
		      __LINE__, strerror(errno));
		exit(0);
	}
	trace(INFO, "%s", request);

	SSL_free(ssl);
	SSL_CTX_free(ctx);

	return 0;
}
