#include "web_browser.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
SOCKET sockfd;
Connection *connection = new Connection();
Communication *communication = new Communication();
URLparser *urlparser =  new URLparser();
HTMLparser *htmlparser = new HTMLparser();
WindowHandlerPool *HWNDpool = new WindowHandlerPool();
boolean  g_flag =false;
LPCTSTR lpszClass = TEXT("WebBrowser");
HWND hEdit;
HWND hMain;
WNDPROC originalProc;	//hEdit subclassing�� ���� ���� ����.
//scrollbar
HWND hWndHorzScroll;
HWND hWndVertScroll;
RECT rMain;
int hScrollPos = 0;
int vScrollPos = 0;

//GdiPlus code
Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

//���� ���α׷���
char url[INTERNET_MAX_URL_LENGTH];

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG message;
	WNDCLASS wndClass;
	
	//window class ����
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = lpszClass;
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	g_hInst = hInstance;	//winproc���� ����ϱ� ���� �������� ��� (�ν��Ͻ� �ڵ鷯)
	RegisterClass(&wndClass);		//window class ���
	//window ����
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);
	hMain = hWnd;
	ShowWindow(hWnd, nCmdShow);	//window ���
	//printf("left : %d \n right : %d \n top : %d \n botton : %d\n", rMain.left, rMain.right, rMain.top, rMain.bottom);
	////printf("left : %d \n right : %d \n top : %d \n botton : %d\n", mainRect.left, mainRect.right, mainRect.top, mainRect.bottom);
	//rMain = mainRect;
	//�޽��� ����
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	Render *render;
	RECT rc;
	//int iHThumb;
	//int iVThumb;
	SCROLLINFO si;
	//int result;

	switch (iMessage) {

	case WM_CREATE:	//window â�� �������ڸ��� Edit��URI�� ����.
		hEdit = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 70, 10, 500, 25, hWnd, (HMENU)ID_EDIT, g_hInst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("URL"), WS_VISIBLE | WS_CHILD, 20, 10, 50, 25, hWnd, (HMENU)ID_STATIC, NULL, NULL);
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);	//Gdiplus code
		originalProc = (WNDPROC)SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)EditProc);	//set subclass
		//scroll bar
		//iHThumb = GetSystemMetrics(SM_CXHTHUMB); ->  ������� ũ�⸦ �޾ƿ��� �Լ�.
		//iVThumb = GetSystemMetrics(SM_CYVTHUMB);
		GetClientRect(hWnd, &rMain);	//Main Window ��ǥ�� �޾ƿ���
		hWndHorzScroll = CreateWindow("SCROLLBAR", (LPSTR)NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN, rMain.left, rMain.bottom - 20, rMain.right - rMain.left - 20, 20, hWnd, (HMENU)IDC_HSCROLL, g_hInst, NULL);
		SetScrollRange(hWndHorzScroll, SB_CTL, rMain.left, rMain.right, TRUE);
		SetScrollPos(hWndHorzScroll, SB_CTL, rMain.left, TRUE);
		hWndVertScroll = CreateWindow("SCROLLBAR", (LPSTR)NULL, WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_RIGHTALIGN, rMain.right - 20, rMain.top, 20, rMain.bottom - 20, hWnd, (HMENU)IDC_HSCROLL, g_hInst, NULL);
		SetScrollRange(hWndVertScroll, SB_CTL, rMain.top, rMain.bottom, TRUE);
		SetScrollPos(hWndVertScroll, SB_CTL, rMain.top, TRUE);
		SetFocus(hEdit);
		return 0;

	case WM_SIZE:	//window�� ũ�Ⱑ ���� ���� ��, ȣ��.
		GetClientRect(hWnd, &rMain);	//Main Window ��ǥ�� �޾ƿ���
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
		si.nMin = rMain.left;
		si.nMax = rMain.right;
		si.nPage = LOWORD(lParam);	//lParam�� �������� : ���� �� ���� ������ ��.
		si.nPos = hScrollPos;
		SetScrollInfo(hWndHorzScroll, SB_HORZ, &si, TRUE);
		//InvalidateRect(hWnd, NULL, TRUE);
		RedrawWindow(hMain, 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
		return 0;

	case WM_COMMAND:
	
		switch (LOWORD(wParam))
		{
		case ID_EDIT:
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:	//Edit â�� Ű���� �Է��� ���ö����� url �迭�� ����.
				GetWindowText(hEdit, url, sizeof(url));
				break;
			}
		}

		return 0;

	case WM_HSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINERIGHT:
			hScrollPos = min(rMain.right, hScrollPos + 100);
			break;
		case SB_LINELEFT:
			hScrollPos = max(rMain.left, hScrollPos - 100);
			break;
		case SB_PAGELEFT:
			hScrollPos = max(rMain.left, hScrollPos - 100);
			break;
		case SB_PAGERIGHT:
			hScrollPos = min(rMain.right, hScrollPos + 100);
			break;
		}
		SetScrollPos((HWND)lParam, SB_CTL, hScrollPos, TRUE);
		RedrawWindow(hMain, 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
		return 0;

	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			vScrollPos = max(rMain.top, vScrollPos - 100);
			break;
		case SB_LINEDOWN:
			vScrollPos = min(rMain.bottom, vScrollPos + 100);
			break;
		case SB_PAGEUP:
			vScrollPos = max(rMain.top, vScrollPos - 100);
			break;
		case SB_PAGEDOWN:
			vScrollPos = min(rMain.bottom, vScrollPos + 100);
			break;
		}
		SetScrollPos((HWND)lParam, SB_CTL, vScrollPos, TRUE);
		RedrawWindow(hMain, 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
		return 0;

	case WM_SETFOCUS:
		return 0;

	case WM_PAINT:

		/*if (htmlparser->getIsParsed())
		{
			htmlparser->setIsParsed(false);
			render = new Render(htmlparser->getDocumentNode(), urlparser, HWNDpool,g_hInst, hWnd);
			htmlparser->setIsParsed(false);
			render->travellingParseTree(hdc);
		}*/
		//GetClientRect(hWnd, &rMain);	//Main Window ��ǥ�� �޾ƿ���
		if (g_flag)
		{
			printf("render tree print\n");
			//g_flag = false;
			hdc = BeginPaint(hWnd, &ps);
			render = new Render(htmlparser->getDocumentNode(), urlparser, HWNDpool, g_hInst, hWnd);
			render->travellingParseTree(hdc);
			EndPaint(hWnd, &ps);
		}
		else
		{
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rMain);	//Main Window ��ǥ�� �޾ƿ���
			
			TextOut(hdc, 300, 300, "Enter URL........", strlen("Enter URL........"));
			EndPaint(hWnd, &ps);
		}

		//htmlparser->setIsParsed(true);
		//Sleep(5000);
		return 0;

	case WM_DESTROY:
		Gdiplus::GdiplusShutdown(gdiplusToken);	//closeGdiplus
		SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)originalProc);	//close subclass
		PostQuitMessage(0);
		
		return 0;

	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

