#include "QLIE_Extract.h"
#include "../TDA/ConsoleX.h"
#include "../TDA/MemX.h"
#include "../TDA/FileX.h"

#include <cstdio>
#include <corecrt_share.h>

namespace QLIE_Extract
{
	static std::vector<LPTPack_PackEntry> g_vecPackEntry;

	//************************
	//* THashFilePack_Create *
	//************************
	static DWORD g_fnTHashFilePack_CreateRet = NULL;
	__declspec(naked)
		LPTPack_PackEntry __stdcall RawTHashFilePack_Create(LPTPack_InitTable pFileList, CHAR cUn0, LPTPack_Buffer pKeyFile, CHAR cUn1, CHAR cUn2, PCWCHAR lpPackName)
	{
		__asm
		{
			pop edi;  //Save Ret Addr
			pop eax;  //Pop Para 1
			pop edx;  //Pop Para 2
			pop ecx;  //Pop Para 3
			push edi; //Push Ret Addr

			push ebp;
			mov ebp, esp;
			add esp, 0xFFFFFFEC;
			jmp g_fnTHashFilePack_CreateRet;
		}
	}

	LPTPack_PackEntry __stdcall NewTHashFilePack_Create(LPTPack_InitTable pFileList, CHAR cUn0, LPTPack_Buffer pKeyFile, CHAR cUn1, CHAR cUn2, PCWCHAR lpPackName)
	{
		LPTPack_PackEntry packEntry = RawTHashFilePack_Create(pFileList, cUn0, pKeyFile, cUn1, cUn2, lpPackName);
		g_vecPackEntry.emplace_back(packEntry);
		return packEntry;
	}

	__declspec(naked)
		VOID __stdcall THashFilePack_Create_CallCVT()
	{
		__asm
		{
			pop edi;   //Save Ret Addr 
			push ecx;  //Push Para3
			push edx;  //Push Para2
			push eax;  //Push Para1
			push edi;  //Push Ret Addr

			jmp NewTHashFilePack_Create
		}
	}


	//************************
	//*  THashFilePack_Get  *
	//************************
	static DWORD g_fnTFilePack_GetRaw = NULL;
	__declspec(naked)
		LPTPack_Buffer __fastcall RawTFilePack_Get(LPTPack_ResIndex pResIndex, SIZE_T nSequence)
	{
		__asm
		{
			mov eax, ecx;
			mov cl, 0x1;
			jmp g_fnTFilePack_GetRaw;
		}
	}


	//************************
	//*     TObject_Free     *
	//************************
	static DWORD g_fnTObject_FreeRaw = NULL;
	__declspec(naked)
		LPTPack_Buffer __fastcall RawTObject_Free(LPTPack_Buffer pTBuffer)
	{
		__asm
		{
			mov eax, ecx;
			jmp g_fnTObject_FreeRaw;
		}
	}

	//*******************
	//* Export Function *
	//*******************

	std::vector<LPTPack_PackEntry>& GetPackEntryList()
	{
		return g_vecPackEntry;
	}

	LPTPack_Buffer TObject_Free(LPTPack_Buffer pTBuffer)
	{
		return RawTObject_Free(pTBuffer);
	}

	LPTPack_Buffer TFilePack_Get(LPTPack_ResIndex pResIndex, SIZE_T nSequence)
	{
		return RawTFilePack_Get(pResIndex, nSequence);
	}

	BOOL Init(DWORD RawfnTHashFilePack_Create, DWORD RawfnTFilePack_Get, DWORD RawfnTObject_Free)
	{
		g_fnTObject_FreeRaw = RawfnTObject_Free;
		g_fnTFilePack_GetRaw = RawfnTFilePack_Get;
		g_fnTHashFilePack_CreateRet = RawfnTHashFilePack_Create + 0x6;

		return TDA::MemX::WriteHookCode_RET(RawfnTHashFilePack_Create, g_fnTHashFilePack_CreateRet, (DWORD)THashFilePack_Create_CallCVT);
	}

	BOOL PrintPackInfo()
	{
		size_t count = 0;

		TDA::ConsoleX::PutConsoleW(L"PackEntry:\n");
		TDA::ConsoleX::PutConsoleW(L"=================================\n");
		for (LPTPack_PackEntry packEntry : GetPackEntryList())
		{
			TDA::ConsoleX::PutConsoleW(L"Sequence:%d , Address:0x%X , ResCount:%d\n", count++, (DWORD)packEntry, packEntry->pResIndex->dwResCount);
			TDA::ConsoleX::PutConsoleW(L"PackPath:%s , HashPath:%s\n\n", packEntry->lpPackFileName, packEntry->lpHashFileName);
		}
		TDA::ConsoleX::PutConsoleW(L"=================================\n");

		return TRUE;
	}

	BOOL PrintResInfo()
	{
		TDA::ConsoleX::PutConsoleW(L"Input Pack Sequence:");

		size_t sequence = 0;
		scanf_s("%d", &sequence);

		LPTPack_ResIndex resIndex = GetPackEntryList()[sequence]->pResIndex;
		for (size_t iteRes = 0; iteRes < resIndex->dwResCount; iteRes++)
		{
			TDA::ConsoleX::PutConsoleW(L"%s\n", resIndex->pResNameEntry[iteRes]);
		}
		TDA::ConsoleX::PutConsoleW(L"\n");

		return TRUE;
	}

	BOOL LogInfo(LPCWSTR lpLogFileName)
	{
		FILE* fp = _wfsopen(lpLogFileName, L"w, ccs=UTF-16LE", _SH_DENYNO);
		if (!fp) { MessageBoxW(NULL, L"CreateListFile Failed!!", NULL, NULL); return FALSE; }

		size_t count = 0;
		for (LPTPack_PackEntry packEntry : GetPackEntryList())
		{
			LPTPack_ResIndex resIndex = packEntry->pResIndex;

			fwprintf_s(fp, L"Sequence:%d , Address:0x%X , ResCount:%d\n", count++, (DWORD)packEntry, resIndex->dwResCount);
			fwprintf_s(fp, L"PackPath:%s , HashPath:%s\n", packEntry->lpPackFileName, packEntry->lpHashFileName);
			fwprintf_s(fp, L"=========================================================\n\n");
			for (size_t iteRes = 0; iteRes < resIndex->dwResCount; iteRes++)
			{
				fwprintf_s(fp, L"%s\n", resIndex->pResNameEntry[iteRes]);
			}
			fwprintf_s(fp, L"\n=========================================================\n\n\n");

			fflush(fp);
		}

		return TRUE;
	}

	BOOL ExtractRes()
	{
		TDA::ConsoleX::PutConsoleW(L"Input Pack Sequence:");

		size_t sequence = 0;
		scanf_s("%d", &sequence);

		LPTPack_ResIndex resIndex = GetPackEntryList()[sequence]->pResIndex;


		std::wstring fileRelativePath;
		std::wstring fileRelativeFolder;
		LPTPack_Buffer pTBuffer = NULL;
		WCHAR* lpResName = NULL;
		for (size_t iteRes = 0; iteRes < resIndex->dwResCount; iteRes++)
		{
			pTBuffer = QLIE_Extract::TFilePack_Get(resIndex, iteRes);
			lpResName = resIndex->pResNameEntry[iteRes];

			if (TDA::FileX::SaveFileViaPath(lpResName, pTBuffer->pBuffer, pTBuffer->dwSize))
			{
				TDA::ConsoleX::PutConsoleW(L"Extract:%s\n", lpResName);
			}
			
			QLIE_Extract::TObject_Free(pTBuffer);
		}



		return TRUE;
	}

}
