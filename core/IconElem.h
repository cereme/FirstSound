#pragma once
#include <iostream>
#include <vector>
#include <string>

class IconElem
{
public:
	int offset;
	std::wstring signature;
	int length;
	std::vector<wchar_t> path;
	std::vector<wchar_t> iconpath;
public:
	IconElem(int offset, std::wstring signature, int length,
		std::vector<wchar_t> path, std::vector<wchar_t> iconpath);
	~IconElem();
};

