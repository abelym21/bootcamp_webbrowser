#include "web_browser.h"

WindowHandlerPool::WindowHandlerPool()
{
	windowHandlerNum = 0;
}

WindowHandlerPool::~WindowHandlerPool()
{
}


int WindowHandlerPool::getHwndNum()
{
	return this->windowHandlerNum;
}

std::vector<HWND> WindowHandlerPool::getHwndPool()
{
	return this->winHandlerPool;
}

int WindowHandlerPool::addHwnd(HWND hWnd)
{
	winHandlerPool.push_back(hWnd);
	return 0;
}

int WindowHandlerPool::deleteHWNDs()
{
	for (int j = 0; j < winHandlerPool.size(); j++)
	{
		DestroyWindow(winHandlerPool[j]);
	}
	return 0;
}