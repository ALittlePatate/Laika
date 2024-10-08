#include "libc.h"
#include "resolve_apis.h"

extern LAIKA_NOOPT size_t wchar_to_utf8(wchar_t wc, unsigned char* utf8);

#ifdef _MSC_VER
#pragma optimize("", off)
#endif
LAIKA_NOOPT char* strncpy_(char* dest, const char* src, size_t n) {
    size_t i = 0;

    for (; i < n && src[i] != 0; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = 0;
    return dest;
}

LAIKA_NOOPT void* memset_(void* a, int val, size_t size) {
    if (a == NULL)
        return NULL;
    for (size_t i = 0; i < size; i++)
        ((char*)a)[i] = (char)val;
    return a;
}

LAIKA_NOOPT size_t wcstombs_(char* dest, const wchar_t* src, size_t n) {
    size_t count = 0;
    unsigned char utf8[4];

    while (*src != L'\0' && count < n) {
        size_t len = wchar_to_utf8(*src, utf8);
        if (len == 0 || count + len > n)
            return (size_t)-1;
        if (dest != NULL) {
            for (size_t i = 0; i < len; i++)
                dest[count++] = (char)utf8[i];
        }
        else
            count += len;
        src++;
    }
    if (dest != NULL && count < n)
        dest[count] = '\0';
    return count;
}

#ifdef _MSC_VER
#pragma optimize("", on)
#endif

LAIKA_NOOPT char* strcpy_(char* dest, const char* src) {
    size_t i = 0;

    for (; src[i] != 0; i++)
        dest[i] = src[i];
    dest[i] = 0;
    return dest;
}

LAIKA_NOOPT size_t strlen_(char* src) {
	size_t s = 0;

	if (src == NULL)
		return 0;
	for (; src[s] != 0; s++);
	return s;
}

LAIKA_NOOPT int strncmp_(char* a, char* b, size_t size) {
	if (a == NULL || b == NULL)
		return -1;
	for (size_t i = 0; a[i] != 0 && b[i] != 0 && i < size; i++)
		if (a[i] != b[i])
			return a[i] - b[i];
	return 0;
}

LAIKA_NOOPT size_t utf8_char_length(unsigned char c) {
    if ((c & 0x80) == 0) return 1;       // 1-byte character: 0xxxxxxx
    if ((c & 0xE0) == 0xC0) return 2;    // 2-byte character: 110xxxxx
    if ((c & 0xF0) == 0xE0) return 3;    // 3-byte character: 1110xxxx
    if ((c & 0xF8) == 0xF0) return 4;    // 4-byte character: 11110xxx
    return 0;                            // Invalid UTF-8
}

LAIKA_NOOPT wchar_t utf8_to_wchar(const unsigned char* mbstr, size_t* length) {
    wchar_t wc = 0;
    size_t len = utf8_char_length(mbstr[0]);
    *length = len;

    switch (len) {
    case 1:
        wc = mbstr[0];
        break;
    case 2:
        wc = ((mbstr[0] & 0x1F) << 6) | (mbstr[1] & 0x3F);
        break;
    case 3:
        wc = ((mbstr[0] & 0x0F) << 12) | ((mbstr[1] & 0x3F) << 6) | (mbstr[2] & 0x3F);
        break;
    case 4:
        wc = ((mbstr[0] & 0x07) << 18) | ((mbstr[1] & 0x3F) << 12) | ((mbstr[2] & 0x3F) << 6) | (mbstr[3] & 0x3F);
        break;
    default:
        *length = 0; // Invalid UTF-8
        break;
    }
    return wc;
}

LAIKA_NOOPT size_t mbstowcs_(wchar_t* dest, const char* src, size_t n) {
    size_t count = 0;
    const unsigned char* mbstr = (const unsigned char*)src;

    while (*mbstr != '\0' && count < n) {
        size_t len = 0;
        wchar_t wc = utf8_to_wchar(mbstr, &len);
        if (len == 0)
            return (size_t)-1;
        if (dest != NULL)
            dest[count] = wc;
        mbstr += len;
        count++;
    }
    if (dest != NULL && count < n)
        dest[count] = L'\0';
    return count;
}

LAIKA_NOOPT size_t wchar_to_utf8(wchar_t wc, unsigned char* utf8) {
    if (wc <= 0x7F) {
        utf8[0] = (unsigned char)wc;
        return 1;
    }
    else if (wc <= 0x7FF) {
        utf8[0] = 0xC0 | ((wc >> 6) & 0x1F);
        utf8[1] = 0x80 | (wc & 0x3F);
        return 2;
    }
    else if (wc <= 0xFFFF) {
        utf8[0] = 0xE0 | ((wc >> 12) & 0x0F);
        utf8[1] = 0x80 | ((wc >> 6) & 0x3F);
        utf8[2] = 0x80 | (wc & 0x3F);
        return 3;
    }
    else if (wc <= 0x10FFFF) {
        utf8[0] = 0xF0 | ((wc >> 18) & 0x07);
        utf8[1] = 0x80 | ((wc >> 12) & 0x3F);
        utf8[2] = 0x80 | ((wc >> 6) & 0x3F);
        utf8[3] = 0x80 | (wc & 0x3F);
        return 4;
    }
    return 0; // Invalid wide character
}

LAIKA_NOOPT int wcscmp_(const wchar_t* s1, const wchar_t* s2) {
    while (*s1 != L'\0' && *s2 != L'\0') {
        if (*s1 != *s2)
            return (*s1 < *s2) ? -1 : 1;
        s1++;
        s2++;
    }
    if (*s1 == L'\0' && *s2 == L'\0')
        return 0;
    else if (*s1 == L'\0')
        return -1;
    else
        return 1;
}

LAIKA_NOOPT wchar_t* wcsstr_(const wchar_t* haystack, const wchar_t* needle) {
    if (!*needle)
        return (wchar_t*)haystack;
    while (*haystack) {
        const wchar_t* h = haystack;
        const wchar_t* n = needle;

        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        if (!*n)
            return (wchar_t*)haystack;
        haystack++;
    }
    return NULL;
}

LAIKA_NOOPT wchar_t* wcscat_(wchar_t* dest, const wchar_t* src) {
    wchar_t* d = dest;
    while (*d != L'\0') {
        d++;
    }
    const wchar_t* s = src;
    while (*s != L'\0') {
        *d = *s;
        d++;
        s++;
    }
    *d = L'\0';
    return dest;
}

LAIKA_NOOPT wchar_t* wcscpy_(wchar_t* dest, const wchar_t* src) {
    wchar_t* d = dest;
    const wchar_t* s = src;
    while ((*d++ = *s++) != L'\0') {
    }

    return dest;
}

LAIKA_NOOPT int strcmp_(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (*(unsigned char*)s1 - *(unsigned char*)s2);
}

LAIKA_NOOPT char* strcat_(char* dest, const char* src) {
    char* d = dest;

    while (*d != '\0') {
        d++;
    }
    while (*src != '\0') {
        *d++ = *src++;
    }
    *d = '\0';
    return dest;
}

char** split_lines(const char* fileContent, int* lineCount) {
    int lines = 0;
    const char* start = fileContent;
    const char* end = fileContent;
    char** result = NULL;

    while (*end != '\0') {
        if (*end == '\n') {
            int lineLength = end - start + 1;

            if (result == NULL) {
                result = Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, sizeof(char*) * (lines + 1));
            }
            else {
                result = Api.Heaprealloc_(_crt_heap, HEAP_ZERO_MEMORY, result, sizeof(char*) * (lines + 1));
            }

            result[lines] = Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, lineLength + 1);
            strncpy_(result[lines], start, lineLength);
            result[lines][lineLength] = '\0';
            lines++;

            if (*end == '\r' && *(end + 1) == '\n') {
                end++;
            }
            else if (*end == '\n' && *(end + 1) == '\r') {
                end++;
            }

            start = end + 1;
        }
        end++;
    }
    if (start != end) {
        int lineLength = end - start;
        if (result == NULL) {
            result = Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, sizeof(char*) * (lines + 1));
        }
        else {
            result = Api.Heaprealloc_(_crt_heap, HEAP_ZERO_MEMORY, result, sizeof(char*) * (lines + 1));
        }
        result[lines] = Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, lineLength + 1);
        strncpy_(result[lines], start, lineLength);
        result[lines][lineLength] = '\0';  // Null-terminate the line
        lines++;
    }
    *lineCount = lines;
    return result;
}

