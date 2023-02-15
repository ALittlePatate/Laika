#pragma once
#include <Windows.h>
#include <shlobj_core.h>
#include <wininet.h>
#include <winsock.h>
#include "utils.h"


typedef BOOL(WINAPI* Tconnect)(SOCKET, const SOCKADDR*, int);
typedef BOOL(WINAPI* Tsend)(SOCKET, const char*, int, int);
typedef BOOL(WINAPI* Trecv)(SOCKET, char*, int, int);
typedef BOOL(WINAPI* Tclosesocket)(SOCKET);
typedef u_short(WINAPI* Thtons)(u_short);
typedef u_long(WINAPI* Tinet_addr)(const char*);
typedef SOCKET(WINAPI* Tsocket)(int, int, int);
typedef int(WINAPI* TWSAStartup)(WORD, LPWSADATA);
typedef int(WINAPI* TWSAGetLastError)(void);

typedef void* (WINAPI* Tmemset)(void*, int, size_t);
typedef void* (WINAPI* Tmalloc)(size_t);
typedef void(WINAPI* Tfree)(void*);
typedef int(WINAPI* Tstrncmp)(const char*, const char*, size_t);
typedef size_t(WINAPI* Tmbstowcs)(wchar_t*, const char*, size_t);
typedef char*(WINAPI* Tstrcpy)(char*, const char*);

typedef BOOL(WINAPI* TReadFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL(WINAPI* TWriteFile)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL(WINAPI* TCloseHandle)(HANDLE);
typedef DWORD(WINAPI* TGetLastError)(void);
typedef BOOL(WINAPI* TCreatePipe)(PHANDLE, PHANDLE, LPSECURITY_ATTRIBUTES, DWORD);
typedef BOOL(WINAPI* TWaitForSingleObject)(HANDLE, DWORD);
typedef void(WINAPI* TSleep)(DWORD);
typedef HANDLE(WINAPI* TCreateThread)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
typedef BOOL(WINAPI* TTerminateThread)(HANDLE, DWORD);
typedef BOOL(WINAPI* TCreateProcessW)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
typedef BOOL(WINAPI* TTerminateProcess)(HANDLE, UINT);
typedef BOOL(WINAPI* TFreeLibrary)(HMODULE);
typedef FARPROC(WINAPI* TGetProcAddress)(HMODULE, LPCSTR);

typedef struct ApiList {
	Tconnect connect;
	Tsocket socket;
	Tsend send;
	Trecv recv;
	Tclosesocket closesocket;
	Thtons htons;
	Tinet_addr inet_addr;
	TWSAStartup WSAStartup;
	TWSAGetLastError WSAGetLastError;

	Tmemset memset;
	Tmalloc malloc;
	Tfree free;
	Tstrncmp strncmp;

	TReadFile ReadFile;
	TWriteFile WriteFile;
	TCloseHandle CloseHandle;
	TGetLastError GetLastError;
	TCreatePipe CreatePipe;
	TWaitForSingleObject WaitForSingleObject;
	TSleep Sleep;
	TCreateThread CreateThread;
	TTerminateThread TerminateThread;
	TCreateProcessW CreateProcessW;
	TTerminateProcess TerminateProcess;
	TFreeLibrary FreeLibrary;
	TGetProcAddress GetProcAddress;

	Tmbstowcs mbstowcs;
	Tstrcpy strcpy;
} API;

void InitApis();
void FreeApis();