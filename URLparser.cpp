#include "web_browser.h"

URLparser::URLparser()
{
	this->originalUrl.clear();
	this->uri.clear();
	this->serverName.clear();
}

int URLparser::parseUrl(char *url)
{
	boolean tmp;
	URL_COMPONENTS urlComp;
	std::string tmpUrl;
	tmpUrl.clear();

	ZeroMemory(&urlComp, sizeof(urlComp));
	urlComp.dwStructSize = sizeof(urlComp);

	urlComp.dwSchemeLength = INTERNET_MAX_SCHEME_LENGTH;
	urlComp.lpszScheme = new TCHAR[INTERNET_MAX_SCHEME_LENGTH + 1];
	urlComp.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
	urlComp.lpszHostName = new TCHAR[INTERNET_MAX_HOST_NAME_LENGTH + 1];
	urlComp.dwUrlPathLength = INTERNET_MAX_PATH_LENGTH;
	urlComp.lpszUrlPath = new TCHAR[INTERNET_MAX_PATH_LENGTH+1];
	urlComp.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
	urlComp.lpszUserName = new TCHAR[INTERNET_MAX_USER_NAME_LENGTH+1];

	
	if (!(url[0] == 'h' && url[1] == 't' && url[2] == 't' && url[3] == 'p'))
	{
		tmpUrl.append("http://");

		/*if (!(url[0] == 'w' && url[1] == 'w' && url[2] == 'w' && url[3] == '.'))
		{
			tmpUrl.append("wwww.");
		}*/

		tmpUrl.append(url);
		this->originalUrl = tmpUrl;

		tmp = InternetCrackUrl(tmpUrl.c_str(), tmpUrl.length(), 0, &urlComp);
	}
	else
	{
		tmp = InternetCrackUrl(url, strlen(url), 0, &urlComp);
	}

	if (!tmp)
	{
		printf("Error : parseServername function error.\n");
		return 1;
	}

	if (urlComp.dwUrlPathLength == 0)
	{
		urlComp.lpszUrlPath = "/";
		urlComp.dwUrlPathLength = 1;
	}

	this->serverName = urlComp.lpszHostName;
	this->port = std::to_string(urlComp.nPort);
	this->uri = urlComp.lpszUrlPath;
	return 0;
}


std::string URLparser::getServerName()
{
	return this->serverName;
}

std::string URLparser::getUri()
{
	return this->uri;
}

std::string URLparser::getPort()
{
	return this->port;
}

std::string URLparser::getUrl()
{
	return this->originalUrl;
}