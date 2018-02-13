#pragma once
#include "stdInc.h"
#include "resource.h"

struct OperateWindow
{
	HWND hWnd;

public:
	static VOID NewInstance() { DialogBox(HINST, MAKEINTRESOURCE(IDD_OPERATE), NULL, VirtualProc); }

public:
	OperateWindow() = delete;
	OperateWindow(const HWND& hWnd) : hWnd(hWnd) {}

private:
	static INT_PTR CALLBACK VirtualProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		OperateWindow* p = NULL;
		p = (OperateWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (!p)
		{
			p = new OperateWindow(hwnd);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p);
		}
		INT_PTR ret = p->DlgProc(hwnd, message, wParam, lParam);
		if (message == WM_DESTROY) delete p;
		return ret;
	}
	INT_PTR CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	VOID OnDestroy(HWND hwnd);

	VOID OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
};