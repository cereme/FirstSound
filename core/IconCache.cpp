#pragma once
#include "IconCache.h"
IconCache::IconCache(std::wstring path) {
	signatures_1x.push_back(L"0200"); //Microsoft Windows Store 관련 + 디바이스
	signatures_1x.push_back(L"2200"); //임시파일 여러개 모아놓은거같음
	signatures_1x.push_back(L"4200");
	signatures_2x.push_back(L"0100");
	signatures_2x.push_back(L"1100");
	signatures_2x.push_back(L"1000");
	signatures_2x.push_back(L"4100");
	signatures_2x.push_back(L"8100");
	signatures_2x.push_back(L"9100");
	signatures_2x.push_back(L"A100");
	signatures_2x.push_back(L"C100");
	signatures_2x.push_back(L"FFFF");

	std::wifstream file;
	file.open(path, std::ios::binary);
	wchar_t* buffer = nullptr;

	file.seekg(0, std::ios::end);
	int fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	try {
		buffer = new wchar_t[fileSize];
	}
	catch (int errCode) {
		std::cout << "Memory Alloc Error:" << errCode << std::endl;

	}
	file.read(buffer, fileSize);

	content = std::vector<wchar_t>(buffer, buffer + fileSize);
	delete[] buffer;
}

bool IconCache::CheckSignature1x(std::wstring sig) {
	for (auto elem : signatures_1x) {
		if (sig == elem) {
			return true;
		}
	}
	return false;
}

bool IconCache::CheckSignature2x(std::wstring sig) {
	for (auto elem : signatures_2x) {
		if (sig == elem) {
			return true;
		}
	}
	return false;
}

std::wstring IconCache::slice(int start, int end, bool prettify = false, bool isSingleByte = false) {
	std::wstring return_val;

	std::string result;
	for (int i = start; i < end; i++) {
		std::stringstream tmp;
		tmp << std::uppercase << std::hex << content[i];
		if (tmp.str() == "0") result += "0";
		else if (stoi(tmp.str(), NULL, 16) < 0x10) result += "0";
		result += tmp.str();
	}
	return_val = std::wstring(result.begin(), result.end());

	if (prettify) {
		std::wstring wresult(result.begin(), result.end());
		std::wstring result_pretty;
		std::vector<std::wstring> bytes;
		for (int i = 0; i < wresult.length(); i += 2) {
			std::wstring tmp = wresult.substr(i, 2);
			bytes.push_back(tmp);
		}
		/*
		for (int i = 0; i < bytes.size(); i+=2) {
		if (bytes[i] != L"00" && bytes[i + 1] != L"00" && !isSingleByte) {
		int unicode = std::stoi(bytes[i + 1], NULL, 16) * 256 + std::stoi(bytes[i], NULL, 16);
		result_pretty += wchar_t(unicode);
		}
		else {
		result_pretty += std::stoi(bytes[i], NULL, 16);
		if (bytes[i + 1] != L"00")
		result_pretty += std::stoi(bytes[i + 1], NULL, 16);
		}
		}*/

		for (int i = 0; i < bytes.size() / 2; i++) {

			int unicode = std::stoi(bytes[2 * i + 1], NULL, 16) * 256 + std::stoi(bytes[2 * i], NULL, 16);

			if ((unicode >= 0xAC00) && (unicode <= 0xD7AF)) {
				result_pretty += wchar_t(unicode);
			}
			else {
				result_pretty += std::stoi(bytes[2 * i], NULL, 16);
				if (bytes[2 * i + 1] != L"00")
					result_pretty += std::stoi(bytes[2 * i + 1], NULL, 16);
			}
		}
		return_val = result_pretty;
	}

	return return_val;
}

