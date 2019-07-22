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

// TWindow

#pragma once

#include "TStdInclude.hpp"

class TWindowBase abstract
{
	friend class TWindowBaseEx;

	// 窗口句柄相关
private:
	HWND hWnd = NULL;
public:
	HWND GetHwnd() { return hWnd; }
private:
	VOID _SetHwnd(HWND hwnd) { hWnd = hwnd; }

	// 内部窗口类
private:
	std::pair<PVOID, std::wstring> GetIdentity()
	{
		auto vptr = *((PVOID*)this); // 取 this 指向的内容的前 8 个字节，即虚函数表的指针
		WCHAR clsName[256];
		swprintf_s(clsName, L"cls::0x%p", vptr);
		return std::make_pair(vptr, std::wstring(clsName));
	}
	// 子类自主注册以 strClassName 为类名的窗口类，若注册成功将 bIsRegistered 置为 TRUE。应始终返回 TRUE。
	virtual BOOL _RegisterClasses(const std::wstring& strClassName, BOOL& bIsRegistered) = 0;
	std::wstring strClassName;
public:
	const std::wstring& GetClsName() { return strClassName; }
	BOOL SetClsName(const std::wstring& strName) // 在第一次创建前调用，将以设置的类名注册类
	{
		if (strClassName.empty())
		{
			strClassName = strName;
			return TRUE;
		}
		else
			return FALSE;
	}

private:
	// 内部函数，将作为创建窗口时的第一步，与子类无关
	BOOL InitInstance()
	{
		if (hWnd) return 0;
		static std::set<PVOID> set;
		auto id = GetIdentity();
		if (!set.count(id.first))
		{
			BOOL bIsRegistered = FALSE;
			if (!_RegisterClasses(strClassName.empty() ? id.second : strClassName, bIsRegistered))
				return FALSE;
			if (bIsRegistered) set.insert(id.first);
		}
		if (strClassName.empty()) strClassName = id.second;
		return TRUE;
	}
	// 调用 InitInstance 后紧接调用子类提供的该函数
	virtual BOOL _Create(PVOID param) = 0;
	// 内部创建窗口接口
protected:
	// 子类使用该函数创建窗口，利用参数传递信息
	BOOL Create(PVOID param)
	{
		InitInstance();
		return _Create(param);
	}
};

// 用于功能拓展
class TWindowBaseEx abstract : public TWindowBase
{
	friend class TWindowVirtual;
	friend class TDialogVirtual;

public:
	using TWindowBase::TWindowBase;

	// 窗口句柄相关
private:
	VOID SetHwnd(HWND hwnd) { _SetHwnd(hwnd); }

