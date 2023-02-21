#pragma once
#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include "resolve_apis.h"

#define Sleep_TIME 30

char* CAESAR(char* in);
char* CAESAR_DECRYPT(char* in);
LPCWSTR ConvertCharToWChar(const char* str);
void Sleep_(int time_to_wait);
void Message();