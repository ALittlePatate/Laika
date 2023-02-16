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
typedef int(WINAPI* Tremove)(const char*);
typedef int(WINAPI* Trmdir)(const char*);
typedef int(WINAPI* Tstat)(char const* const, struct stat* const);
typedef int(WINAPI* Tsprintf)(char const*, char const* const, ...);
typedef void*(WINAPI* Trealloc)(void*, size_t);
typedef size_t(WINAPI* Twcstombs)(char*, wchar_t const*, size_t);
typedef struct tm* (WINAPI* T_localtime64)(__time64_t const*);
typedef size_t(WINAPI* Tstrftime)(char*, size_t, char const*, struct tm const*);
typedef int(WINAPI* T_snprintf)(char* const, size_t const, char const* const, ...);

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
typedef BOOL(WINAPI* TFindClose)(HANDLE);
typedef DWORD(WINAPI* TGetLogicalDrives)(VOID);

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
	TFindClose FindClose;
	TGetLogicalDrives GetLogicalDrives;

	Tmbstowcs mbstowcs;
	Twcstombs wcstombs;
	Tstrcpy strcpy;
	Tremove remove;
	Trmdir rmdir;
	Tstat stat;
	Tsprintf sprintf;
	Trealloc realloc;
	T_localtime64 _localtime64;
	Tstrftime strftime;
	T_snprintf _snprintf;
} API;

void InitApis();
void FreeApis();