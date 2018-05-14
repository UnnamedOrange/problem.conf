#include "stdInc.h"
#include "resource.h"

#include "Conf Collection.h"
#include "Operate Window.h"

#define CODE_EXIT (-1)
#define CODE_CONTINUE (0)
#define CODE_INVALID_FILE (1)

const size_t buffer_size = 16 * 1024;
char buffer[buffer_size];

void PrintTitle()
{
	std::cout << "problem.conf" << std::endl;
	std::cout << "Orange Software" << std::endl;
	std::cout << "键入 HELP 以查看帮助" << std::endl;
}

void command_New()
{
	SHGetSpecialFolderPath(NULL, buffer, CSIDL_DESKTOP, FALSE);
	_tcscat_s(buffer, "\\problem.conf");
	ConfCollection().create(buffer);

}
void command_Help()
{
	using std::cout;
	using std::endl;
	cout << "可用的命令：" << endl;
	cout << "HELP - 帮助" << endl;
	cout << "ABOUT - 关于" << endl;
	cout << "CLS - 清屏" << endl;
	cout << "NEW - 在桌面上新建文件" << endl;
	cout << "[*\\problem.conf] - 修改一个文件" << endl;
	cout << "[Folder] - 根据文件夹构建 problem.conf" << endl;
}
void command_About()
{
	using std::cout;
	using std::endl;
	cout << "problem.conf" << endl;
	cout << "Copyright (c) 2018 Orange Software" << endl;
	cout << "一个垃圾软件" << endl;
}

int IsPlemCfgValid()
{
	size_t length = strlen(buffer);
	char* b = buffer;
	char* e = buffer + length - 1;
	if (*b == '\"' && *e == '\"')
	{
		b++;
		*e = '\0';
		e--;
	}
	DWORD dwAttr = GetFileAttributes(b);
	if ((dwAttr == INVALID_FILE_ATTRIBUTES) || (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		return false;
	while (b <= e && *e != '\\')
		e--;
	e++;

	// e为文件名开头
	const char stdFileName[] = "problem.conf";
	if (strcmp(e, stdFileName))
		return false;

	// b为完整路径开头
	ConfCollection().load(b);
	return true;
}
int IsFolderValid()
{
	// TODO: 文件夹模式
	return false;
}
int IsCommandValid()
{
	static std::map<std::string, int> map;
	enum
	{
		EXIT = -1,
		HELP,
		ABOUT,
		NEW,
		CLS
	};
	if (!map.size())
	{
		struct InitList
		{
			int id;
			const char* str;
		};
		const InitList list[] =
		{
		{ EXIT, "EXIT" },
		{ HELP, "HELP" },
		{ ABOUT, "ABOUT" },
		{ NEW, "NEW" },
		{ CLS, "CLS" }
		};
		for (int i = 0; i < sizeof(list) / sizeof(InitList); i++)
			map[list[i].str] = list[i].id;
	}

	_strupr_s(buffer);
	if (!map.count(buffer))
		return false;

	switch (map[buffer])
	{
	case EXIT:
		return CODE_EXIT;
	case NEW:
	{
		command_New();
		OperateWindow::NewInstance();
		break;
	}
	case CLS:
	{
		system("cls");
		PrintTitle();
		break;
	}
	case HELP:
	{
		command_Help();
		break;
	}
	case ABOUT:
	{
		command_About();
		break;
	}
	}
	return true;
}

int _tmain()
{
	int ret = CODE_CONTINUE;
	PrintTitle();
	while (~ret)
	{
		std::cout << std::endl;
		std::cout << "Command> ";
		std::cin.getline(buffer, buffer_size);
		if (ret = IsPlemCfgValid())
		{
			OperateWindow::NewInstance();
			ret = CODE_CONTINUE;
			continue;
		}
		if (ret = IsFolderValid())
		{
			OperateWindow::NewInstance();
			ret = CODE_CONTINUE;
			continue;
		}

		// TODO: 解析命令
		if (ret = IsCommandValid())
			continue;

		std::cout << "无效的路径或命令。键入 HELP 以查看帮助。" << std::endl;
	}
	return 0;
}