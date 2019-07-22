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
		cout << "���õ����" << endl;
		cout << "HELP - ����" << endl;
		cout << "ABOUT - ����" << endl;
		cout << "CLS - ����" << endl;
		cout << "NEW - ���������½��ļ�" << endl;
		cout << "[*\\problem.conf] - �޸�һ���ļ�" << endl;
		//cout << "[Folder] - �����ļ��й��� problem.conf" << endl;
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
		cout << "һ���������" << endl;
	}
	VOID PrintTitle()
	{
		using std::cout;
		using std::endl;
		cout << "problem.conf" << endl;
		cout << "Orange Software" << endl;
		cout << "���� HELP �Բ鿴����" << endl;
	}
};

extern App app;