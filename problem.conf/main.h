#pragma once
#include "pch.h"

class App : public TApplication
{

public:
	virtual INT Execute() override;

public:
	VOID PrintHelp()
	{
		using std::cout;
		using std::endl;
		cout << "可用的命令：" << endl;
		cout << "HELP - 帮助" << endl;
		cout << "ABOUT - 关于" << endl;
		cout << "CLS - 清屏" << endl;
		cout << "NEW - 在桌面上新建文件" << endl;
		cout << "[*\\problem.conf] - 修改一个文件" << endl;
		//cout << "[Folder] - 根据文件夹构建 problem.conf" << endl;
	}
	VOID PrintAbout()
	{
		using std::cout;
		using std::endl;
		cout << "problem.conf" << endl;
		cout << "Copyright (c) 2019 Orange Software" << endl;
		cout << "Version " << GetFileInfo().GetVer1() << "." <<
			GetFileInfo().GetVer2() << "." <<
			GetFileInfo().GetVer3() << "." <<
			GetFileInfo().GetVer4() << endl;
		cout << "一个垃圾软件" << endl;
	}
	VOID PrintTitle()
	{
		using std::cout;
		using std::endl;
		cout << "problem.conf" << endl;
		cout << "Orange Software" << endl;
		cout << "键入 HELP 以查看帮助" << endl;
	}
};

extern App app;