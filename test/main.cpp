#include "./../core/IconCache.h"
#include "./../core/IconElem.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <locale.h>

std::wstring GetIconCachePath() {
	TCHAR buf[256];
	DWORD size = 256;
	std::wstring path = L"C:\\Users\\";
	GetUserName(buf, &size);
	if (size > 256) {
		throw "Too Long Username";
	}
	path += buf;
	path += L"\\AppData\\Local\\IconCache.db";
	return path;
}
int main()
{
	_wsetlocale(LC_ALL, L"korean");

	IconCache cache(L"IconCache.db");
	//IconCache cache(GetIconCachePath());

	cache.AnalyzeCLI();

	/*
	cache.Analyze();

	for (auto elem : cache.icons) {
	std::wcout << elem.signature << std::endl;
	}
	*/

	return 0;
}

