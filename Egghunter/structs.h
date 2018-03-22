#ifndef __STRUCTS_H
#define __STRUCTS_H

#include <Windows.h>

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

#endif // __STRUCTS_H