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

// TGdiplus

#pragma once
#include "TStdInclude.hpp"

class TGdiplus final
{
	static ULONG_PTR& GetGdiplusToken()
	{
		static ULONG_PTR GdiplusToken = 0;
		return GdiplusToken;
	}
	static int& GetRefCount()
	{
		static int RefCount = 0;
		return RefCount;
	}
	static VOID Load()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&GetGdiplusToken(), &gdiplusStartupInput, nullptr);
	}
	static VOID Unload()
	{
		auto& token = GetGdiplusToken();
		if (token)
		{
			Gdiplus::GdiplusShutdown(token);
			token = 0;
		}
	}

public:
	TGdiplus()
	{
		if (!GetRefCount())
			Load();
		GetRefCount()++;
	}
	~TGdiplus()
	{
		GetRefCount()--;
		if (!GetRefCount())
			Unload();
	}
};