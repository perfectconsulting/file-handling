/*
 * interface.cpp
 * Version 1.0.0							8th September 1999.
 */

#include "stdio.h"
#include <windows.h>
#include "index.h"
#include "interface.h"

struct _FCB
{
	HANDLE hdlFile;
	ushort nSize;
	CIndex *lpCache;
};

_FCB * lpFCB[N_FIL];

void __stdcall FILInitialize(void)
{
	for(short i = 1; i < N_FIL; i++)
		lpFCB[i] = NULL;
}

void __stdcall FILTerminate(void)
{
	for(short i = 1; i < N_FIL; i++)
		FILClose(i);
}

short __stdcall FILOpen(const char * lpszPath, const char *lpszMode, short nSize)
{
	HANDLE hdlFile;
	DWORD dwMode;

	for(short i = 1; i < N_FIL; i++)
		if(lpFCB[i] == NULL)
			break;

	if(i == N_FIL)
		return 0;

	switch(*lpszMode)
	{
		case 'r':
			dwMode = GENERIC_READ;
			break;

		case 'w':
			dwMode = GENERIC_WRITE;
			break;

		case 'x':
			dwMode = GENERIC_READ | GENERIC_WRITE;
			break;

		default:
			return 0;
	}

	hdlFile = CreateFile(lpszPath, dwMode, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);

	if(hdlFile == INVALID_HANDLE_VALUE)
		return 0;
	
	lpFCB[i] = new _FCB;
	lpFCB[i]->hdlFile = hdlFile;
	lpFCB[i]->nSize = nSize;
	lpFCB[i]->lpCache = new CIndex;

	return i;
}

short __stdcall FILClose(short n)
{
	if(!IsValid(n))
		return 0;

	CloseHandle(lpFCB[n]->hdlFile);
	FreeCache(n);

	delete lpFCB[n]->lpCache;

	delete lpFCB[n];
	lpFCB[n] = NULL;

	return 1;
}

short __stdcall FILRead(short n, uchar * lpBuffer, long lOffset)
{
	uchar *lpData;
	DWORD dwBytesRead;

	if(!IsValid(n))
		return 0;

	if(lpFCB[n]->lpCache->Find(lOffset, (ulong*)&lpData))
	{
		memcpy(lpBuffer, lpData, lpFCB[n]->nSize);
		return 1;
	}

	if(SetFilePointer (lpFCB[n]->hdlFile, lOffset * lpFCB[n]->nSize, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return 0;

	if(!ReadFile(lpFCB[n]->hdlFile, lpBuffer, lpFCB[n]->nSize, &dwBytesRead, NULL))
		return 0;

	if(dwBytesRead != lpFCB[n]->nSize) 
		return 0;

	lpData = new uchar[lpFCB[n]->nSize];
	memcpy(lpData, lpBuffer, lpFCB[n]->nSize);

	lpFCB[n]->lpCache->Insert(lOffset, (ulong)lpData);

	return 1;
}

short __stdcall FILWrite(short n, uchar * lpBuffer, long lOffset)
{
	uchar *lpData;
	DWORD dwBytesRead;

	if(!IsValid(n))
		return 0;

	if(SetFilePointer (lpFCB[n]->hdlFile, lOffset * lpFCB[n]->nSize, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return 0;

	if(!WriteFile(lpFCB[n]->hdlFile, lpBuffer, lpFCB[n]->nSize, &dwBytesRead, NULL))
		return 0;

	if(dwBytesRead != lpFCB[n]->nSize) 
		return 0;

	if(lpFCB[n]->lpCache->Find(lOffset, (ulong*)&lpData))
		memcpy(lpData, lpBuffer, lpFCB[n]->nSize);

	return 1;
}

bool IsValid(short n)
{
	if( n > N_FIL)
		return false;

	if(lpFCB[n] == NULL)
		return false;

	return true;
}

void FreeCache(short n)
{
	uchar *lpData;

	lpFCB[n]->lpCache->Reset();

	while(lpFCB[n]->lpCache->Ascending((ulong*)&lpData))
		delete lpData;
}
