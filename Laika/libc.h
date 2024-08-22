#pragma once
#include <Windows.h>
#include <stddef.h>

extern HANDLE _crt_heap;

char* strcpy_(char* dest, const char* src);
size_t strlen_(char* src);
int strncmp_(char* a, char* b, size_t size);
void *memset_(void* a, int val, size_t size);
size_t mbstowcs_(wchar_t* dest, const char* src, size_t n);
size_t wcstombs_(char* dest, const wchar_t* src, size_t n);
int wcscmp_(const wchar_t* s1, const wchar_t* s2);
wchar_t* wcsstr_(const wchar_t* haystack, const wchar_t* needle);
char** split_lines(const char* fileContent, int* lineCount);
void* my_GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
void* get_ntfunction(const char* func);
