#include "web_browser.h"


Communication::Communication()
{
	this->responseAll.clear();
}

int Communication::sendRequestMessage(SOCKET sockfd, std::string uri)
{
	int sendInt;
	std::string request;
	
	request.append("GET ");
	request.append(uri);
	request.append(" HTTP/1.1\r\n\r\n");

	sendInt = send(sockfd, request.c_str(), request.length(),0);

	printf("----------------------------------------------------\n");
	printf("requestMessage : \n");
	printf("%s\n", request.c_str());
	printf("----------------------------------------------------\n");

	request.clear();

	return sendInt;
}

int Communication::sendRequestMessage(SOCKET sockfd, std::string uri, std::string serverName, std::string originalServerName)
{
	int sendInt;
	std::string request;

	request.append("GET ");
	request.append(uri);
	//test
	request.append(" HTTP/1.1\r\n");
	request.append("Host: " + serverName + "\r\n\r\n");
	//request.append("Connection: keep-alive\r\n");
	//request.append("Cache-Control: max-age=0\r\n");
	//request.append("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n");
	//request.append("Upgrade-Insecure-Requests: 1\r\n");
	//request.append("User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/48.0.2564.116 Safari/537.36\r\n");
	//request.append("Referer: http://"+originalServerName+"/"+"\r\n");
	//request.append("Accept-Encoding: gzip, deflate, sdch\r\n");
	//request.append("Accept-Language: ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4\r\n\r\n");

	

	sendInt = send(sockfd, request.c_str(), request.length(), 0);

	printf("----------------------------------------------------\n");
	printf("requestMessage : \n");
	printf("%s\n", request.c_str());
	printf("----------------------------------------------------\n");

	request.clear();

	return sendInt;
}

int Communication::recvResponseMessage(SOCKET sockfd)
{
	//timeout 처리를 위한 변수
	//struct timeval tv;
	//tv.tv_sec = 2000;
	//tv.tv_usec = 0;
	
	int recvBytes = -1;
	char response[MESSAGE_LEN] = {'\0'};

	do{

		memset(response, 0x00, MESSAGE_LEN);
		recvBytes = recv(sockfd, response, sizeof(response), 0);
		/*
		if (SOCKET_ERROR == setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)))
		{
			printf("[error] : recv function time out.\n");
			return 1;
		}*/

		responseAll.append(response, recvBytes);

		if (responseAll.rfind("</html>", responseAll.size()) != std::string::npos)
		{
			break;
		}

	} while (recvBytes > 0);

	return recvBytes;
}


int Communication::recvImgResponseMessage(SOCKET sockfd)
{
	//timeout 처리를 위한 변수
	//struct timeval tv;
	//tv.tv_sec = 2000;
	//tv.tv_usec = 0;

	int recvBytes = -1;
	char response[MESSAGE_LEN] = { '\0' };

	do {

		memset(response, 0x00, MESSAGE_LEN);
		recvBytes = recv(sockfd, response, sizeof(response), 0);
		/*
		if (SOCKET_ERROR == setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)))
		{
		printf("[error] : recv function time out.\n");
		return 1;
		}*/

		imgResponseAll.append(response, recvBytes);

	} while (recvBytes > 0);

	return recvBytes;
}

std::string Communication::getResponseAll()
{
	return this->responseAll;
}

int Communication::clearResponseAll()
{
	this->responseAll.clear();
	
	return 0;
}

std::string Communication::getImgResponseAll()
{
	return this->imgResponseAll;
}

int Communication::clearImgResponseAll()
{
	this->imgResponseAll.clear();

	return 0;
}