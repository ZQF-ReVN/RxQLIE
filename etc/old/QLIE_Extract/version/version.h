#pragma once
#include <Windows.h>

#pragma comment(linker, "/EXPORT:GetFileVersionInfoA=_AheadLib_GetFileVersionInfoA,@1")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoByHandle=_AheadLib_GetFileVersionInfoByHandle,@2")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExA=_AheadLib_GetFileVersionInfoExA,@3")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExW=_AheadLib_GetFileVersionInfoExW,@4")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeA=_AheadLib_GetFileVersionInfoSizeA,@5")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExA=_AheadLib_GetFileVersionInfoSizeExA,@6")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExW=_AheadLib_GetFileVersionInfoSizeExW,@7")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeW=_AheadLib_GetFileVersionInfoSizeW,@8")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoW=_AheadLib_GetFileVersionInfoW,@9")
#pragma comment(linker, "/EXPORT:VerFindFileA=_AheadLib_VerFindFileA,@10")
#pragma comment(linker, "/EXPORT:VerFindFileW=_AheadLib_VerFindFileW,@11")
#pragma comment(linker, "/EXPORT:VerInstallFileA=_AheadLib_VerInstallFileA,@12")
#pragma comment(linker, "/EXPORT:VerInstallFileW=_AheadLib_VerInstallFileW,@13")
#pragma comment(linker, "/EXPORT:VerLanguageNameA=_AheadLib_VerLanguageNameA,@14")
#pragma comment(linker, "/EXPORT:VerLanguageNameW=_AheadLib_VerLanguageNameW,@15")
#pragma comment(linker, "/EXPORT:VerQueryValueA=_AheadLib_VerQueryValueA,@16")
#pragma comment(linker, "/EXPORT:VerQueryValueW=_AheadLib_VerQueryValueW,@17")


PVOID pfnAheadLib_GetFileVersionInfoA;
PVOID pfnAheadLib_GetFileVersionInfoByHandle;
PVOID pfnAheadLib_GetFileVersionInfoExA;
PVOID pfnAheadLib_GetFileVersionInfoExW;
PVOID pfnAheadLib_GetFileVersionInfoSizeA;
PVOID pfnAheadLib_GetFileVersionInfoSizeExA;
PVOID pfnAheadLib_GetFileVersionInfoSizeExW;
PVOID pfnAheadLib_GetFileVersionInfoSizeW;
PVOID pfnAheadLib_GetFileVersionInfoW;
PVOID pfnAheadLib_VerFindFileA;
PVOID pfnAheadLib_VerFindFileW;
PVOID pfnAheadLib_VerInstallFileA;
PVOID pfnAheadLib_VerInstallFileW;
PVOID pfnAheadLib_VerLanguageNameA;
PVOID pfnAheadLib_VerLanguageNameW;
PVOID pfnAheadLib_VerQueryValueA;
PVOID pfnAheadLib_VerQueryValueW;


HMODULE g_hmRealDll = NULL;

FARPROC GetAddress(PCSTR pszProcName)
{
	FARPROC pFunc = GetProcAddress(g_hmRealDll, pszProcName);
	if (pFunc != NULL) return pFunc;

	MessageBoxW(NULL, L"GetProcAddress Failed!", L"Proxy Dll", MB_OK);
	ExitProcess(0);

	return NULL;
}

