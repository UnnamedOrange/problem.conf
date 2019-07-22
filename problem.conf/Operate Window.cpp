#include "pch.h"
#include "resource.h"
#include "Operate Window.h"

#include "main.h"
#include "Conf Collection.h"

BOOL SetFocusEx(HWND hwnd)
{
	HWND hwndLast;
	DWORD dwThreadId;
	SetForegroundWindow(hwnd);
	dwThreadId = GetWindowThreadProcessId(hwnd, 0);
	AttachThreadInput(GetCurrentThreadId(), dwThreadId, TRUE);
	hwndLast = SetFocus(hwnd);
	AttachThreadInput(GetCurrentThreadId(), dwThreadId, FALSE);
	if (hwndLast == hwnd)
		return FALSE;
	return TRUE;
}

//message
INT_PTR CALLBACK OperateWindow::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);

		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
	default:
		return FALSE;
	}
	return TRUE;
}

VOID InitCombo(HWND hWnd)
{
	struct InitList
	{
		int id;
		int sel;
		std::initializer_list<const char*> txt;
	};
	const InitList list[] = {
	{ IDC_COMBO_NAME3, 0,{"in", "txt"} },
	{ IDC_COMBO_NAME5, 0,{"ans", "out", "txt" } },
	{ IDC_COMBO_TL, 0,{ "1", "2", "3", "5", "7", "10", "20" } },
	{ IDC_COMBO_ML, 3,{ "64", "128", "256", "512", "1024", "8", "233" } },
	{ IDC_COMBO_OUTL, 0,{ "64" } },
	{ IDC_COMBO_COMP, 1,
	{
		//要一一对应
		"Special Judge",
		"全文比较",
		"有序整数序列",
		"无序整数序列",
		"浮点数序列(1e-4)",
		"浮点数序列(1e-6)",
		"浮点数序列(1e-9)"
	} },
	{ IDC_COMBO_CASE, 1,{ "5", "10", "20" } },
	{ IDC_COMBO_EXCASE, 0,{ "0", "1", "5" } },
	{ IDC_COMBO_SAMPLECASE, 0,{ "0", "1", "3" } },
	};

	for (int i = 0; i < sizeof(list) / sizeof(InitList); i++)
	{
		HWND hwnd = GetDlgItem(hWnd, list[i].id);
		for (const char* const* p = list[i].txt.begin(); p != list[i].txt.end(); p++)
			ComboBox_AddItemData(hwnd, *p);
		ComboBox_SetCurSel(hwnd, list[i].sel);
	}
}

VOID LoadConf(HWND hWnd)
{
	if (ConfCollection::strRaw.empty()) return;

}
bool SaveConf(HWND hWnd)
{
	ConfCollection::DataDomain& data = ConfCollection::data;
	const int buffer_size = 16 * 1024;
	char buffer[buffer_size];

#define GetEditText(id) Edit_GetText(GetDlgItem(hWnd, id), buffer, buffer_size)
#define GetComboBoxText(id) ComboBox_GetText(GetDlgItem(hWnd, id), buffer, buffer_size)
#define GetComboBoxUINT(id, x) GetComboBoxText(id); if (!isdigit(buffer[0])) return false; sscanf_s(buffer, "%u", &x);
	//name
	GetEditText(IDC_EDIT_NAME2);
	data.input_pre = buffer;

	GetComboBoxText(IDC_COMBO_NAME3);
	data.input_suf = buffer;

	GetEditText(IDC_EDIT_NAME4);
	data.output_pre = buffer;

	GetComboBoxText(IDC_COMBO_NAME5);
	data.output_suf = buffer;

	//limit
	GetComboBoxUINT(IDC_COMBO_TL, data.time_limit);
	GetComboBoxUINT(IDC_COMBO_ML, data.memory_limit);
	GetComboBoxUINT(IDC_COMBO_OUTL, data.output_limit);

	//comp
	data.use_builtin_checker = ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_COMBO_COMP));

	//data
	GetComboBoxUINT(IDC_COMBO_CASE, data.n_tests);
	GetComboBoxUINT(IDC_COMBO_EXCASE, data.n_ex_tests);
	GetComboBoxUINT(IDC_COMBO_SAMPLECASE, data.n_sample_tests);
#undef GetEditText
#undef GetComboBoxText
#undef GetComboBoxUINT

	ConfCollection().save();
	return true;
}
BOOL OperateWindow::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hWnd = hwnd;
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	SetFocusEx(hwnd);
	InitCombo(hwnd);
	return TRUE;
}
VOID OperateWindow::OnDestroy(HWND hwnd)
{
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	SetFocusEx(GetConsoleWindow());
}

VOID OperateWindow::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDCANCEL:
	{
		EndDialog(hwnd, FALSE);
		break;
	}
	case IDOK:
	{
		if (!SaveConf(hwnd))
		{
			MessageBox(hwnd, "输入了非法数据，保存失败", "错误", MB_ICONERROR);
			break;
		}
		std::cout << "已保存。" << std::endl;
		EndDialog(hwnd, TRUE);
		break;
	}
	case IDC_EDIT_NAME1:
	{
		if (codeNotify == EN_CHANGE)
		{
			char buffer[MAX_PATH];
			Edit_GetText(hwndCtl, buffer, MAX_PATH);
			Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_NAME2), buffer);
			Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_NAME4), buffer);
		}
		break;
	}
	default:
		break;
	}
}