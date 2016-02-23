#include "web_browser.h"

Connection::Connection()
{
	sock = Socket();
	this->result = NULL;
	this->hints = {NULL};
}

int Connection::connecting(char *serverName, char *port)
{
	int tmp = 0;
	struct addrinfo *pAddr = NULL;

	setAddr(serverName, port);

	for (pAddr = result; pAddr != NULL; pAddr = pAddr->ai_next)
	{
		sock.createSocket(pAddr);
		tmp = connect(sock.getSockFd(), pAddr->ai_addr, (int)pAddr->ai_addrlen);

		if ( tmp == SOCKET_ERROR)
		{
			closesocket(sock.getSockFd());
			sock.setSockFd(INVALID_SOCKET);
			continue;
		}

		break;
	}

	freeaddrinfo(result);
	
	if (tmp == SOCKET_ERROR)
	{
		printf("Error : connecting function error.\n");
		return 1;
	}

	return 0;
}

int Connection::startConnect()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Error : WSAStartup function error.\n");
		return 1;
	}

	return 0;
}

int Connection::finishConnect(SOCKET sockfd)
{
	closesocket(sockfd);
	WSACleanup();
	return 0;
}


int Connection::setAddr(char *ip, char *port)
{
	DWORD dwRetval;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//IPv4, IPv6 모두 가능
	hints.ai_socktype = SOCK_STREAM;	//TCP 통신
	hints.ai_protocol = IPPROTO_TCP;	//TCP 통신

	dwRetval = getaddrinfo(ip, port, &hints, &result);

	if (dwRetval != 0)
	{
		printf("Error : getaddrinfo function error.\n");
		WSACleanup();
		return 1;
	}

	return 0;
}

//getter

Socket Connection::getSocket()
{
	return this->sock;
}