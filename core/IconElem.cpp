#include "IconElem.h"

IconElem::IconElem(int offset, std::wstring signature, int length,
	std::vector<wchar_t> path, std::vector<wchar_t> iconpath)
{
	this->offset = offset;
	this->signature = signature;
	this->length = length;
	this->path = path;
	this->iconpath = iconpath;
}


IconElem::~IconElem()
{
}
