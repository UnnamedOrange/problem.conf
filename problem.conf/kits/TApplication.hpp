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

// TApplication

#pragma once

#include "TStdInclude.hpp"

#include "TFileInfo.hpp"
#include "TBlock.hpp"

class TApplication
{
	HINSTANCE hInstance;
	TFileInfo FileInfo;
	std::vector<std::wstring> CommandLine;
	std::wstring ApplicationName;
	std::wstring GUID;

private:
	VOID BuiltinVerifySingleTApplication()
	{
		static BOOL bUsedApplicationClass;
		if (bUsedApplicationClass)
			throw std::runtime_error("Only one instance can you hold.");
		bUsedApplicationClass = TRUE;
	}
	VOID BuiltinGetCommandLine()
	{
		LPWSTR lpCmdLine = GetCommandLineW();
		int nCmd;
		LPWSTR* lpCmdLineArray = CommandLineToArgvW(lpCmdLine, &nCmd);
		for (int i = 0; i < nCmd; i++)
			CommandLine.push_back(lpCmdLineArray[i]);
		LocalFree((HLOCAL)lpCmdLineArray);
	}

public:
	TApplication()
	{
		hInstance = HINST;
		BuiltinVerifySingleTApplication();
		BuiltinGetCommandLine();
	}
	TApplication(LPCWSTR lpcAppName, LPCWSTR lpcGUID) : TApplication()
	{
		ApplicationName = lpcAppName;
		GUID = lpcGUID;
	}

public:
	const HINSTANCE GetInstance() { return hInstance; }
	const TFileInfo& GetFileInfo() const { return FileInfo; }
	const std::vector<std::wstring>& GetCmdLine() const { return CommandLine; }
	const std::wstring& GetApplicationName() const { return ApplicationName; }
	const std::wstring& GetGUID() const { return GUID; }

public:
	TBlock LoadResource(LPCWSTR lpcResourceName, LPCWSTR lpcTypeName)
	{
		TBlock block;
		HRSRC hResInfo = FindResourceW(hInstance, lpcResourceName, lpcTypeName);
		if (!hResInfo) return throw std::exception(), block;
		HGLOBAL hResource = ::LoadResource(hInstance, hResInfo);
		if (!hResource) return throw std::exception(), block;
		DWORD dwSize = SizeofResource(hInstance, hResInfo);
		if (!dwSize) return throw std::exception(), block;

		LPVOID p = LockResource(hResource);
		block.realloc(dwSize);
		block.assign(0, p, dwSize);
		FreeResource(hResource);

		return block;
	}

public:
	virtual INT Execute() = 0;

public:
	// 应该仅在返回 TRUE 时继续
	BOOL SingleInstance()
	{
		if (GUID.empty())
			return throw std::runtime_error("A GUID is needed."), FALSE;
		HANDLE hMutex = CreateMutexW(nullptr, FALSE, GUID.c_str());
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			return FALSE;
		return TRUE;
	}
};