VOID ProxyDll()
{
	WCHAR folder[MAX_PATH];
	GetSystemDirectoryW(folder, MAX_PATH);
	lstrcatW(folder, L"\\version.dll");

	g_hmRealDll = LoadLibraryW(folder);

	if (g_hmRealDll == NULL)
	{
		MessageBoxW(NULL, L"Load Real Dll Failed!", L"Proxy Dll", MB_OK);
		ExitProcess(0);
		return;
	}

	pfnAheadLib_GetFileVersionInfoA = GetAddress("GetFileVersionInfoA");
	pfnAheadLib_GetFileVersionInfoByHandle = GetAddress("GetFileVersionInfoByHandle");
	pfnAheadLib_GetFileVersionInfoExA = GetAddress("GetFileVersionInfoExA");
	pfnAheadLib_GetFileVersionInfoExW = GetAddress("GetFileVersionInfoExW");
	pfnAheadLib_GetFileVersionInfoSizeA = GetAddress("GetFileVersionInfoSizeA");
	pfnAheadLib_GetFileVersionInfoSizeExA = GetAddress("GetFileVersionInfoSizeExA");
	pfnAheadLib_GetFileVersionInfoSizeExW = GetAddress("GetFileVersionInfoSizeExW");
	pfnAheadLib_GetFileVersionInfoSizeW = GetAddress("GetFileVersionInfoSizeW");
	pfnAheadLib_GetFileVersionInfoW = GetAddress("GetFileVersionInfoW");
	pfnAheadLib_VerFindFileA = GetAddress("VerFindFileA");
	pfnAheadLib_VerFindFileW = GetAddress("VerFindFileW");
	pfnAheadLib_VerInstallFileA = GetAddress("VerInstallFileA");
	pfnAheadLib_VerInstallFileW = GetAddress("VerInstallFileW");
	pfnAheadLib_VerLanguageNameA = GetAddress("VerLanguageNameA");
	pfnAheadLib_VerLanguageNameW = GetAddress("VerLanguageNameW");
	pfnAheadLib_VerQueryValueA = GetAddress("VerQueryValueA");
	pfnAheadLib_VerQueryValueW = GetAddress("VerQueryValueW");

}

extern "C" __declspec(naked) void __cdecl AheadLib_GetFileVersionInfoA(void)
{
	__asm jmp pfnAheadLib_GetFileVersionInfoA;
}

extern "C" __declspec(naked) void __cdecl AheadLib_GetFileVersionInfoByHandle(void)
{
	__asm jmp pfnAheadLib_GetFileVersionInfoByHandle;
}

extern "C" __declspec(naked) void __cdecl AheadLib_GetFileVersionInfoExA(void)
{
	__asm jmp pfnAheadLib_GetFileVersionInfoExA;
}

extern "C" __declspec(naked) void __cdecl AheadLib_GetFileVersionInfoExW(void)
{
	__asm jmp pfnAheadLib_GetFileVersionInfoExW;
}

extern "C" __declspec(naked) void __cdecl AheadLib_GetFileVersionInfoSizeA(void)
{
	__asm jmp pfnAheadLib_GetFileVersionInfoSizeA;
}

extern "C" __declspec(naked) void __cdecl AheadLib_GetFileVersionInfoSizeExA(void)
{
	__asm jmp pfnAheadLib_GetFileVersionInfoSizeExA;
}

extern "C" __declspec(naked) void __cdecl AheadLib_GetFileVersionInfoSizeExW(void)
{
	__asm jmp pfnAheadLib_GetFileVersionInfoSizeExW;
}

extern "C" __declspec(naked) void __cdecl AheadLib_GetFileVersionInfoSizeW(void)
{
	__asm jmp pfnAheadLib_GetFileVersionInfoSizeW;
}

extern "C" __declspec(naked) void __cdecl AheadLib_GetFileVersionInfoW(void)
{
	__asm jmp pfnAheadLib_GetFileVersionInfoW;
}

extern "C" __declspec(naked) void __cdecl AheadLib_VerFindFileA(void)
{
	__asm jmp pfnAheadLib_VerFindFileA;
}

extern "C" __declspec(naked) void __cdecl AheadLib_VerFindFileW(void)
{
	__asm jmp pfnAheadLib_VerFindFileW;
}

extern "C" __declspec(naked) void __cdecl AheadLib_VerInstallFileA(void)
{
	__asm jmp pfnAheadLib_VerInstallFileA;
}

extern "C" __declspec(naked) void __cdecl AheadLib_VerInstallFileW(void)
{
	__asm jmp pfnAheadLib_VerInstallFileW;
}

extern "C" __declspec(naked) void __cdecl AheadLib_VerLanguageNameA(void)
{
	__asm jmp pfnAheadLib_VerLanguageNameA;
}

extern "C" __declspec(naked) void __cdecl AheadLib_VerLanguageNameW(void)
{
	__asm jmp pfnAheadLib_VerLanguageNameW;
}

extern "C" __declspec(naked) void __cdecl AheadLib_VerQueryValueA(void)
{
	__asm jmp pfnAheadLib_VerQueryValueA;
}

extern "C" __declspec(naked) void __cdecl AheadLib_VerQueryValueW(void)
{
	__asm jmp pfnAheadLib_VerQueryValueW;
}