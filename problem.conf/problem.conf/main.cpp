#include "stdInc.h"
#include "resource.h"

#include "Conf Collection.h"
#include "Operate Window.h"

#define CODE_EXIT (-1)
#define CODE_CONTINUE (0)
#define CODE_INVALID_FILE (1)

const size_t buffer_size = 16 * 1024;
char buffer[buffer_size];

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

	//eΪ�ļ�����ͷ
	const char stdFileName[] = "problem.conf";
	if (strcmp(e, stdFileName))
		return false;

	//bΪ����·����ͷ
	ConfCollection().load(b);
	return true;
}

void PrintTitle()
{
	std::cout << "problem.conf" << std::endl;
	std::cout << "Orange Software" << std::endl;
	std::cout << "���� Help �Ի�ȡ����" << std::endl;
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

		//TODO: ��������
	}
	return 0;
}