#include "shellcode.h"

extern API Api;

HANDLE FindProcessByArch(const wchar_t* arch)
{
    HANDLE hSnapshot;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system
    hSnapshot = Api.CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    // Iterate through the processes in the system
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Api.Process32FirstW(hSnapshot, &pe32)) {
        Api.CloseHandle(hSnapshot);
        return NULL;
    }
    do {
        HANDLE hProcess;
        BOOL isWow64;

        hProcess = Api.OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        if (hProcess == NULL) {
            continue;
        }

        // Determine the process architecture
        if (!Api.IsWow64Process(hProcess, &isWow64)) {
            // Failed to determine the process architecture, so skip to the next process
            Api.CloseHandle(hProcess);
            hProcess = NULL;
            continue;
        }

        // Check if the process matches the provided architecture
        if ((Api.wcscmp(arch, L"x86") == 0 && !isWow64) || (Api.wcscmp(arch, L"x64") == 0 && isWow64)) {
            if (hProcess != NULL) {
                Api.CloseHandle(hSnapshot);
                return hProcess;
            }
        }
    } while (Api.Process32NextW(hSnapshot, &pe32));

    // No matching process found
    Api.CloseHandle(hSnapshot);
    return NULL;
}