#pragma once
#include <vector>

#include "QLIE_Extract_Struct.h"

namespace QLIE_Extract
{
	BOOL ExtractRes();
	BOOL PrintResInfo();
	BOOL PrintPackInfo();
	BOOL LogInfo(LPCWSTR lpLogFileName);
	//std::vector<LPTPack_PackEntry>& GetPackEntryList();
	//LPTPack_Buffer TObject_Free(LPTPack_Buffer pTBuffer);
	//LPTPack_Buffer TFilePack_Get(LPTPack_ResIndex pResIndex, SIZE_T nSequence);
	BOOL Init(DWORD RawfnTHashFilePack_Create, DWORD RawfnTFilePack_Get, DWORD RawfnTObject_Free);
}
