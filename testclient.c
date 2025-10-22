#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define token "7515996637:AAGz4Me9uTw2K-vXwK5SvD7oQ4iU_ZtG18w"

struct header {
	char* name;
	char* data;
};

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct hostent* h;
	if ((h = gethostbyname("api.telegram.org")) == NULL) {
		printf("gethostbyname");
		return 1;
	}

	printf("%s\n", inet_ntoa(*((struct in_addr*)h->h_addr)));
	struct sockaddr_in addr = {AF_INET, htons(443),
	                           *((struct in_addr*)h->h_addr)};

	connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));

	SSL_CTX* ctx = SSL_CTX_new(TLS_method());
	SSL* ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sockfd);
	SSL_connect(ssl);
	char* request = "GET /bot" token
	                "/getMe HTTP/1.1\r\nHost: api.telegram.org\r\n\r\n";
	SSL_write(ssl, request, strlen(request));
	printf("%s", request);
	char buffer[1024] = {0};
	SSL_read(ssl, buffer, 1023);
	printf("\033[1;33mResponse:\n\033[1;34m%s\033[0m\n", buffer);

	return 0;
}
