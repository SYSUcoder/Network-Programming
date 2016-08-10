#include <sys/socket.h> // sockfd and basic socket definitions
#include <sys/types.h> // basic socket definitions
#include <netinet/in.h> // IPPROTO_TCP and etc   sockaddr_in() and other Internet defns
#include <arpa/inet.h> // inet_pton   inet(3) functions
#include <unistd.h> // read()
#include "readAndWrite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAXLINE 4096
#define SERV_PORT  9877
#define SA struct sockaddr

void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];
	while (fgets(sendline, MAXLINE, fp) != NULL)
	{
		writen(sockfd, sendline, strlen(sendline));

		if (readline(sockfd, recvline, MAXLINE) == 0)
		{
			printf("str_cli: server terminated prematurely\n");
			exit(1);
		}

		fputs(recvline, stdout);
        memset(sendline, 0, sizeof(sendline));
		memset(recvline, 0, sizeof(recvline));
	}
}

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		printf("usage: tcpcli <IPaddress>\n");
		exit(1);
	}

	// socket()
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// create server address
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	// connet()
	connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd);

	exit(0);
}
