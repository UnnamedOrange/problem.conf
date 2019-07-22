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

// TPrivateFont

// Private font loader

#pragma once
#include "TStdInclude.hpp"

#include "TBlock.hpp"

class TPrivateFont
{
protected:
	TBlock data;

private:
	HANDLE fHandle = NULL;

	VOID _RegisterFont()
	{
		if (data.empty()) return;
		DWORD pNumFonts = 0;
		fHandle = AddFontMemResourceEx(data.data(), data.size(), NULL, &pNumFonts);
	}
	VOID _UnregisterFont()
	{
		if (fHandle)
		{
			RemoveFontMemResourceEx(fHandle);
			fHandle = NULL;
		}
	}

public:
	TPrivateFont() = default;
	TPrivateFont(const TBlock& block) : data(block)
	{
		_RegisterFont();
	}
	TPrivateFont(TBlock&& block) : data(std::move(block))
	{
		_RegisterFont();
	}
	~TPrivateFont()
	{
		_UnregisterFont();
	}
};

// for Gdiplus
class TPrivateFontPlus final : public TPrivateFont
{
	Gdiplus::PrivateFontCollection pfc;
	Gdiplus::FontFamily family;

	VOID _AfterRegisterFont()
	{
		pfc.AddMemoryFont(data.data(), data.size());
		INT numFound;
		pfc.GetFamilies(1, &family, &numFound);
	}

public:
	TPrivateFontPlus() = default;
	TPrivateFontPlus(const TBlock& block) : TPrivateFont(block)
	{
		_AfterRegisterFont();
	}
	TPrivateFontPlus(TBlock&& block) : TPrivateFont(std::move(block))
	{
		_AfterRegisterFont();
	}
	const Gdiplus::FontFamily& operator()() { return family; }
};