void IconCache::AnalyzeCLI(void)
{
	int startOffset = 0x48;

	int expectedCount = std::stoi(slice(startOffset + 1, startOffset + 2) + slice(startOffset + 0, startOffset + 1), NULL, 16);

	startOffset += 4;

	int sectionCount = 0;
	int length;

	while (true) {
		std::cout << "Offset\t\t" << std::hex << startOffset;
		std::cout << "\t\tCount : " << std::dec << sectionCount << std::endl;

		std::wstring itemHeader = slice(startOffset, startOffset + 4);
		std::wstring signature = itemHeader.substr(0, 4);

		std::wcout << "Signature \t" << signature << std::endl;

		startOffset += 4;

		int iconLength;

		if (CheckSignature2x(signature)) {
			length = std::stoi(itemHeader.substr(4, 2), NULL, 16) * 2;
			iconLength = 4;
		}
		else if (CheckSignature1x(signature)) {
			length = std::stoi(itemHeader.substr(4, 2), NULL, 16);
			int length_second = std::stoi(itemHeader.substr(6, 2), NULL, 16);
			if (length_second != 0) {
				auto tmp = itemHeader.substr(6, 2) + itemHeader.substr(4, 2);
				length = std::stoi(tmp, NULL, 16);
			}
			iconLength = 4;
			if (signature == L"2200") {
				iconLength = 4;
			}
		}
		else {
			std::wcout << "No Pathlen :" << itemHeader << std::endl;
			if (sectionCount >= expectedCount) {
				startOffset -= 4;
				break;
			}

			while (slice(startOffset, startOffset + 4).substr(0, 4) != L"0100") {
				startOffset += 1;
			}
			continue;
		}


		wprintf(L"Path\t\t%s\n", slice(startOffset, startOffset + length, true).c_str());
		wprintf(L"IconPath\t%s\n", slice(startOffset + length, startOffset + length + iconLength).c_str());

		//std::wcout << "Path\t\t" << cache.slice(startOffset, startOffset + length, true, isSingleByte) << std::endl;

		//std::wcout << "IconPath\t" << cache.slice(startOffset + length, startOffset + length + 4, false) << std::endl;

		sectionCount++;

		startOffset += (length + iconLength);

		std::cout << "-------------------------------------------------------" << std::endl;

		if (sectionCount > expectedCount) {
			break;
		}
	}

	std::cout << "First Section Count : " << sectionCount << std::endl;

	////////////////////////

	expectedCount = std::stoi(slice(startOffset + 1, startOffset + 2) + slice(startOffset + 0, startOffset + 1), NULL, 16);

	startOffset += 4;

	sectionCount = 0;

	while (true) {
		std::cout << "Offset\t\t" << std::hex << startOffset;
		std::cout << "\t\tCount : " << std::dec << sectionCount << std::endl;

		std::wstring itemHeader = slice(startOffset, startOffset + 2);
		int length = std::stoi(itemHeader.substr(0, 2), NULL, 16) * 2;

		int iconLength = 12;

		std::wcout << "Length\t" << length << std::endl;

		startOffset += 2;



		wprintf(L"Path\t\t%s\n", slice(startOffset, startOffset + length, true).c_str());
		wprintf(L"IconPath\t%s\n", slice(startOffset + length, startOffset + length + iconLength).c_str());

		//std::wcout << "Path\t\t" << cache.slice(startOffset, startOffset + length, true, isSingleByte) << std::endl;

		//std::wcout << "IconPath\t" << cache.slice(startOffset + length, startOffset + length + 4, false) << std::endl;

		sectionCount++;

		startOffset += (length + iconLength);

		std::cout << "-------------------------------------------------------" << std::endl;

		if (sectionCount >= expectedCount) {
			break;
		}
	}

	std::cout << "Second Section Count : " << sectionCount << std::endl;

	////////////////////////

	expectedCount = std::stoi(slice(startOffset + 1, startOffset + 2) + slice(startOffset + 0, startOffset + 1), NULL, 16);

	startOffset += 4;

	sectionCount = 0;

	while (true) {
		std::cout << "Offset\t\t" << std::hex << startOffset;
		std::cout << "\t\tCount : " << std::dec << sectionCount << std::endl;

		std::wstring itemHeader = slice(startOffset, startOffset + 2);
		int length = std::stoi(itemHeader.substr(0, 2), NULL, 16) * 2;

		int iconLength = 12;

		std::wcout << "Length\t" << length << std::endl;

		startOffset += 2;



		wprintf(L"Path\t\t%s\n", slice(startOffset, startOffset + length, true).c_str());
		wprintf(L"IconPath\t%s\n", slice(startOffset + length, startOffset + length + iconLength).c_str());

		//std::wcout << "Path\t\t" << cache.slice(startOffset, startOffset + length, true, isSingleByte) << std::endl;

		//std::wcout << "IconPath\t" << cache.slice(startOffset + length, startOffset + length + 4, false) << std::endl;

		sectionCount++;

		startOffset += (length + iconLength);

		std::cout << "-------------------------------------------------------" << std::endl;

		if (sectionCount >= expectedCount) {
			break;
		}
	}

}

