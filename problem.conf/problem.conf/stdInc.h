#pragma once
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <iostream>
#include <vector>
#include <map>

#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

extern "C" const IMAGE_DOS_HEADER __ImageBase; //&__ImageBase
#define HINST (HINSTANCE)(&__ImageBase)