#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // For HRESULT
#include <vcruntime_exception.h>
#include <vector>

// Source: https://github.com/Microsoft/DirectX-Graphics-Samples
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

inline std::wstring ToWString(const std::string& inString, UINT inCodePage = CP_UTF8)
{
	if (inString.length() == 0)
	{
		return std::wstring();
	}

	uint32_t stringLength =  static_cast<uint32_t>( inString.length() + 1 );
	uint32_t wideStringLength = MultiByteToWideChar(inCodePage, 0, inString.c_str(), stringLength, 0, 0);
	std::vector<wchar_t> wideChars;
	wideChars.resize(wideStringLength);
	MultiByteToWideChar(inCodePage, 0, inString.c_str(), stringLength, wideChars.data(), wideStringLength);
	return std::wstring(wideChars.data());
}

inline std::string ToString(const std::wstring& inWideString, UINT inCodePage = CP_UTF8)
{
	if (inWideString.length() == 0)
	{
		return std::string();
	}

	uint32_t wideStringLength = static_cast<uint32_t>( inWideString.length() + 1 );
	uint32_t stringLength = WideCharToMultiByte(inCodePage, 0, inWideString.c_str(), wideStringLength, 0, 0, "", NULL);
	std::vector<char> chars;
	chars.resize(stringLength);
	WideCharToMultiByte(inCodePage, 0, inWideString.c_str(), wideStringLength, chars.data(), stringLength, "", NULL);
	return std::string(chars.data());
}
