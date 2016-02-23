#include "web_browser.h"


HTMLnode::HTMLnode(std::string nodeName)
{
	this->nodeName = nodeName;
}

HTMLnode::~HTMLnode()
{
}


int HTMLnode::setText(std::string text)
{
	this->text = text;

	return 0;
}

int HTMLnode::setAttributes(std::string attributeName, std::string attributeValue)
{
	this->attributes[attributeName] = attributeValue;

	return 0;
}

int HTMLnode::addChild(HTMLnode *child)
{
	children.push_back(child);

	return 0;
}

int HTMLnode::setParent(HTMLnode *parent)
{
	this->parent = parent;
	return 0;
}

std::string HTMLnode::getText()
{
	return this->text;
}

std::map<std::string, std::string> HTMLnode::getAttributes()
{
	return this->attributes;
}

std::list<HTMLnode*> HTMLnode::getChilderen()
{
	return this->children;
}

HTMLnode* HTMLnode::getParent()
{
	return this->parent;
}

std::string HTMLnode::getNodeName()
{
	return this->nodeName;
}