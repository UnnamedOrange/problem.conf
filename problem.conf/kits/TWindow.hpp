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

#include "TMessage.hpp"

class __TWindowBase
{
	// 窗口句柄相关
private:
	HWND __hWnd = NULL;
protected:
	VOID _SetHwnd(HWND hwnd) { __hWnd = hwnd; } // WM_DESTROY 后应设为 0
public:
	HWND GetHwnd() { return __hWnd; }

	// 内部窗口类
public:
	std::pair<PVOID, std::wstring> __GetIdentity()
	{
		auto vptr = *((PVOID*)this); // 取 this 指向的内容的前 8 个字节，即虚函数表的指针
		WCHAR strIdName[256];
		swprintf(strIdName, 256, L"TWindow::0x%p", vptr);
		return std::make_pair(vptr, std::wstring(strIdName));
	}
	// 外部窗口类
private:
	std::wstring __strClassName;
	VOID __SetClassName(const std::wstring& name) { __strClassName = name; }
	// 该方法由用户实现
	virtual std::wstring _ReturnClassName() = 0;
	// 如果注册了窗口类，则将 isRegistered 设为 TRUE。应始终返回 TRUE
	virtual BOOL _RegisterClasses(const std::wstring& strClassName, BOOL& isRegistered) = 0;
public:
	VOID RegisterClasses()
	{
		auto cls = _ReturnClassName();
		if (cls.empty())
			cls = __GetIdentity().second;
		__SetClassName(cls);

		static std::unordered_set<std::wstring> set;
		if (!set.count(cls))
		{
			BOOL isRegistered = false;
			if (!_RegisterClasses(cls, isRegistered))
				throw std::runtime_error("fail to register window class.");
			if (isRegistered)
				set.insert(GetClsName());
		}
	}
public:
	const std::wstring& GetClsName() { return __strClassName; }

	// 对外的创建窗口
private:
	virtual INT_PTR _Create() = 0;
public:
	INT_PTR Create()
	{
		RegisterClasses();
		if (GetHwnd())
			throw std::runtime_error("the window still exists.");
		return _Create();
	}
};

// 窗口基类，所有窗口都应是 TWindow
class TWindow : public __TWindowBase
{
	// 键盘加速键
private:
	HACCEL __hAccel = NULL;
public:
	VOID SetCurrentAccelerator(HACCEL handle) { __hAccel = handle; }
	VOID RevokeCurrentAccelerator() { __hAccel = NULL; }

	// 消息循环
public:
	int MsgLoop()
	{
		MSG msg;
		while (GetMessageW(&msg, NULL, 0, 0))
		{
			if (!__hAccel || !TranslateAcceleratorW(GetHwnd(), __hAccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
		return (int)msg.wParam;
	}

	// 自定义消息
public:

#pragma push_macro("DefineTMessage")
#define DefineTMessage(name) const UINT name = TMessage::Register(L#name)
	DefineTMessage(WM_NOTIFY_HOST_DESTROY); // WPARAM: this; LPARAM: null
	DefineTMessage(WM_NOTIFY_HOST_CREATE); // WPARAM: this, LPARAM: null
#undef DefineTMessage
#pragma pop_macro("DefineTMessage")

	// 预窗口过程
private:
	int __iWidth = NULL;
	int __iHeight = NULL;
protected:
	VOID _PreProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CREATE:
		case WM_INITDIALOG:
		{
			PostMessageW(GetHost(), WM_NOTIFY_HOST_CREATE, (WPARAM)this, 0);

			RECT rect;
			GetClientRect(hwnd, &rect);
			__iWidth = rect.right - rect.left;
			__iHeight = rect.bottom - rect.top;
			break;
		}
		case WM_SIZE:
			HANDLE_WM_SIZE(hwnd, wParam, lParam,
				[this](HWND hwnd, UINT state, int cx, int cy)
				{
					__iWidth = cx;
					__iHeight = cy;
				});
		default:
			break;
		}
	}
	VOID _PostProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DESTROY:
		{
			PostMessageW(GetHost(), WM_NOTIFY_HOST_DESTROY, (WPARAM)this, 0);
			break;
		}
		default:
			break;
		}
	}
public:
	const int& iWidth = __iWidth;
	const int& iHeight = __iHeight;

	// Host 支持
private:
	HWND __hwndHost = NULL;
public:
	VOID SetHost(HWND host) { __hwndHost = host; }
	HWND GetHost() const { return __hwndHost; }

	// 置窗口到中心
public:
	VOID CenterizeWindow()
	{
		int cxS = GetSystemMetrics(SM_CXSCREEN);
		int cyS = GetSystemMetrics(SM_CYSCREEN);
		MoveWindow(GetHwnd(), (cxS - iWidth) >> 1, (cyS - iHeight) >> 1, iWidth, iHeight, TRUE);
	}
};

