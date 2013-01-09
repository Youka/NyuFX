#ifndef WinIO_h
#define WinIO_h

#include <stdio.h>
#include <windows.h>

static wchar_t* utf8_to_utf16(const char *s){
	// Conversion data
	int ws_len;
	wchar_t *ws;
	// Get utf16 length from utf8
	ws_len = MultiByteToWideChar(CP_UTF8, 0, s, strlen(s)+1, 0, 0);
	// Allocate utf16 memory
	ws = (wchar_t*)malloc(ws_len * sizeof(wchar_t));
	// Convert utf8 to utf16
	MultiByteToWideChar(CP_UTF8, 0, s, strlen(s)+1, ws, ws_len);
	// Return utf16 buffer
	return ws;
}

static FILE* _utf8open(const char* filename, const char* mode){
	// Wide-char buffers
	wchar_t *wfilename;
	wchar_t wmode[4];
	FILE *f;
	// Convert filename to wide-char
	wfilename = utf8_to_utf16(filename);
	// Convert mode to wide-char
	wmode[sizeof(wmode)-1] = L'\0';
	mbstowcs(wmode, mode, 3);
	// Create file handle, clear filename buffer and return file
	f = _wfopen(wfilename, wmode);
	free(wfilename);
	return f;
}

static FILE* _utf8reopen(const char* filename, const char* mode, FILE *file){
	// Wide-char buffers
	wchar_t *wfilename;
	wchar_t wmode[4];
	FILE *f;
	// Convert filename to wide-char
	wfilename = utf8_to_utf16(filename);
	// Convert mode to wide-char
	wmode[sizeof(wmode)-1] = L'\0';
	mbstowcs(wmode, mode, 3);
	// Create file handle, clear filename buffer and return file
	f = _wfreopen(wfilename, wmode, file);
	free(wfilename);
	return f;
}

static int _utf8system(const char *command){
	wchar_t *wcommand = utf8_to_utf16(command);
	int result = _wsystem(wcommand);
	free(wcommand);
	return result;
}

static int _utf8remove(const char *filename){
	wchar_t *wfilename = utf8_to_utf16(filename);
	int result = _wremove(wfilename);
	free(wfilename);
	return result;
}

static int _utf8rename(const char *filename, const char *filename2){
	wchar_t *wfilename = utf8_to_utf16(filename);
	wchar_t *wfilename2 = utf8_to_utf16(filename2);
	int result = _wrename(wfilename, wfilename2);
	free(wfilename);
	free(wfilename2);
	return result;
}

#endif