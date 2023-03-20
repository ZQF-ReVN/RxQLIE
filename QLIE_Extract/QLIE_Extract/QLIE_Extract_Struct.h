#pragma once
#include <Windows.h>

typedef struct TPack_InitTable
{
	PDWORD            pTable;
}*LPTPack_InitTable;

typedef struct TPack_Handle
{
	PDWORD            pTable;
	HANDLE            hFile;
	PWCHAR            lpPackName;
}*LPTPack_Handle;

typedef struct TPack_ResEntry
{
	DWORD             dwOffset;
	DWORD             dwUn0;
	DWORD             dwCompSize;
	DWORD             dwDempSize;
	DWORD             dwCompFlag;
	DWORD             dwEncFlag;
	DWORD             dwDecKey;
}*LPTPack_ResEntry;

typedef struct TPack_ResIndex
{
	PDWORD            pTable;
	LPTPack_InitTable pTPack_InitTable;
	LPTPack_Handle    pPackHandle;
	LPTPack_ResEntry* pResInfoEntry;
	PWCHAR*           pResNameEntry;
	DWORD             dwResCount;
}*LPTPack_ResIndex;

typedef struct TPack_PackEntry
{
	PDWORD            pTable;
	DWORD             dwUn0;
	PDWORD            dwUn1;
	LPTPack_ResIndex  pResIndex;
	WCHAR*            lpHashFileName;
	WCHAR*            lpPackFileName;
}*LPTPack_PackEntry;

typedef struct TPack_Buffer
{
	PDWORD            pTable;
	PDWORD            pBuffer;
	DWORD             dwSize;
}*LPTPack_Buffer;


typedef VOID(CALLBACK* fnOnPackEntryHandle)(LPTPack_PackEntry pPackEntry, LPTPack_Buffer pKeyFile, PCWCHAR lpPackName);