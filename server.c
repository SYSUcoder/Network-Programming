#include <sys/socket.h> // sockfd and basic socket definitions
#include <sys/types.h> // basic socket definitions
#include <netinet/in.h> // IPPROTO_TCP and etc   sockaddr_in and other Internet defns
#include <arpa/inet.h> // inet_pton   inet(3) functions htonl() & htons()
#include <unistd.h> // read()
#include <errno.h> // errno
#include <sys/wait.h> // waitpid()

#include "readAndWrite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MAXLINE 4096
#define LISTENQ 1024
#define SERV_PORT  9877
#define SA struct sockaddr

// echo to client
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

void showTextWithTime(int sockfd, struct sockaddr_in *cliaddr)
{
	ssize_t n;
	char buf[MAXLINE];
	time_t ticks;
	while (1)
	{
		while ( (n = read(sockfd, buf, MAXLINE)) > 0)
		{
			ticks = time(NULL);
			printf("(%.24s)\nFrom port %d: %s", ctime(&ticks), ntohs((*cliaddr).sin_port), buf);
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

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
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

	// call waitpid()
	signal(SIGCHLD, sig_chld);

	for ( ; ; )
	{
		clilen = sizeof(cliaddr);
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0)
		{
			if (errno == EINTR)
				continue;
			else
			{
				printf("accept error\n");
				exit(1);
			}
		}

		if ( (childpid = fork()) == 0)
		{
			close(listenfd);
			// str_echo(connfd);
			showTextWithTime(connfd, &cliaddr);
			exit(0);
		}
		close(connfd);
	}
}