	// 消息循环
public:
	INT MessageLoop()
	{
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (!hAccel || !TranslateAccelerator(hWnd, hAccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		return (INT)msg.wParam;
	}

	// 键盘加速键
private:
	HACCEL hAccel = NULL;
public:
	VOID SetCurrentAccelerator(HACCEL handle) { hAccel = handle; }
	VOID RevokeCurrentAccelerator() { hAccel = NULL; }

	// 自动记录窗口位置与客户区大小
private:
	int _iLeft = 0, _iTop = 0, _iWidth = 0, _iHeight = 0;
	VOID _OnMove(HWND hwnd, int x, int y)
	{
		_iLeft = x;
		_iTop = y;
	}
	VOID _OnSize(HWND hwnd, UINT state, int cx, int cy)
	{
		_iWidth = cx;
		_iHeight = cy;
	}
public:
	const int& iLeft = _iLeft;
	const int& iTop = _iTop;
	const int& iWidth = _iWidth;
	const int& iHeight = _iHeight;

	// 窗口位置
public:
	VOID CenterizeWindow(BOOL bActivate)
	{
		int cxS = GetSystemMetrics(SM_CXSCREEN);
		int cyS = GetSystemMetrics(SM_CYSCREEN);
		int width = iWidth;
		int height = iHeight;
		if (!(iWidth && iHeight))
		{
			RECT rect;
			GetWindowRect(GetHwnd(), &rect);
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
		}
		SetWindowPos(GetHwnd(), NULL, (cxS - width) >> 1, (cyS - height) >> 1, NULL, NULL, SWP_NOSIZE | (bActivate ? 0 : SWP_NOACTIVATE));
	}
};

// VirtualProc
class TWindowVirtual abstract : public TWindowBaseEx
{
public:
	using TWindowBaseEx::TWindowBaseEx;

private:
	virtual VOID BeforeVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual VOID AfterVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	static LRESULT CALLBACK VirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		TWindowVirtual* p = (TWindowVirtual*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

		if (message == WM_CREATE)
		{
			p = (TWindowVirtual*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
			p->SetHwnd(hwnd);
		}

		LRESULT ret;
		if (p)
		{
			p->BeforeVirtualProc(hwnd, message, wParam, lParam);
			switch (message)
			{
				HANDLE_MSG(hwnd, WM_CREATE, p->OnCreate);
			case WM_DESTROY: HANDLE_WM_DESTROY(hwnd, wParam, lParam, p->OnDestroy); break;
			case WM_MOVE: HANDLE_WM_MOVE(hwnd, wParam, lParam, p->_OnMove); break;
			case WM_SIZE: HANDLE_WM_SIZE(hwnd, wParam, lParam, p->_OnSize); break;
			default:
				break;
			}
			ret = p->WndProc(hwnd, message, wParam, lParam);
			p->AfterVirtualProc(hwnd, message, wParam, lParam);
		}
		else ret = DefWindowProcW(hwnd, message, wParam, lParam);

		if (p && message == WM_DESTROY)
			p->SetHwnd(NULL);

		return ret;
	}
private:
	virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0; // 必须加上 CALLBACK 标记
	virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) = 0; // 必须在此完成 WM_CREATE
	virtual VOID OnDestroy(HWND hwnd) = 0;

	// 注册窗口类
private:
	virtual BOOL _RegisterClasses(const std::wstring& strClassName, BOOL& bIsRegistered) override
	{
		WNDCLASSEXW wndclassex = { sizeof(WNDCLASSEXW) };
		wndclassex.style = CS_HREDRAW | CS_VREDRAW;
		wndclassex.lpfnWndProc = VirtualProc;
		wndclassex.cbClsExtra = 0;
		wndclassex.cbWndExtra = 0;
		wndclassex.hInstance = HINST;
		wndclassex.hIcon = NULL;
		wndclassex.hIconSm = NULL;
		wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclassex.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
		wndclassex.lpszMenuName = NULL;
		wndclassex.lpszClassName = strClassName.c_str();
		return bIsRegistered = RegisterClassExW(&wndclassex);
	}
};

// Host 窗口
class TWindowHost : public TWindowVirtual
{
public:
	using TWindowVirtual::TWindowVirtual;

	// Virtual Proc
private:
	virtual VOID BeforeVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}
	virtual VOID AfterVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}

	// 窗口创建参数
private:
	struct TWindowHostParam
	{
		TWindowHostParam() = default;
	};
	virtual BOOL _Create(PVOID param) override
	{
		BOOL bRet = !!CreateWindowExW(NULL, GetClsName().c_str(), L"",
			NULL,
			0,
			0,
			0,
			0,
			NULL, NULL, HINST, this);
		if (bRet)
		{
			SetWindowPos(GetHwnd(), HWND_BOTTOM, 0, 0, 0, 0, NULL);
		}
		return bRet;
	}

public:
	HWND CreateHost()
	{
		Create(&TWindowHostParam());
		return GetHwnd();
	}

	// 扩展功能
};

// 正常弹出窗口
class TWindowPopup : public TWindowVirtual
{
public:
	using TWindowVirtual::TWindowVirtual;

	// Virtual Proc
private:
	virtual VOID BeforeVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}
	virtual VOID AfterVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}

	// 窗口创建参数
