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

// TTimer

#pragma once

#include "TStdInclude.hpp"

class TTimer final
{
	static std::unordered_map<UINT_PTR, TTimer*>& map()
	{
		static std::unordered_map<UINT_PTR, TTimer*> obj;
		return obj;
	}
	static void CALLBACK VirtualProc(HWND hwnd, UINT msg, UINT_PTR nTimerId, DWORD dwTime)
	{
		(*map().at(nTimerId)).call(dwTime);
	}
	UINT_PTR id = UINT_PTR(-1);

	std::function<void(DWORD dwTime)> func;

	VOID call(DWORD dwTime)
	{
		if (!func)
			throw std::runtime_error("no func is specified.");
		func(dwTime);
	}

public:
	TTimer() noexcept = default;
	TTimer(std::function<void(DWORD dwTime)> func) noexcept : func(func) {}
	~TTimer() noexcept { kill(); }

	VOID operator=(std::function<void(DWORD dwTime)> func)
	{
		this->func = func;
	}

	VOID set(UINT uElapse, BOOL rightaway)
	{
		if (!func)
			throw std::runtime_error("no func is specified.");
		kill();
		id = SetTimer(NULL, 0, uElapse, VirtualProc);
		map()[id] = this;
		if (rightaway) call(0);
	}
	VOID kill()
	{
		if (id != UINT_PTR(-1))
		{
			map().erase(id);
			KillTimer(NULL, id);
			id = UINT_PTR(-1);
		}
	}
};