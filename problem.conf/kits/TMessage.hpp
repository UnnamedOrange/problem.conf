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

// TMessage

// Window Message Manager, Singleton

#pragma once
#include "TStdInclude.hpp"

class TMessage final
{
	UINT rec = 0;
	std::unordered_map<std::wstring, UINT> reg;
	TMessage() = default;

	UINT __Register(const std::wstring& name)
	{
		try
		{
			return reg.at(name);
		}
		catch (const std::exception&)
		{
			return reg[name] = WM_APP + rec++;
		}
	}

private:
	static TMessage& Singleton()
	{
		static TMessage instance;
		return instance;
	}
public:
	static UINT Register(const std::wstring& name)
	{
		return Singleton().__Register(name);
	}
};