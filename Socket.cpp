#include "web_browser.h"

Socket::Socket()
{
	this->sockfd = INVALID_SOCKET;
}


int Socket::createSocket(struct addrinfo *pAddr)
{
	sockfd = socket(pAddr->ai_family, pAddr->ai_socktype, pAddr->ai_protocol);
	
	if (sockfd == INVALID_SOCKET)
	{
		printf("Error : creatSocket function error.\n");
		return 1;
	}

	return 0;
}

//getter
SOCKET Socket::getSockFd()
{
	return this->sockfd;
}

//setter
void Socket::setSockFd(SOCKET fd)
{
	this->sockfd = fd;
}


