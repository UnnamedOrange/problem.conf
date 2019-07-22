// Copyright (C) 2018 Orange Software
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// TFileInfo

// Build 1 in 2018-6-4

#pragma once

#include "TStdInclude.hpp"

class TFileInfo : public VS_FIXEDFILEINFO
{
	TFileInfo& operator=(const VS_FIXEDFILEINFO& b)
	{
#define assign(x) x = b.x
		assign(dwSignature);
		assign(dwStrucVersion);
		assign(dwFileVersionMS);
		assign(dwFileVersionLS);
		assign(dwProductVersionMS);
		assign(dwProductVersionLS);
		assign(dwFileFlagsMask);
		assign(dwFileFlags);
		assign(dwFileOS);
		assign(dwFileType);
		assign(dwFileSubtype);
		assign(dwFileDateMS);
		assign(dwFileDateLS);
#undef assign
		return *this;
	}
	BOOL GetFileInfo(LPCWSTR lpcFileName)
	{
		VS_FIXEDFILEINFO* pffi;
		// in fact dwHandle is ignored
		DWORD dwHandle = 0;
		// return 0 if failed
		DWORD dwSize = GetFileVersionInfoSizeW(lpcFileName, &dwHandle);
		if (!dwSize)
			return FALSE;
		BYTE* pBlock = new BYTE[dwSize];
		GetFileVersionInfoW(lpcFileName, 0, dwSize, pBlock);
		// dwSize is no longer used
		VerQueryValueW(pBlock, L"\\", (LPVOID*)& pffi, (PUINT)& dwSize);
		*this = *pffi;
		delete[] pBlock;
		return TRUE;
	}
	VOID GetFileInfo() // 获取自身文件信息
	{
		std::vector<WCHAR> strFileName(65536);
		GetModuleFileNameW(HINST, strFileName.data(), 65536);
		try
		{
			if (!GetFileInfo(strFileName.data()))
				throw std::logic_error("Fail to get own file info.");
		}
		catch (const std::logic_error&)
		{
#ifdef _DEBUG
			MessageBoxW(NULL, L"TKernel doesn't find the version info of your "
				"application. It is recommended that you attach the version "
				"info in the resource file. This message won't show up if the "
				"(_DEBUG) isn't defined.", L"Warning", MB_ICONWARNING);
#endif
		}
	}
	VOID GetFileInfo(HINSTANCE hInstance) // 根据提供的 hInstance 获取版本信息，适用于 DLL
	{
		std::vector<WCHAR> strFileName(65536);
		GetModuleFileNameW(hInstance, strFileName.data(), 65536);
		try
		{
			if (!GetFileInfo(strFileName.data()))
				throw std::logic_error("Fail to get own file info");
		}
		catch (const std::logic_error&)
		{
#ifdef _DEBUG
			MessageBoxW(NULL, L"TKernel doesn't find the version info of your "
				"application. It is recommended that you attach the version "
				"info in the resource file. This message won't show up if the "
				"(_DEBUG) isn't defined.", L"Warning", MB_ICONWARNING);
#endif
		}
	}

public:
	TFileInfo() { GetFileInfo(); }
	explicit TFileInfo(LPCWSTR lpcFileName) { GetFileInfo(lpcFileName); }
	explicit TFileInfo(HINSTANCE hInstance) { GetFileInfo(hInstance); }
	TFileInfo(const TFileInfo&) = default;
	TFileInfo(TFileInfo&&) = delete;

public:
	DWORD GetVer1() const { return (DWORD)HIWORD(dwProductVersionMS); }
	DWORD GetVer2() const { return (DWORD)LOWORD(dwProductVersionMS); }
	DWORD GetVer3() const { return (DWORD)HIWORD(dwProductVersionLS); }
	DWORD GetVer4() const { return (DWORD)LOWORD(dwProductVersionLS); }
};