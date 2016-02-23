
#include "web_browser.h"
#include <stack>

HTMLparser::HTMLparser()
{
	resultToken.clear();
	this->HTMLstate.insert(std::pair<std::string, boolean>("StartState", false));
	this->HTMLstate.insert(std::pair<std::string, boolean>("DataState", false));
	this->HTMLstate.insert(std::pair<std::string, boolean>("OpenTagState", false));
	this->HTMLstate.insert(std::pair<std::string, boolean>("CloseTagState", false));
	this->HTMLstate.insert(std::pair<std::string, boolean>("TagNameState", false));
	this->HTMLstate.insert(std::pair<std::string, boolean>("FinalState", false));

}

HTMLparser::~HTMLparser()
{
}

boolean HTMLparser::getIsParsed()
{
	return this->isParsed;
}

int HTMLparser::setIsParsed(boolean value)
{
	this->isParsed = value;
	return 0;
}


HTMLnode* HTMLparser::getDocumentNode()
{
	return this->document;
}

int HTMLparser::parsingHTML(std::string response)
{
	createParseTree();
	tokenizing(response);

	return 0;
}

int HTMLparser::checkTextToken(std::string token)
{
	if (token == "")
	{
		return 0;
	}

	HTMLnode *newNode = new HTMLnode("text");
	newNode->setText(token);
	setupParseTree(newNode);

	return 0;
}

int HTMLparser::checkTagToken(std::string token)
{
	//parse tag name
	int tagPoint = token.find(' ', 0);
	boolean isAttribute = false;
	HTMLnode *newNode;
	std::string tag;

	if (tagPoint != std::string::npos)
	{
		tag = token.substr(0, tagPoint);
		isAttribute = true;
	}
	else
	{//attribute 없이 tag만 들어온 경우
		tag = token;
	}

	if (tag == "br/")
	{
		tag = "br";
	}

	newNode = new HTMLnode(tag);

	//자식 처리
	//attribute 처리
	if (isAttribute)
	{
		setupParseTree(settingAttribute(newNode, token, tagPoint));
		return 0;
	}
	else
	{
		setupParseTree(newNode);
		return 0;
	}

}

HTMLnode* HTMLparser::settingAttribute(HTMLnode *newNode, std::string token, int tagPoint)
{
	char c;
	int equalPoint = tagPoint;
	std::string attributeName;
	std::string attributeValue;
	boolean attributeNameState = true;
	boolean attributeValueState = false;

	for (int i = tagPoint; i < (int)token.size(); i++)
	{
		c = token.at(i);

		if (attributeValueState && (c == ' ' || i == token.size() - 1))
		{
			newNode->setAttributes(attributeName, attributeValue);
			attributeName.clear();
			attributeValue.clear();
			attributeNameState = !attributeNameState;
			attributeValueState = !attributeValueState;
			continue;
		}
		else if (c == ' ' || c == '\"' || c == '\'')
		{
			continue;
		}
		else if (c == '=')
		{
			attributeNameState = !attributeNameState;
			attributeValueState = !attributeValueState;
			continue;
		}

		if (attributeNameState)
		{
			attributeName.push_back(c);
			continue;
		}
		else if (attributeValueState)
		{
			attributeValue.push_back(c);
			continue;
		}
	}

	return newNode;
}

int HTMLparser::createParseTree()
{
	document = new HTMLnode("document");	//root 노드인 document 노드 생성.

	matchStack.push(document);

	return 0;
}

int HTMLparser::setupParseTree(HTMLnode *newNode)
{
	std::string nodeName = newNode->getNodeName();

	if (nodeName == "br" || nodeName == "input" || nodeName == "img" || nodeName == "text" || nodeName == "meta" || nodeName == "link" || nodeName == "hr" || nodeName == "area")	//닫는 테그가 필요 없는 테그 혹은 text
	{
		isCloseTag = false;
		newNode->setParent(matchStack.top());	//부모설정
		matchStack.top()->addChild(newNode);	//자식설정
	}
	else if ( nodeName == matchStack.top()->getNodeName() && isCloseTag)	//닫는 테그일 경우.
	{
		isCloseTag = false;
		matchStack.pop();
		return 0;
	}
	else
	{
		newNode->setParent(matchStack.top());	//부모설정
		matchStack.top()->addChild(newNode);	//자식설정
		matchStack.push(newNode);				//stack에 넣기
	}

	return 0;
}

