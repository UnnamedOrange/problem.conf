#include "../../TKernel/TKernel/TKernel/kits/TKernel.h"
#include "resource.h"
#include "main.h"

#include "Conf Collection.h"
#include "Operate Window.h"

App app;

#define CODE_EXIT (-1)
#define CODE_CONTINUE (0)
#define CODE_INVALID_FILE (1)

const size_t buffer_size = 16 * 1024;
char buffer[buffer_size];

void command_New()
{
	SHGetSpecialFolderPathA(NULL, buffer, CSIDL_DESKTOP, FALSE);
	strcat_s(buffer, "\\problem.conf");
	ConfCollection().create(buffer);

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
	DWORD dwAttr = GetFileAttributesA(b);
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
		app.PrintTitle();
		break;
	}
	case HELP:
	{
		app.PrintHelp();
		break;
	}
	case ABOUT:
	{
		app.PrintAbout();
		break;
	}
	}
	return true;
}

INT App::Execute()
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

int _tmain()
{
	return app.Execute();
}