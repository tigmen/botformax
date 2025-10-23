#include <arpa/inet.h>
#include <ctype.h>
#include <json-c/json.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define token "7515996637:AAGz4Me9uTw2K-vXwK5SvD7oQ4iU_ZtG18w"

#define IP_HOST "ipv4-internet.yandex.net"
#define IP_PATH "/api/v0/ip"

struct header {
	char* name;
	char* data;
};

#define NEXT_TOKEN "\r\n"
#define SIZE_TOKEN "Content-Length"
enum STATES { SKIP, PARSE, DATA, READ };

#define N 4096
void HTTP_parse(SSL* ssl) {
	enum STATES state = PARSE;
	char* buffer = malloc(N * sizeof(char));

	SSL_read(ssl, buffer, N);
	int parse_n = 0;
	int content_lenght = 0;
	for (char* p = buffer; *p; p++) {
		switch (state) {
			case PARSE:
				if (*p == '\r') {
					p++;
					if (*p == '\n') {
						state = READ;
						break;
					}
				} else if (*p == ':') {
					content_lenght = 0;
					state = DATA;
				} else if (*p != SIZE_TOKEN[parse_n++]) {
					parse_n = 0;
					state = SKIP;
				}
				break;
			case SKIP:
				if (*p == '\r') {
					p++;
					if (*p == '\n') {
						state = PARSE;
					}
				}
				break;
			case DATA:
				if (*p == '\r') {
					p++;
					if (*p == '\n') {
						state = PARSE;
					}
				} else if(isdigit(*p)) {
					content_lenght = content_lenght * 10 + *p - '0';
					printf("%c", *p);
				}
				break;
			default:
				break;
		}
		if (state == READ) {
			break;
		}
	}

	SSL_read(ssl, buffer, content_lenght);
	buffer[content_lenght] = '\0';
	printf("\n%s\n", buffer);
}

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent* h;
	if ((h = gethostbyname(IP_HOST)) == NULL) {
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
	char* request =
	    "GET /api/v0/ip "
	    "HTTP/1.1\r\nHost:ipv4-internet.yandex.net\r\n\r\n";
	SSL_write(ssl, request, strlen(request));
	printf("%s", request);

	HTTP_parse(ssl);

	return 0;
}