std::string HTMLparser::removeResponseHeader(std::string response)
{
	int ptr = 0;
	std::string result;
	ptr = response.find("\r\n\r\n");
	result = response.substr(ptr + 4, response.size() - ptr - 4);

	return result;
}


int HTMLparser::tokenizing(std::string responseAll)
{

	char c = '\0';
	std::string response;
	boolean scriptState = false;
	std::string tmp;
	int point = 0;
	
	HTMLstate["StartState"] = true;
	HTMLstate["DataState"] = true;

	response = removeResponseHeader(responseAll);	//resopnse 메시지의 header 제거

	if ((point = response.find("<html")) != std::string::npos)	//response 메시지의 html 테그 앞의 내용 제거.
	{
		//test
	//	point = response.find(">", point);
		//test
		response = response.substr(point , response.size() - point);
	}
	
	for (int i = 0; i < response.size(); i++){
	
		if (HTMLstate["DataState"])
		{
			c = response.at(i);

			if (c == '<')
			{
				checkTextToken(getResultToken());
				HTMLstate["DataState"] = false;
				HTMLstate["OpenTagState"] = true;
				continue;
			}
			else if (c == '\r' || c == '\n' || c == '\t')
			{
				continue;
			}
			else
			{
				token.push(c);
				continue;
			}
		}
		else if (HTMLstate["OpenTagState"])
		{
			c = response.at(i);

			if (c == '/')
			{
				isCloseTag = true;
				HTMLstate["CloseTagState"] = true;
				HTMLstate["OpenTagState"] = false;
				continue;
			}
			else
			{
				HTMLstate["TagNameState"] = true;
				HTMLstate["OpenTagState"] = false;
				continue;
			}

		}
		else if (HTMLstate["CloseTagState"])
		{
			c = response.at(i);

			HTMLstate["CloseTagState"] = false;
			HTMLstate["TagNameState"] = true;
			continue;
		}
		else if (HTMLstate["TagNameState"])
		{
			token.push(c);
			c = response.at(i);

			if (c == '>')
			{
				tmp = getResultToken();
				//script 테그 안의 내용을 무시하기 위한 예외 처리.
				if (tmp.find("script", 0) == 0)
				{
					i = response.find("</script>", i);
					i = i + strlen("</script>") - 1;
					HTMLstate["TagNameState"] = false;
					HTMLstate["DataState"] = true;
				}
				else if (tmp.find("style", 0) == 0)
				{
					i = response.find("</style>", i);
					i = i + strlen("</style>") - 1;
					HTMLstate["TagNameState"] = false;
					HTMLstate["DataState"] = true;
				}
				//주석 예외 처리.
				else if (tmp.find("!--",0 ) == 0)
				{
					//주석 안에 태그가 있을 경우 예외 처리.
					if (tmp.back() != '-')
					{
						i = response.find("-->", i);
						i = i + strlen("-->") - 1;
					}

					HTMLstate["TagNameState"] = false;
					HTMLstate["DataState"] = true;
				}
				else
				{
					checkTagToken(tmp);
					HTMLstate["TagNameState"] = false;
					HTMLstate["DataState"] = true;
					continue;
				}

			}
			//닫힘 테그가 별도로 없는 테그를 위한 예외 처리.
			else if (c == '/' && response.at(i+1) == '>')
			{
				tmp = getResultToken();
				i++;
				isCloseTag = true;
				checkTagToken(tmp);
				HTMLstate["TagNameState"] = false;
				HTMLstate["DataState"] = true;

				continue;
			}
			else
			{
				continue;
			}
		}
		else
		{
			printf("Error : tokenizing function error. - State : Exception\n");
			break;
		}
	}
	printf("\n[notice] : End of tokenizing function.\n");

	return 0;
}


std::string HTMLparser::getResultToken()
{
	resultToken.clear();

	while (!token.empty())
	{
		resultToken.append(1, token.front());
		token.pop();
	}

	return resultToken;
}


void HTMLparser::testprint()
{
}