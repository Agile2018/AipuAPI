// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

//DWORD WINAPI ThreadMain(LPVOID lpParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:	
		DisableThreadLibraryCalls(hModule);
		break;
	//{
	//	DisableThreadLibraryCalls(hModule);		
	//	DWORD  dwThreadId;
	//	HANDLE hThread = CreateThread(
	//		NULL, 0, (LPTHREAD_START_ROUTINE)ThreadMain, NULL, 0, &dwThreadId);
	//	//break;
	//}
	//return TRUE;
    case DLL_THREAD_ATTACH:		
    case DLL_THREAD_DETACH:		
    case DLL_PROCESS_DETACH:
		//return TRUE;
        break;
    }
    return TRUE;
}

//DWORD WINAPI ThreadMain(LPVOID lpParam)
//{	
//	MessageBox(NULL, L"From new thread", L"From new thread", MB_OK);
//	return 0;
//}