LRESULT CALLBACK EditProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{	

	switch (iMessage)
	{
	case WM_KEYDOWN:
	
		if (wParam == VK_RETURN)
		{
			g_flag = false;
			communication->clearResponseAll();
			HWNDpool->deleteHWNDs();
			//DestroyWindow(hEdit);
			urlparser->parseUrl(url);			//����ڰ� �Է��� URL �Ľ�
			connection->startConnect();			//���� ����
			connection->connecting(&(urlparser->getServerName()[0]), &(urlparser->getPort()[0]));//����~
			sockfd = connection->getSocket().getSockFd();
			communication->sendRequestMessage(sockfd, urlparser->getUri());	//request �޽��� send
			communication->recvResponseMessage(connection->getSocket().getSockFd());	//response �޽��� recv
			connection->finishConnect(sockfd);
			htmlparser->parsingHTML(communication->getResponseAll());
			//htmlparser->setIsParsed(true);
			g_flag = true;
			//InvalidateRect(hMain, NULL, true);
			
			RedrawWindow(hMain, 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
			//RedrawWindow(hMain, 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_ALLCHILDREN );
			SetFocus(hMain);	//hMain���� focus�� �Ѱ��ش�.
			printf("reddrqWindow end\n");
			return 0;
		}
	case WM_SETFOCUS:
		//htmlparser->setIsParsed(false);
		//g_flag = false;
		return 0;

	//case WM_KILLFOCUS :		//hMain���� focus�� �Ѱ� �ָ�, InvalidateRect�� �Ѵ�.
	//	
	//	InvalidateRect(hMain, NULL, true);
	//	
	//	return 0;
	}

	return CallWindowProc(originalProc, hWnd, iMessage, wParam, lParam);
}