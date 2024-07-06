#include "libc.h"
#include "resolve_apis.h"

size_t strlen_(char* src) {
	size_t s = 0;

	if (src == NULL)
		return 0;
	for (; src[s] != 0; s++);
	return s;
}

char* strcpy_(char* dest, const char* src) {
	size_t i = 0;

	for (; src[i] != 0; i++)
		dest[i] = src[i];
	dest[i] = 0;
	return dest;
}

void *memset_(void* a, int val, size_t size) {
	if (a == NULL)
		return NULL;
	for (size_t i = 0; i < size; i++)
		((char*)a)[i] = (char)val;
	return a;
}

int strncmp_(char* a, char* b, size_t size) {
	if (a == NULL || b == NULL)
		return -1;
	for (size_t i = 0; a[i] != 0 && b[i] != 0 && i < size; i++)
		if (a[i] != b[i])
			return a[i] - b[i];
	return 0;
}

size_t utf8_char_length(unsigned char c) {
    if ((c & 0x80) == 0) return 1;       // 1-byte character: 0xxxxxxx
    if ((c & 0xE0) == 0xC0) return 2;    // 2-byte character: 110xxxxx
    if ((c & 0xF0) == 0xE0) return 3;    // 3-byte character: 1110xxxx
    if ((c & 0xF8) == 0xF0) return 4;    // 4-byte character: 11110xxx
    return 0;                            // Invalid UTF-8
}

wchar_t utf8_to_wchar(const unsigned char* mbstr, size_t* length) {
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

size_t mbstowcs_(wchar_t* dest, const char* src, size_t n) {
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

size_t wchar_to_utf8(wchar_t wc, unsigned char* utf8) {
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

size_t wcstombs_(char* dest, const wchar_t* src, size_t n) {
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

int wcscmp_(const wchar_t* s1, const wchar_t* s2) {
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

wchar_t* wcsstr_(const wchar_t* haystack, const wchar_t* needle) {
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
		if (strcmp(functionName, lpProcName) == 0) {
			DWORD funcRVA = addrRVAs[ordinals[i]];
			void* funcPtr = (void*)((BYTE*)hModule + funcRVA);
			return funcPtr;
		}
	}
	return NULL;
}

void* get_ntfunction(const char* func) {
	PTEB tebPtr = GetTEB();

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