private:
	struct TWindowPopupParam
	{
		HWND hwndHost;
		TWindowPopupParam() = default;
		TWindowPopupParam(HWND hwndHost) : hwndHost(hwndHost) {}
	};
	virtual BOOL _Create(PVOID param) override
	{
		const TWindowPopupParam& p = *((TWindowPopupParam*)param);
		hwndHost = p.hwndHost;
		return !!CreateWindowExW(NULL, GetClsName().c_str(), L"",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL, NULL, HINST, this);
	}

	// 外部接口
public:
	HWND CreatePopup(HWND hwndHost)
	{
		Create(&TWindowPopupParam(hwndHost));
		return GetHwnd();
	}

	// 扩展功能
public:
	VOID ShowAndUpdate() // 在调用 CreatePopup 后调用
	{
		ShowWindow(GetHwnd(), SW_SHOW);
		UpdateWindow(GetHwnd());
	}

private:
	HWND hwndHost; // Host 窗口相关
public:
	HWND GetHostWindow() { return hwndHost; }
};

// 子窗口
class TWindowChild : public TWindowVirtual
{
public:
	using TWindowVirtual::TWindowVirtual;

	// Virtual Proc
private:
	virtual VOID BeforeVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}
	virtual VOID AfterVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}

	// 窗口创建参数
private:
	struct TWindowChildParam
	{
		HWND hwndParent;
		HMENU id;
		TWindowChildParam() = default;
		TWindowChildParam(HWND hwndParent, HMENU id) : hwndParent(hwndParent), id(id) {}
	};
	virtual BOOL _Create(PVOID param) override
	{
		const TWindowChildParam& p = *((TWindowChildParam*)param);
		return !!CreateWindowExW(NULL, GetClsName().c_str(), L"",
			WS_CHILD | WS_VISIBLE,
			0,
			0,
			0,
			0,
			p.hwndParent, p.id, HINST, this);
	}

	// 外部接口
public:
	HWND CreateChild(HWND hwndParent, HMENU id)
	{
		Create(&TWindowChildParam(hwndParent, id));
		return GetHwnd();
	}
};

// VirtualProc for Dialog
class TDialogVirtual abstract : public TWindowBaseEx
{
	using TWindowBaseEx::TWindowBaseEx;

private:
	virtual VOID BeforeVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual VOID AfterVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
protected:
	static INT_PTR CALLBACK VirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		TDialogVirtual* p = (TDialogVirtual*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

		if (message == WM_INITDIALOG)
		{
			p = (TDialogVirtual*)lParam;
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
			p->SetHwnd(hwnd);
		}

		LRESULT ret;
		if (p)
		{
			p->BeforeVirtualProc(hwnd, message, wParam, lParam);
			switch (message)
			{
			case WM_INITDIALOG: HANDLE_WM_INITDIALOG(hwnd, wParam, lParam,
				[&](HWND hwnd, HWND hwndFocus, LPARAM lParam) -> BOOL
				{
					p->initDialogStruct = InitDialogStruct(hwnd, hwndFocus, lParam);
					SetTimer(hwnd, 0, 0, nullptr);
					return TRUE;
				}); break;
			case WM_TIMER: HANDLE_WM_TIMER(hwnd, wParam, lParam,
				[&](HWND hwnd, UINT id) -> void
				{
					if (p->initDialogStruct.bReady && !id)
					{
						KillTimer(hwnd, 0);
						PostMessage(hwnd, WM_USER, NULL, NULL);
					}
				}); break;
			case WM_USER:
			{
				if (p->initDialogStruct.bReady)
				{
					p->initDialogStruct.bReady = FALSE;
					p->OnInitDialog(p->initDialogStruct.hwnd,
						p->initDialogStruct.hwndFocus,
						p->initDialogStruct.lParam);
					PostMessageW(hwnd, WM_SIZE, (WPARAM)(UINT)(SIZE_RESTORED), MAKELPARAM((p->iWidth), (p->iHeight)));
				}
				break;
			}

			case WM_DESTROY: HANDLE_WM_DESTROY(hwnd, wParam, lParam, p->OnDestroy); break;
			case WM_MOVE: HANDLE_WM_MOVE(hwnd, wParam, lParam, p->_OnMove); break;
			case WM_SIZE: HANDLE_WM_SIZE(hwnd, wParam, lParam, p->_OnSize); break;
			default:
				break;
			}
			ret = p->WndProc(hwnd, message, wParam, lParam);
			p->AfterVirtualProc(hwnd, message, wParam, lParam);
		}
		else ret = FALSE;

		if (p && message == WM_DESTROY)
			p->SetHwnd(NULL);

		return ret;
	}
private:
	struct InitDialogStruct
	{
		BOOL bReady = FALSE;
		HWND hwnd = NULL;
		HWND hwndFocus = NULL;
		LPARAM lParam = NULL;
		InitDialogStruct() = default;
		InitDialogStruct(HWND hwnd, HWND hwndFocus, LPARAM lParam) :
			bReady(TRUE), hwnd(hwnd), hwndFocus(hwndFocus), lParam(lParam) {}
	} initDialogStruct;
	virtual INT_PTR CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0; // 必须加上 CALLBACK 标记
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) = 0; // 收到 WM_INITDIALOG 后，将会通过时钟将消息延迟，故该消息不是真实的 WM_INITDIALOG
	virtual VOID OnDestroy(HWND hwnd) = 0;

	// 注册窗口类
