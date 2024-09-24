#pragma once
#include <Windows.h>
//#include <winternl.h>
#include <stddef.h>

extern HANDLE _crt_heap;

#ifndef _MSC_VER
#define LAIKA_NOOPT __attribute__((optimize("O0")))
#else
#define LAIKA_NOOPT
#endif

typedef long NTSTATUS;

#define FD_SET_(fd, set) do { \
    if (((fd_set FAR *)(set))->fd_count < FD_SETSIZE) \
        ((fd_set FAR *)(set))->fd_array[((fd_set FAR *)(set))->fd_count++]=(fd);\
} while(0)

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING* PUNICODE_STRING;
typedef const UNICODE_STRING* PCUNICODE_STRING;

typedef struct _PEB_LDR_DATA {
    BYTE Reserved1[8];
    PVOID Reserved2[3];
    LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY {
    PVOID Reserved1[2];
    LIST_ENTRY InMemoryOrderLinks;
    PVOID Reserved2[2];
    PVOID DllBase;
    PVOID Reserved3[2];
    UNICODE_STRING FullDllName;
    BYTE Reserved4[8];
    PVOID Reserved5[3];
    union {
        ULONG CheckSum;
        PVOID Reserved6;
    } DUMMYUNIONNAME;
    ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    BYTE Reserved1[16];
    PVOID Reserved2[10];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

typedef
VOID
(NTAPI* PPS_POST_PROCESS_INIT_ROUTINE) (
    VOID
    );

typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[1];
    PVOID Reserved3[2];
    PPEB_LDR_DATA Ldr;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    PVOID Reserved4[3];
    PVOID AtlThunkSListPtr;
    PVOID Reserved5;
    ULONG Reserved6;
    PVOID Reserved7;
    ULONG Reserved8;
    ULONG AtlThunkSListPtr32;
    PVOID Reserved9[45];
    BYTE Reserved10[96];
    PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
    BYTE Reserved11[128];
    PVOID Reserved12[1];
    ULONG SessionId;
} PEB, * PPEB;

typedef struct __TEB {
	PVOID Reserved1[12];
	PPEB  ProcessEnvironmentBlock;
	PVOID Reserved2[399];
	BYTE  Reserved3[1952];
	PVOID TlsSlots[64];
	BYTE  Reserved4[8];
	PVOID Reserved5[26];
	PVOID ReservedForOle;
	PVOID Reserved6[4];
	PVOID TlsExpansionSlots;
} TEB_, * PTEB_;

#ifdef _M_X64
unsigned long long __readgsqword_(unsigned long long offset);
#else
unsigned long __readfsdword_(unsigned long offset);
#endif

char* strcpy_(char* dest, const char* src);
size_t strlen_(char* src);
int strncmp_(char* a, char* b, size_t size);
int strcmp_(const char* s1, const char* s2);
char* strcat_(char* dest, const char* src);
void *memset_(void* a, int val, size_t size);
size_t mbstowcs_(wchar_t* dest, const char* src, size_t n);
size_t wcstombs_(char* dest, const wchar_t* src, size_t n);
int wcscmp_(const wchar_t* s1, const wchar_t* s2);
wchar_t* wcsstr_(const wchar_t* haystack, const wchar_t* needle);
wchar_t* wcscat_(wchar_t* dest, const wchar_t* src);
wchar_t* wcscpy_(wchar_t* dest, const wchar_t* src);
char** split_lines(const char* fileContent, int* lineCount);
void* my_GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
void* get_ntfunction(const char* func);
