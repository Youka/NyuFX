#ifndef WinIO_h
#define WinIO_h

#ifdef _WIN32

#include <stdio.h>

FILE* _utf8fopen(const char* filename, const char* mode);
FILE* _utf8freopen(const char* filename, const char* mode, FILE *file);
FILE* _utf8popen(const char* command, const char* mode);
int _utf8system(const char *command);
int _utf8remove(const char *filename);
int _utf8rename(const char *filename, const char *filename2);

#else

#define _utf8fopen fopen
#define _utf8freopen freopen
#define _utf8popen popen
#define _utf8system system
#define _utf8remove remove
#define _utf8rename rename

#endif

#endif