#pragma once
#include "stdInc.h"
#include "resource.h"

static const char* builtin_checker[] =
{
	nullptr,
	"fcmp",
	"ncmp",
	"uncmp",
	"rcmp4",
	"rcmp6",
	"rcmp9"
};
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
			fcmp, // 全文比较（忽略文末回车，不忽略行末空格）
			ncmp, // 比较单行有序64位整数序列
			uncmp, // 比较单行无序64位整数序列，即排序后比较
			rcmp4, // 比较双精度浮点数序列，最大绝对或相对误差为 1.0e-4
			rcmp6, // 比较双精度浮点数序列，最大绝对或相对误差为 1.0e-6
			rcmp9, // 比较双精度浮点数序列，最大绝对或相对误差为 1.0e-9
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
	void create(const char* lpcFileName)
	{
		strFileName = lpcFileName;
	}
	void load(const char* lpcFileName)
	{
		strFileName = lpcFileName;

		// 读入文件
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
		// TODO: 分析

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
		out += "use_builtin_judger on\r\n";
#define saveString(x) out += #x " "; out += data.x; out += "\r\n";
#define saveUINT(x) out += #x " "; AddUINT(out, data.x); out += "\r\n"; 
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

		if (data.use_builtin_checker)
		{
			out += "use_builtin_checker ";
			out += builtin_checker[data.use_builtin_checker];
		}

		HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD dwWritten;
		WriteFile(hFile, out.c_str(), out.size(), &dwWritten, NULL);
		CloseHandle(hFile);
	}
};