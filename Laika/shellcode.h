#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "resolve_apis.h"

HANDLE FindProcessByArch(const wchar_t* arch);