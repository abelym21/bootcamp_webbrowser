#include "web_browser.h"


Render::Render(HTMLnode *document, URLparser *oldurlparser, WindowHandlerPool *HWNDpool,HINSTANCE hInst, HWND hWnd)
{
	this->document = document;
	this->urlparser = oldurlparser;
	this->hInst = hInst;
	this->hWnd = hWnd;
	this->HWNDpool = HWNDpool;
	this->lengthPoint = X_INIT_CURSOR;
	this->heightPoint = Y_INIT_CURSOR;
}

Render::~Render()
{
	//Gdiplus::GdiplusShutdown(g_GdiPlusTokenBoxData);
}


int Render::travellingParseTree(HDC hdc)
{
	std::stack<HTMLnode *>travellingStack;
	HTMLnode *ptr = document;
	HTMLnode *tmp;
	std::list<HTMLnode *>children;
	
	travellingStack.push(ptr);
	while (!travellingStack.empty())
	{
		ptr = travellingStack.top();
		travellingStack.pop();
		renderingHTMLnode(ptr, hdc);
		children = ptr->getChilderen();
		for (std::list<HTMLnode *>::reverse_iterator itor = children.rbegin(); itor != children.rend(); ++itor )
		{
			travellingStack.push(*itor);
		}
	}
	printf("end of travellingParserTree function\n");
	return 0;
}

int Render::renderingHTMLnode(HTMLnode *node, HDC hdc)
{
	std::string nodeName = node->getNodeName();
	std::list<HTMLnode*> children = node->getChilderen();

	if (nodeName == "text")
	{
		rederingTextTag(hdc, node);
	}
	else if (nodeName == "br")
	{
		renderingBrTag(hdc);
	}
	else if (nodeName == "img")
	{
		renderingImgTag(hdc, node);
	}
	else if (nodeName == "input")
	{
		renderingInputTag(hdc, node);
	}
	else if (nodeName == "form")
	{

	}
	else
	{
		//body, html, document 기타 등등.
	}

	return 0;
}

int Render::rederingTextTag(HDC hdc, HTMLnode *node)
{
	int fontsize;
	HFONT font, oldfont;

	if (node->getParent()->getNodeName() == "h1")
	{
		//drawText
		fontsize = 50;
		font = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "궁서");
		oldfont = (HFONT)SelectObject(hdc, font);
		TextOut(hdc, lengthPoint, heightPoint, node->getText().c_str(), node->getText().size());
		SelectObject(hdc, oldfont);
		DeleteObject(font);
		heightPoint = heightPoint + fontsize + LINE_HEIGHT_SIZE;

	}
	else if (node->getParent()->getNodeName() == "p")
	{
		TextOut(hdc, lengthPoint, heightPoint, node->getText().c_str(), node->getText().size());
		heightPoint = heightPoint + LINE_HEIGHT_SIZE;
	}
	else
	{
		TextOut(hdc, lengthPoint, heightPoint, node->getText().c_str(), node->getText().size());
		heightPoint = heightPoint + 30 + LINE_HEIGHT_SIZE;
		//lengthPoint = lengthPoint + 2 * node->getText().size();
	}

	return 0;
}


int Render::renderingBrTag(HDC hdc)
{
	heightPoint = heightPoint + LINE_HEIGHT_SIZE;
	lengthPoint = X_INIT_CURSOR;
	return 0;
}

