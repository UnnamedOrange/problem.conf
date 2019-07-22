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

// TBlock

#pragma once
#include "TStdInclude.hpp"

class TBlock
{
	BYTE* pData;
	DWORD dwSize;

	VOID Construct(DWORD size)
	{
		if (pData) Destruct();
		if (!size) return;
		dwSize = size;
		pData = new BYTE[dwSize];
		if (!pData)
		{
			dwSize = 0;
			throw std::runtime_error("Fail to new (pData).");
		}
	}
	VOID Destruct()
	{
		if (!pData) return;
		delete[] pData;
		pData = nullptr;
		dwSize = 0;
	}

public:
	TBlock() noexcept : pData(), dwSize() {}
	~TBlock() noexcept { Destruct(); }
	TBlock(const TBlock& b) noexcept : TBlock() { (*this) = b; }
	TBlock(TBlock&& b) noexcept
	{
		pData = b.pData;
		dwSize = b.dwSize;
		b.pData = nullptr;
		b.dwSize = 0;
	}

	explicit TBlock(DWORD dwSize) : TBlock()
	{
		Construct(dwSize);
	}

	TBlock& operator=(const TBlock& b)
	{
		Destruct();
		if (b.pData)
		{
			Construct(b.dwSize);
			std::memcpy(pData, b.pData, b.dwSize);
		}
		return *this;
	}
	VOID clear() { Destruct(); }
	VOID realloc(DWORD size) { Construct(size); }
	BOOL empty() const { return !pData; }
	DWORD size() const { return dwSize; }
	const VOID* data() const { return pData; }
	VOID* data() { return pData; }
	VOID assign(DWORD deltaBegin, LPCVOID source, DWORD size)
	{
		std::memcpy(pData + deltaBegin, source, size);
	}

private:
	BOOL _WriteToFileAndCloseHandle(HANDLE hFile)
	{
		DWORD dwWritten;
		BOOL bRet = WriteFile(hFile, pData, dwSize, &dwWritten, NULL);
		CloseHandle(hFile);
		return bRet;
	}

public:
	BOOL WriteToFile(const std::wstring& strPath)
	{
		HANDLE hFile = CreateFileW(strPath.c_str(), GENERIC_WRITE,
			FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		return _WriteToFileAndCloseHandle(hFile);
	}
	BOOL WriteToFile(const std::string& strPath)
	{
		HANDLE hFile = CreateFileA(strPath.c_str(), GENERIC_WRITE,
			FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		return _WriteToFileAndCloseHandle(hFile);
	}
};