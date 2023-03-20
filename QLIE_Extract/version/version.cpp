#include "version.h"
#include "../QLIE_Extract/QLIE_Extract.h"
#include "../TDA/ConsoleX.h"


VOID UserInterface()
{
	char flag = 0;

	while (true)
	{
		std::cout << "Waiting For Command...\n";
		std::cin >> flag;

		switch (flag)
		{
		case 'p':
		{
			QLIE_Extract::PrintPackInfo();
		}
		break;

		case 'r':
		{
			QLIE_Extract::PrintResInfo();
		}
		break;

		case 'f':
		{
			QLIE_Extract::LogInfo(L"QLIEFileNameList.txt");
		}
		break;

		case 'd':
		{
			QLIE_Extract::ExtractRes();
		}
		break;

		default:
		{
			std::cout << "Illegal command" << std::endl;
		}
		break;

		}
	}
}

VOID StartHook()
{
	TDA::ConsoleX::SetConsole(L"QLIE Extract");

	QLIE_Extract::Init(0x004EE820, 0x004EDB68, 0x00405740);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)UserInterface, NULL, NULL, NULL);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ProxyDll();
		StartHook();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
