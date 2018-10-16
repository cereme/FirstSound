#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <locale.h>
#include "IconElem.h"

class IconCache {
public:
	std::vector<wchar_t> content;
	std::vector<std::wstring> signatures_1x;
	std::vector<std::wstring> signatures_2x;
	std::vector<IconElem> icons;
public:
	IconCache(std::wstring path);

	bool CheckSignature1x(std::wstring sig);
	bool CheckSignature2x(std::wstring sig);

	std::wstring slice(int start, int end, bool prettify, bool isSingleByte);

	void AnalyzeCLI(void);
	void Analyze(void);

};