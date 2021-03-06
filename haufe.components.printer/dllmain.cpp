//	PrintMonitor for the Ghostscript Printer Driver
//	Copyright (C) 2019  Haufe-Lexware
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Affero General Public License as
//	published by the Free Software Foundation, either version 3 of the
//	License, or (at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Affero General Public License for more details.
//
//	You should have received a copy of the GNU Affero General Public License
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
