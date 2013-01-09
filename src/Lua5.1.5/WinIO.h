#ifndef WinIO_h
#define WinIO_h

#include <stdio.h>

wchar_t* utf8_to_utf16(const char *s);
FILE* _utf8open(const char* filename, const char* mode);
FILE* _utf8reopen(const char* filename, const char* mode, FILE *file);
FILE* _utf8popen(const char* command, const char* mode);
int _utf8system(const char *command);
int _utf8remove(const char *filename);
int _utf8rename(const char *filename, const char *filename2);

#endif