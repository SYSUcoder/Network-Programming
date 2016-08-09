#include <sys/socket.h> // sockfd and basic socket definitions
#include <sys/types.h> // basic socket definitions
#include <netinet/in.h> // IPPROTO_TCP and etc   sockaddr_in() and other Internet defns
#include <arpa/inet.h> // inet_pton   inet(3) functions
#include <unistd.h> // read()
#include <errno.h> // errno
#include "readAndWrite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAXLINE 4096
#define LISTENQ 1024
#define SERV_PORT  9877
#define SA struct sockaddr

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];

	while (1)
	{
		while ( (n = read(sockfd, buf, MAXLINE)) > 0)
		{
			writen(sockfd, buf, n);
			printf("%s", buf);
			memset(buf, 0, sizeof(buf));
		}
			

		// if read() was interrupted and it will set errno as EINTR
		if (n < 0 && errno == EINTR)
			continue;
		else if (n < 0)
		{
			printf("str_echo: read error");
			exit(1);
		}
		break;
	}
}

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	// socket()
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	// create ip address and port
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // htonl() return 32 bits
	servaddr.sin_port = htons(SERV_PORT); // htons() return 16 bits

	// bind()
	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	// listen()
	listen(listenfd, LISTENQ);

	for ( ; ; )
	{
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (SA *) &cliaddr, &clilen);

		if ( (childpid = fork()) == 0)
		{
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}
}