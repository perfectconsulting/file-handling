/*
 * interface.cpp
 * Version 1.0.0							8th September 1999.
 */

#include "glodef.h"

#define N_FIL						32
#define INVALID_SET_FILE_POINTER	-1						

extern "C" __declspec(dllexport) void __stdcall FILInitialize(void);
extern "C" __declspec(dllexport) void __stdcall FILTerminate(void);
extern "C" __declspec(dllexport) short __stdcall FILOpen(const char *, const char *, short);
extern "C" __declspec(dllexport) short __stdcall FILClose(short);
extern "C" __declspec(dllexport) short __stdcall FILRead(short, uchar *, long);
extern "C" __declspec(dllexport) short __stdcall FILWrite(short, uchar *, long);

bool IsValid(short);
void FreeCache(short);

