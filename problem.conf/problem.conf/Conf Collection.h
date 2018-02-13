#pragma once
#include "stdInc.h"
#include "resource.h"

struct ConfCollection
{
	static std::string strFileName;
	static std::string strRaw;

	struct DataDomain
	{
		std::string name;
		std::string input_pre;
		std::string input_suf;
		std::string output_pre;
		std::string output_suf;

		UINT time_limit;
		UINT memory_limit;
		UINT output_limit;

		enum COMPARE
		{
			spj,
			fcmp, //ȫ�ıȽϣ�������ĩ�س�����������ĩ�ո�
			ncmp, //�Ƚϵ�������64λ��������
			uncmp, //�Ƚϵ�������64λ�������У��������Ƚ�
			rcmp4, //�Ƚ�˫���ȸ��������У������Ի�������Ϊ 1.0e-4
			rcmp6, //�Ƚ�˫���ȸ��������У������Ի�������Ϊ 1.0e-6
			rcmp9, //�Ƚ�˫���ȸ��������У������Ի�������Ϊ 1.0e-9
		};
		UINT use_builtin_checker;

		UINT n_tests;
		UINT n_ex_tests;
		UINT n_sample_tests;
	};
	static DataDomain data;

	void clear()
	{
		strFileName.clear();
		strRaw.clear();
	}
	void load(const char* lpcFileName)
	{
		strFileName = lpcFileName;

		//�����ļ�
		HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwSize = GetFileSize(hFile, NULL);
		strRaw.resize(dwSize);
		DWORD dwRead;
		ReadFile(hFile, (LPVOID)&strRaw[0], dwSize, &dwRead, NULL);
		CloseHandle(hFile);

		analyse();
	}
	void analyse()
	{

	}
	static inline void AddUINT(std::string& str, UINT x)
	{
		char buffer[16];
		sprintf_s(buffer, "%u", x);
		str += buffer;
	}
	void save()
	{
		std::string out;
#define saveString(x) out += #x " "; out += data.x; out += "\n";
#define saveUINT(x) out += #x " "; AddUINT(out, data.x); out += "\n"; 
		saveString(input_pre);
		saveString(input_suf);
		saveString(output_pre);
		saveString(output_suf);

		saveUINT(time_limit);
		saveUINT(memory_limit);
		saveUINT(output_limit);

		saveUINT(n_tests);
		saveUINT(n_ex_tests);
		saveUINT(n_sample_tests);
#undef saveString
#undef saveUINT
	}
};