private:
	virtual BOOL _RegisterClasses(const std::wstring& strClassName, BOOL& bIsRegistered) override
	{
		bIsRegistered = FALSE;
		return TRUE;
	}
};

// DialogBox
class TDialogBox abstract : public TDialogVirtual
{
public:
	using TDialogVirtual::TDialogVirtual;

	// Virtual Proc
private:
	virtual VOID BeforeVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}
	virtual VOID AfterVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}

	// 窗口创建参数
private:
	struct TDialogBoxParam
	{
		LPCWSTR lpTemplateName = NULL;
		HWND hwndParent = NULL;
		INT_PTR iRet = NULL;
		TDialogBoxParam() = default;
		TDialogBoxParam(LPCWSTR lpTemplateName, HWND hwndParent) :
			lpTemplateName(lpTemplateName), hwndParent(hwndParent) {}
	};
	virtual BOOL _Create(PVOID param) override
	{
		auto ptr = (TDialogBoxParam*)param;
		if (!ptr) return FALSE;
		ptr->iRet = DialogBoxParamW(HINST, ptr->lpTemplateName, ptr->hwndParent, VirtualProc, (LPARAM)this);
		return TRUE;
	}

	// 外部接口
public:
	INT_PTR DialogueBox(LPCWSTR lpTemplateName, HWND hwndParent)
	{
		TDialogBoxParam param(lpTemplateName, hwndParent);
		return Create(&param) ? param.iRet : 0;
	}
};

// CreateBox
class TCreateDialog abstract : public TDialogVirtual
{
public:
	using TDialogVirtual::TDialogVirtual;

	// Virtual Proc
private:
	virtual VOID BeforeVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}
	virtual VOID AfterVirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override {}

	// 窗口创建参数
private:
	struct TCreateDialogParam
	{
		LPCWSTR lpTemplateName = NULL;
		HWND hwndParent = NULL;
		INT_PTR iRet = NULL;
		TCreateDialogParam() = default;
		TCreateDialogParam(LPCWSTR lpTemplateName, HWND hwndParent) :
			lpTemplateName(lpTemplateName), hwndParent(hwndParent) {}
	};
	virtual BOOL _Create(PVOID param) override
	{
		auto ptr = (TCreateDialogParam*)param;
		if (!ptr) return FALSE;
		CreateDialogParamW(HINST, ptr->lpTemplateName, ptr->hwndParent, VirtualProc, (LPARAM)this);
		return TRUE;
	}

	// 外部接口
public:
	HWND CreateDialogue(LPCWSTR lpTemplateName, HWND hwndParent)
	{
		TCreateDialogParam param(lpTemplateName, hwndParent);
		Create(&param);
		return GetHwnd();
	}
};