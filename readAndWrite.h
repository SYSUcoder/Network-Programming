#include <sys/socket.h> // sockfd and basic socket definitions
#include <sys/types.h> // basic socket definitions
#include <netinet/in.h> // IPPROTO_TCP and etc   sockaddr_in() and other Internet defns
#include <arpa/inet.h> // inet_pton   inet(3) functions
#include <unistd.h> // read()
#include <errno.h> // errno


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ssize_t readn(int fd, char *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;
	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ( (nread = read(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
				nread = 0;
			else
				return (-1);
		}
		else if (nread == 0)
			break;
		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft);
}

ssize_t writen(int fd, const char *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ( (nwritten = write(fd, ptr, nleft)) <= 0)
		{
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return (-1);
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}

ssize_t readline(int fd, char *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;
	bool flag;
	ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
		flag = false;
		while (1)
		{
			if ( (rc = read(fd, &c, 1)) == 1)
			{
				*ptr++ = c;
				if (c == '\n')
					flag = true;
			}
			else if (rc == 0)
			{
				*ptr = 0;
				return (n - 1);
			}
			else
			{
				if (errno == EINTR)
					continue;
				return (-1);
			}
			break;
		}
		if (flag)
			break;
	}
}