void* my_GetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
	if (hModule == NULL) {
		return NULL;
	}

	IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)hModule;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		return NULL;
	}

	IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)((BYTE*)hModule + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		return NULL;
	}

	IMAGE_EXPORT_DIRECTORY* exportDir = (IMAGE_EXPORT_DIRECTORY*)((BYTE*)hModule + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	DWORD* nameRVAs = (DWORD*)((BYTE*)hModule + exportDir->AddressOfNames);
	DWORD* addrRVAs = (DWORD*)((BYTE*)hModule + exportDir->AddressOfFunctions);
	WORD* ordinals = (WORD*)((BYTE*)hModule + exportDir->AddressOfNameOrdinals);

	for (DWORD i = 0; i < exportDir->NumberOfNames; i++) {
		const char* functionName = (const char*)((BYTE*)hModule + nameRVAs[i]);
		if (strcmp_(functionName, lpProcName) == 0) {
			DWORD funcRVA = addrRVAs[ordinals[i]];
			void* funcPtr = (void*)((BYTE*)hModule + funcRVA);
			return funcPtr;
		}
	}
	return NULL;
}

#ifdef _M_X64
unsigned long long __readgsqword_(unsigned long long offset) {
#ifndef _MSC_VER
    unsigned long long value;
    __asm__ __volatile__(
        "movq %%gs:%1, %0"
        : "=r" (value)
        : "m" (*(unsigned long long*)(offset))
    );
    return value;
#else
	return __readgsqword(offset);
#endif
}
#else
unsigned long __readfsdword_(unsigned long offset) {
#ifndef _MSC_VER
    unsigned long value;
    __asm__ __volatile__(
        "movl %%fs:%1, %0"
        : "=r" (value)
        : "m" (*(unsigned long*)(offset))
    );
    return value;
#else
    return __readfsdword(offset);
#endif
}
#endif

void* get_ntfunction(const char* func) {
    PTEB_ tebPtr = GetTEB();

	PPEB_LDR_DATA ldrData = tebPtr->ProcessEnvironmentBlock->Ldr;
	PLIST_ENTRY moduleList = &(ldrData->InMemoryOrderModuleList);
	HMODULE hntdll = 0;
	for (PLIST_ENTRY entry = moduleList->Flink; entry != moduleList; entry = entry->Flink) {
		LDR_DATA_TABLE_ENTRY* module = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
		if (wcsstr_(module->FullDllName.Buffer, L"ntdll.dll") != NULL) {
			hntdll = (HMODULE)(module->DllBase);
			break;
		}
	}
	return my_GetProcAddress(hntdll, func);
}