void IconCache::Analyze(void)
{
	int startOffset = 0x48;

	int expectedCount = std::stoi(slice(startOffset + 1, startOffset + 2) + slice(startOffset + 0, startOffset + 1), NULL, 16);

	startOffset += 4;

	int sectionCount = 0;
	int length;

	while (true) {
		std::wstring itemHeader = slice(startOffset, startOffset + 4);
		std::wstring signature = itemHeader.substr(0, 4);
		startOffset += 4;
		int iconLength;
		if (CheckSignature2x(signature)) {
			length = std::stoi(itemHeader.substr(4, 2), NULL, 16) * 2;
			iconLength = 4;
		}
		else if (CheckSignature1x(signature)) {
			length = std::stoi(itemHeader.substr(4, 2), NULL, 16);
			int length_second = std::stoi(itemHeader.substr(6, 2), NULL, 16);
			if (length_second != 0) {
				auto tmp = itemHeader.substr(6, 2) + itemHeader.substr(4, 2);
				length = std::stoi(tmp, NULL, 16);
			}
			iconLength = 4;
			if (signature == L"2200") {
				iconLength = 4;
			}
		}
		else {
			if (sectionCount >= expectedCount) {
				startOffset -= 4;
				break;
			}
			while (slice(startOffset, startOffset + 4).substr(0, 4) != L"0100") {
				startOffset += 1;
			}
			continue;
		}

		std::wstring path = slice(startOffset, startOffset + length, true);
		std::wstring iconpath = slice(startOffset + length, startOffset + length + iconLength);

		std::vector<wchar_t> v_path(path.begin(), path.end());
		std::vector<wchar_t> v_iconpath(iconpath.begin(), iconpath.end());

		icons.push_back(IconElem(startOffset, signature,
			length, v_path, v_iconpath));

		sectionCount++;

		startOffset += (length + iconLength);

		if (sectionCount > expectedCount) {
			break;
		}
	}

	//std::cout << "First Section Count : " << sectionCount << std::endl;

	////////////////////////

	expectedCount = std::stoi(slice(startOffset + 1, startOffset + 2) + slice(startOffset + 0, startOffset + 1), NULL, 16);

	startOffset += 4;

	sectionCount = 0;

	while (true) {
		std::wstring itemHeader = slice(startOffset, startOffset + 2);
		int length = std::stoi(itemHeader.substr(0, 2), NULL, 16) * 2;

		int iconLength = 12;

		startOffset += 2;

		std::wstring path = slice(startOffset, startOffset + length, true);
		std::wstring iconpath = slice(startOffset + length, startOffset + length + iconLength);

		std::vector<wchar_t> v_path(path.begin(), path.end());
		std::vector<wchar_t> v_iconpath(iconpath.begin(), iconpath.end());

		icons.push_back(IconElem(startOffset, L"NULL",
			length, v_path, v_iconpath));

		sectionCount++;

		startOffset += (length + iconLength);

		if (sectionCount >= expectedCount) {
			break;
		}
	}

	//std::cout << "Second Section Count : " << sectionCount << std::endl;

	////////////////////////

	expectedCount = std::stoi(slice(startOffset + 1, startOffset + 2) + slice(startOffset + 0, startOffset + 1), NULL, 16);

	startOffset += 4;

	sectionCount = 0;

	while (true) {
		std::wstring itemHeader = slice(startOffset, startOffset + 2);
		int length = std::stoi(itemHeader.substr(0, 2), NULL, 16) * 2;

		int iconLength = 12;
		startOffset += 2;



		std::wstring path = slice(startOffset, startOffset + length, true);
		std::wstring iconpath = slice(startOffset + length, startOffset + length + iconLength);

		std::vector<wchar_t> v_path(path.begin(), path.end());
		std::vector<wchar_t> v_iconpath(iconpath.begin(), iconpath.end());

		icons.push_back(IconElem(startOffset, L"NULL",
			length, v_path, v_iconpath));
		sectionCount++;

		startOffset += (length + iconLength);

		if (sectionCount >= expectedCount) {
			break;
		}
	}

}