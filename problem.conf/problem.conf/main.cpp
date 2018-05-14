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
	std::cout << "���� HELP �Բ鿴����" << std::endl;
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
	cout << "���õ����" << endl;
	cout << "HELP - ����" << endl;
	cout << "ABOUT - ����" << endl;
	cout << "CLS - ����" << endl;
	cout << "NEW - ���������½��ļ�" << endl;
	cout << "[*\\problem.conf] - �޸�һ���ļ�" << endl;
	cout << "[Folder] - �����ļ��й��� problem.conf" << endl;
}
void command_About()
{
	using std::cout;
	using std::endl;
	cout << "problem.conf" << endl;
	cout << "Copyright (c) 2018 Orange Software" << endl;
	cout << "һ���������" << endl;
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

	// eΪ�ļ�����ͷ
	const char stdFileName[] = "problem.conf";
	if (strcmp(e, stdFileName))
		return false;

	// bΪ����·����ͷ
	ConfCollection().load(b);
	return true;
}
int IsFolderValid()
{
	// TODO: �ļ���ģʽ
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

		// TODO: ��������
		if (ret = IsCommandValid())
			continue;

		std::cout << "��Ч��·����������� HELP �Բ鿴������" << std::endl;
	}
	return 0;
}