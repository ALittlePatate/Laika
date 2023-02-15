#include "file_explorer.h"

int delete_file(char* path) {
	return Api.remove(path);
}

int delete_dir(char* path) {
	return Api.rmdir(path);
}

int get_object_info(char* path, struct stat* fileinfo) {
	return Api.stat(path, fileinfo);
}

char** get_file_list(const char* dirPath, int* numFiles) {
    WIN32_FIND_DATA findData;
    HANDLE hFind;

    WCHAR searchPath[MAX_PATH];
    Api.mbstowcs(searchPath, dirPath, MAX_PATH);

    wcscat(searchPath, L"\\*.*");

    hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    // Allocate a dynamic array to store the file names
    int maxFiles = 100;
    char** fileList = (char**)Api.malloc(maxFiles * sizeof(char*));
    int numFound = 0;

    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue; // Ignore directories
        }

        // Convert the file name to a char string
        WCHAR wFileName[MAX_PATH];
        wcscpy(wFileName, findData.cFileName);
        char fileName[MAX_PATH];
        Api.wcstombs(fileName, wFileName, MAX_PATH);

        // Add the file name to the array
        if (numFound >= maxFiles) {
            maxFiles *= 2;
            fileList = (char**)Api.realloc(fileList, maxFiles * sizeof(char*));
        }
        fileList[numFound] = (char*)Api.malloc(strlen(fileName) + 1);
        Api.strcpy(fileList[numFound], fileName);
        numFound++;
    } while (FindNextFile(hFind, &findData) != 0);

    Api.FindClose(hFind);

    // Resize the array to the actual number of files found
    fileList = (char**)Api.realloc(fileList, numFound * sizeof(char*));

    // Set the numFiles parameter to the number of files found
    *numFiles = numFound;

    return fileList;

    /*
    const char* dirPath = "C:\\Users\\UserName\\Documents\\ExampleDirectory"; // Replace with the path to the directory you want to list

    int numFiles;
    char** fileList = listFiles(dirPath, &numFiles);

    if (fileList == NULL) {
        printf("Error listing files\n");
        return 1;
    }

    // Print the list of files
    for (int i = 0; i < numFiles; i++) {
        printf("%s\n", fileList[i]);
        free(fileList[i]);
    }

    // Free the array and its contents
    free(fileList);

    return 0;
    */
}

void download_file() {

}

void download_folder() {

}

void upload_file() {

}