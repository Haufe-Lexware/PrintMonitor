// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

// printui.exe /s /t2
// rundll32.exe "$(TargetPath)",Install
// rundll32.exe printui.dll,PrintUIEntry "/if /f ""D:\tfs\ThirdParty\Ghostscript\gs9.20\lib\ghostpdf.inf"" /r ""LexwarePDF:"" /m ""Ghostscript PDF"" /b ""Lexware PDF"" /u /Y"
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID pReserved)
{
	hModule, pReserved;
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
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