int Render::renderingImgTag(HDC hdc, HTMLnode *node)
{
	HANDLE fp;
	SOCKET sockfd;
	std::string newFileName;
	std::string response;
	std::string strTmp;
	std::string newURI;
	std::string serverName;
	std::string port;

	Connection connection = Connection();
	Communication communication = Communication();
	HTMLparser htmlparser = HTMLparser();
	URLparser *newUrlparser = new URLparser();

	std::string imgURI;
	std::string imgAlt;



	if (node->getAttributes().count("src"))
	{
		imgURI = node->getAttributes()["src"];
	}
	else if(node->getAttributes().count("data-src"))
	{
		imgURI = node->getAttributes()["data-src"];
	}
	
	if (node->getAttributes().count("alt"))
	{
		imgAlt = node->getAttributes()["alt"];
	}

	int fileNamePointer = 0;
	
	int imgLength;
	int imgHeight;

	newURI.clear();
	strTmp.clear();
	response.clear();

	//[소스코드] : 이미 존재하는 파일인지 확인 하는 방법.

	if (imgURI.find("http://", 0) == -1)		//imgURI가 상대 경로일 때
	{
		newURI = urlparser->getUri().substr(0, urlparser->getUri().rfind("/") + 1);	//새로은 URI 만들기
		//imgURI의 맨 앞에 / 가 있는 경우
		if (imgURI.at(0) == '/')
		{
			newURI.append(imgURI.substr(1, imgURI.size()-1));
		}
		else	//imgURI의 맨 앞에 / 가 없는 경우
		{
			newURI.append(imgURI);
		}
		serverName = urlparser->getServerName();
		port = urlparser->getPort();
	}
	else	//imgURI가 절대 경로일 때
	{
		newUrlparser->parseUrl(&(imgURI[0]));	//[확인] : 이게 가능한지
		newURI = newUrlparser->getUri();
		serverName = newUrlparser->getServerName();
		port = newUrlparser->getPort();
	}


	//새로운 파일 이름 설정
	fileNamePointer = newURI.rfind("/");
	newFileName = newURI.substr(fileNamePointer + 1, newURI.size() - fileNamePointer - 1);
	//로컬에 없는 파일이면 통신을 통해 저장한다
	std::ifstream fileToRead(newFileName);
	if (!fileToRead.is_open())
	{


		connection.startConnect();	//연결 시작
		connection.connecting(&(serverName[0]), &(port[0]));	//연결~
		communication.clearImgResponseAll();
		communication.sendRequestMessage(connection.getSocket().getSockFd(), newURI, serverName, urlparser->getServerName());	//request 메시지 send--> 이미지 전용 메소드
		communication.recvImgResponseMessage(connection.getSocket().getSockFd());	//response 메시지 recv
		response = htmlparser.removeResponseHeader(communication.getImgResponseAll());
		connection.finishConnect(connection.getSocket().getSockFd());
	
		//파일 저장
		std::ofstream outFile(newFileName, std::ofstream::binary);
		outFile << response << std::endl;
		outFile.close();
	}

	
	USES_CONVERSION;	//유니코드 변환을 위해
	
	Gdiplus::Graphics graphics(hdc);//Gdiplus
	Gdiplus::Image *pl;
	pl = Gdiplus::Image::FromFile(A2W(newFileName.c_str()));
	
	
	if (!node->getAttributes().count("width"))
	{
		imgLength = pl->GetWidth();
	}
	else
	{
		imgLength = atoi(node->getAttributes()["width"].c_str());
	}

	if (!node->getAttributes().count("height"))
	{
		imgHeight = pl->GetHeight();
	}
	else
	{
		imgHeight = atoi(node->getAttributes()["height"].c_str());
	}
	


	graphics.DrawImage(pl,lengthPoint, heightPoint, imgLength, imgHeight );

	////test
	//printf("lengthPoint : %d\n", lengthPoint);
	//printf("heightPoint : %d\n", heightPoint);
	//printf("imgLength : %d\n", imgLength);
	//printf("imgHeight : %d\n", imgHeight);

	lengthPoint = X_INIT_CURSOR;
	heightPoint = heightPoint + imgHeight + LINE_HEIGHT_SIZE;
	delete pl;
	return 0;
}

int Render::renderingInputTag(HDC hdc, HTMLnode *node)
{
	HWND hEdit;
	int inputElementWidth;
	int inputElementHeight;
	int i = 0;

	if (node->getAttributes()["type"] == "text")
	{
		inputElementHeight = 25;
		inputElementWidth = 300;

		hEdit = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, lengthPoint, heightPoint, inputElementWidth, inputElementHeight, hWnd, (HMENU)ID_EDIT, hInst, NULL);
		HWNDpool->addHwnd(hEdit);
		heightPoint = heightPoint + inputElementHeight + LINE_HEIGHT_SIZE;
	}
	else if (node->getAttributes()["type"] == "submit")
	{
		inputElementHeight = 25;
		inputElementWidth = 100;

		hEdit = CreateWindow(TEXT("button"), &(node->getAttributes()["value"][0]), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, lengthPoint, heightPoint, inputElementWidth, inputElementHeight, hWnd, (HMENU)1, hInst, NULL);
		HWNDpool->addHwnd(hEdit);
		heightPoint = heightPoint + inputElementHeight + LINE_HEIGHT_SIZE;
	}
	return 0;
}