class TWindowHost : public TWindow
{
	virtual std::wstring _ReturnClassName() override
	{
		return std::wstring();
	}
	virtual BOOL _RegisterClasses(const std::wstring& strClassName, BOOL& isRegistered) override
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
		return isRegistered = RegisterClassExW(&wndclassex);
	}
	virtual INT_PTR _Create() override
	{
		CreateWindowExW(0,
			GetClsName().c_str(), param.strWindowName.c_str(),
			WS_OVERLAPPED,
			0, 0, 0, 0,
			NULL, NULL, HINST, this);
		return 0;
	}

	static LRESULT CALLBACK VirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		TWindowHost* p = (TWindowHost*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

		if (message == WM_NCCREATE)
		{
			p = (TWindowHost*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
			p->_SetHwnd(hwnd);
		}

		if (p) p->_PreProc(hwnd, message, wParam, lParam);
		if (p) p->__PreProc(hwnd, message, wParam, lParam);

		LRESULT ret;
		if (p)
			ret = p->WndProc(hwnd, message, wParam, lParam);
		else
			ret = DefWindowProcW(hwnd, message, wParam, lParam);

		if (p) p->_PostProc(hwnd, message, wParam, lParam);

		if (p && message == WM_DESTROY)
			p->_SetHwnd(0);

		return ret;
	}
	VOID __PreProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		default:
		{
			if (false) {}
			else if (message == WM_NOTIFY_HOST_CREATE)
			{
				nAliveSubwindow++;
			}
			else if (message == WM_NOTIFY_HOST_DESTROY)
			{
				nAliveSubwindow--;
				if (container.at((TWindow*)wParam).bAutoErase)
				{
					container.erase((TWindow*)wParam);
					delete (TWindow*)wParam;
				}

				if (bAutoDestroySelf && !nAliveSubwindow)
					PostMessageW(GetHwnd(), WM_CLOSE, 0, 0);
			}
			break;
		}
		}
	}

private:
	virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0; // 必须加上 CALLBACK 标记

private:
	struct CreateParam
	{
		std::wstring strWindowName;
	} param;
public:
	CreateParam& AccessParam() { return param; }

private:
	struct SubwindowExtraInfo
	{
		BOOL bAutoErase = TRUE;
	};
	std::unordered_map<TWindow*, SubwindowExtraInfo> container;
public:
	template<typename TWindow_Type>
	TWindow_Type* Push(TWindow_Type* newWindow, BOOL bAutoErase = TRUE)
	{
		if (!GetHwnd())
			throw std::runtime_error("only after you create the host can you (Push).");
		if (newWindow->GetHwnd())
			throw std::runtime_error("only before you create the window can you (Push).");
		if (container.count(newWindow))
			throw std::runtime_error("you can't (Push) the same window twice.");

		newWindow->SetHost(GetHwnd());
		auto& info = container[newWindow];
		info.bAutoErase = bAutoErase;
		return newWindow;
	}

private:
	int nAliveSubwindow = 0;
public:
	BOOL bAutoDestroySelf = TRUE;
};

class TWindowPopup : public TWindow
{
	virtual std::wstring _ReturnClassName() override
	{
		return std::wstring();
	}
	virtual BOOL _RegisterClasses(const std::wstring& strClassName, BOOL& isRegistered) override
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
		return isRegistered = RegisterClassExW(&wndclassex);
	}
	virtual INT_PTR _Create() override
	{
		CreateWindowExW(param.dwExStyle,
			GetClsName().c_str(), param.strWindowName.c_str(),
			param.dwStyle,
			param.x, param.y, param.nWidth, param.nHeight,
			param.hWndParent, param.hMenu, HINST, this);
		return 0;
	}

	static LRESULT CALLBACK VirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		TWindowPopup* p = (TWindowPopup*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

		if (message == WM_NCCREATE)
		{
			p = (TWindowPopup*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
			p->_SetHwnd(hwnd);
		}

		if (p) p->_PreProc(hwnd, message, wParam, lParam);

		LRESULT ret;
		if (p)
			ret = p->WndProc(hwnd, message, wParam, lParam);
		else
			ret = DefWindowProcW(hwnd, message, wParam, lParam);

		if (p) p->_PostProc(hwnd, message, wParam, lParam);

		if (p && message == WM_DESTROY)
			p->_SetHwnd(0);

		return ret;
	}

private:
	virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0; // 必须加上 CALLBACK 标记

private:
	struct CreateParam
	{
		std::wstring strWindowName;
		DWORD dwStyle = WS_POPUPWINDOW;
		DWORD dwExStyle = 0;
		int x = CW_USEDEFAULT;
		int y = CW_USEDEFAULT;
		int nWidth = 380;
		int nHeight = 250;
		HWND hWndParent = NULL;
		HMENU hMenu = NULL;
	} param;
public:
	CreateParam& AccessParam() { return param; }
};

