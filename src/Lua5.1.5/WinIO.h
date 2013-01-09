#ifndef WinIO_h
#define WinIO_h

#include <stdio.h>
#include <windows.h>

static FILE* _utf8open(const char* filename, const char* mode){
	// Data
	int w_len;
	wchar_t *wfilename;
	wchar_t wmode[4];
	FILE *f;
	// Convert filename to wide-char
	w_len = MultiByteToWideChar(CP_UTF8, 0, filename, strlen(filename)+1, 0, 0);
	wfilename = (wchar_t*)malloc(w_len * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, filename, strlen(filename)+1, wfilename, w_len);
	// Convert mode to wide-char
	wmode[sizeof(wmode)-1] = L'\0';
	mbstowcs(wmode, mode, 3);
	// Create file handle, clear filename buffer and return file
	f = _wfopen(wfilename, L"r");
	free(wfilename);
	return f;
}

#endif