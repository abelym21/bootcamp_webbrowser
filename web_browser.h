#ifndef __WEBBROWSER_H__
#define __WEBBROWSER_H__

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include<winsock2.h>
#include<Windows.h>
#include<Winbase.h>
#include<Ws2tcpip.h>
#include<Wininet.h>
#include<stdio.h>
#include<string.h>
#include<fstream>
#include<iostream>
#include<string>
#include<gdiplus.h>
#include <atlconv.h>

#include <queue>
#include <stack>
#include <map>
#include <list>
#include <iterator>

#define HTTP_PORT "80"
#define IP "52.192.132.151"
#define MESSAGE_LEN 8192	//8kb
//window
#define ID_EDIT 100
#define ID_STATIC 101
#define ID_PAGE 102
#define IDC_HSCROLL 103

//render
#define LINE_HEIGHT_SIZE 15
#define X_INIT_CURSOR 100
#define Y_INIT_CURSOR 100


class Socket
{
private:
	SOCKET sockfd;

public:
	Socket::Socket();
	SOCKET getSockFd();
	void setSockFd(SOCKET fd);
	int createSocket(struct addrinfo *pAddr);
};


class Connection
{
private:
	WSADATA wsaData;
	Socket sock;
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *addr;
	int setAddr(char *serverName, char *port);
public:
	Connection::Connection();
	int connecting(char *ip, char *port);
	int startConnect();
	int finishConnect(SOCKET sockfd);
	Socket getSocket();
};

class Communication
{
public:
	Communication();
	int sendRequestMessage(SOCKET sockfd, std::string uri);
	int sendRequestMessage(SOCKET sockfd, std::string uri, std::string serverName, std::string originalServerName);
	int recvResponseMessage(SOCKET sockfd);
	int recvImgResponseMessage(SOCKET sockfd);
	std::string getResponseAll();
	std::string getImgResponseAll();
	int clearResponseAll();
	int clearImgResponseAll();

private:
	std::string imgResponseAll;
	std::string responseAll;
};

class URLparser
{
public:
	URLparser();
	int parseUrl(char *url);
	std::string getServerName();
	std::string getUri();
	std::string getPort();
	std::string getUrl();
private:
	std::string port;
	std::string serverName;
	std::string uri;
	std::string originalUrl;
};

class HTMLnode
{
public:
	HTMLnode(std::string nodeName);
	~HTMLnode();
	int setText(std::string text);
	int setAttributes(std::string attributeName, std::string attributeValue);
	int addChild(HTMLnode *child);
	int setParent(HTMLnode *parent);
	std::string getText();
	std::map<std::string, std::string> getAttributes();
	std::list<HTMLnode*> getChilderen();
	HTMLnode* getParent();
	std::string getNodeName();

private:
	std::string nodeName;
	std::string text;
	std::map<std::string, std::string> attributes;
	std::list<HTMLnode*> children;
	HTMLnode* parent;
};

class HTMLparser
{
public:
	HTMLparser();
	~HTMLparser();
	int parsingHTML(std::string responseAll);
	HTMLnode* getDocumentNode();
	void testprint();
	boolean getIsParsed();
	int setIsParsed(boolean value);
	std::string removeResponseHeader(std::string response);

private:
	std::queue<char> token;
	std::string resultToken;
	std::map<std::string, boolean> HTMLstate;
	boolean isParsed = false;
	boolean isCloseTag = false;
	HTMLnode *document;
	std::stack<HTMLnode*> matchStack;	//열고 닫음을 체크하는 stack

	//std::list<std::string>parseList;

	std::string getResultToken();
	int checkTagToken(std::string token);
	int checkTextToken(std::string token);
	HTMLnode* settingAttribute(HTMLnode *newNode, std::string token, int tagPoint);
	int HTMLparser::setupParseTree(HTMLnode *newNode);
	int createParseTree();
	int tokenizing(std::string response);
};

class WindowHandlerPool
{
public:
	WindowHandlerPool();
	~WindowHandlerPool();
	int getHwndNum();
	std::vector<HWND> getHwndPool();
	int addHwnd(HWND hWnd);
	int deleteHWNDs();

private:
	int windowHandlerNum;
	std::vector<HWND> winHandlerPool;
};

class Render
{
public:
	ULONG_PTR g_GdiPlusTokenBoxData;
	Render(HTMLnode *document, URLparser *urlparser, WindowHandlerPool *HWNDpool, HINSTANCE hInst, HWND hWnd);
	int travellingParseTree(HDC hdc);
	~Render();

private:
	HTMLnode *document;
	URLparser *urlparser;
	WindowHandlerPool *HWNDpool;
	HINSTANCE hInst;
	HBITMAP hBmp;
	HWND hWnd;
	std::string strRead;
	int lengthPoint;
	int heightPoint;
	int renderingHTMLnode(HTMLnode *node, HDC hdc);
	int Render::renderingBrTag(HDC hdc);
	int renderingImgTag(HDC hdc, HTMLnode *node);
	int Render::rederingTextTag(HDC hdc, HTMLnode *node);
	int renderingInputTag(HDC hdc, HTMLnode *node);
};


#endif __WEBBROWSER_H__