class TWindowChild : public TWindow
{
	virtual std::wstring _ReturnClassName() override
	{
		return std::wstring();
	}
	virtual BOOL _RegisterClasses(const std::wstring& strClassName, BOOL& isRegistered) override
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
		return isRegistered = RegisterClassExW(&wndclassex);
	}
	virtual INT_PTR _Create() override
	{
		CreateWindowExW(param.dwExStyle,
			GetClsName().c_str(), param.strWindowName.c_str(),
			param.dwStyle,
			param.x, param.y, param.nWidth, param.nHeight,
			param.hWndParent, param.hMenu, HINST, this);
		return 0;
	}

	static LRESULT CALLBACK VirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		TWindowChild* p = (TWindowChild*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

		if (message == WM_CREATE)
		{
			p = (TWindowChild*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
			p->_SetHwnd(hwnd);
		}

		p->_PreProc(hwnd, message, wParam, lParam);

		LRESULT ret;
		if (p)
			ret = p->WndProc(hwnd, message, wParam, lParam);
		else
			ret = DefWindowProcW(hwnd, message, wParam, lParam);

		if (p && message == WM_DESTROY)
			p->_SetHwnd(0);

		return ret;
	}

private:
	virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0; // 必须加上 CALLBACK 标记

private:
	struct CreateParam
	{
		std::wstring strWindowName;
		DWORD dwStyle = WS_CHILDWINDOW | WS_VISIBLE;
		DWORD dwExStyle = 0;
		int x = 0;
		int y = 0;
		int nWidth = 0;
		int nHeight = 0;
		HWND hWndParent = NULL;
		HMENU hMenu = NULL;
	} param;
public:
	CreateParam& AccessParam() { return param; }
};

class TDialogBox : public TWindow
{
	virtual std::wstring _ReturnClassName() override
	{
		return std::wstring(L"#32770");
	}
	virtual BOOL _RegisterClasses(const std::wstring& strClassName, BOOL& isRegistered) override
	{
		return TRUE;
	}
	virtual INT_PTR _Create() override
	{
		return DialogBoxParamW(HINST, param.lpTemplateName, param.hWndParent, VirtualProc, (LPARAM)this);
	}

	static INT_PTR CALLBACK VirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		TDialogBox* p = (TDialogBox*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

		if (message == WM_INITDIALOG)
		{
			p = (TDialogBox*)(lParam);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
			p->_SetHwnd(hwnd);
		}

		if (p) p->_PreProc(hwnd, message, wParam, lParam);

		LRESULT ret = FALSE;
		if (p)
			ret = p->WndProc(hwnd, message, wParam, lParam);

		if (p) p->_PostProc(hwnd, message, wParam, lParam);

		if (p && message == WM_DESTROY)
			p->_SetHwnd(0);

		return ret;
	}

private:
	virtual INT_PTR CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0; // 必须加上 CALLBACK 标记

private:
	struct CreateParam
	{
		LPCWSTR lpTemplateName = nullptr;
		HWND hWndParent = NULL;
	} param;
public:
	CreateParam& AccessParam() { return param; }
};

class TCreateDialog : public TWindow
{
	virtual std::wstring _ReturnClassName() override
	{
		return std::wstring(L"#32770");
	}
	virtual BOOL _RegisterClasses(const std::wstring& strClassName, BOOL& isRegistered) override
	{
		return TRUE;
	}
	virtual INT_PTR _Create() override
	{
		return (INT_PTR)CreateDialogParamW(HINST, param.lpTemplateName, param.hWndParent, VirtualProc, (LPARAM)this);
	}

	static INT_PTR CALLBACK VirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		TCreateDialog* p = (TCreateDialog*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

		if (message == WM_INITDIALOG)
		{
			p = (TCreateDialog*)(lParam);
			SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)p);
			p->_SetHwnd(hwnd);
		}

		if (p) p->_PreProc(hwnd, message, wParam, lParam);

		LRESULT ret = FALSE;
		if (p)
			ret = p->WndProc(hwnd, message, wParam, lParam);

		if (p) p->_PostProc(hwnd, message, wParam, lParam);

		if (p && message == WM_DESTROY)
			p->_SetHwnd(0);

		return ret;
	}

private:
	virtual INT_PTR CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0; // 必须加上 CALLBACK 标记

private:
	struct CreateParam
	{
		LPCWSTR lpTemplateName = nullptr;
		HWND hWndParent = NULL;
	} param;
public:
	CreateParam& AccessParam() { return param; }
};