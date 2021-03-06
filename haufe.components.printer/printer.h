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

#pragma once
#include "handle.h"
#include "log.h"

class Printer :
	public ATL::CAtlDllModuleT<Printer>
{
public:
	static void Register(PWSTR szInfPath);
	static void Unregister(bool bLog);
	static PWSTR GetModuleName();
#ifndef _WIN64
	static bool IsWow64Process();
#endif

	static WCHAR _szPortName[];
	static WCHAR _szMonitorName[];
	static WCHAR _szPrinterName[];
	static WCHAR _szDriverName[];
	static WCHAR CONST _szDescription[];
	static WCHAR CONST _szSetJobOptions[];
	static WCHAR CONST _szGetConfiguration[];
	static WCHAR CONST _szSetConfiguration[];
	static WCHAR CONST _szMonitorUI[];

public:
	template<typename T>
	static BOOL SetXcvData(T& t, PCWSTR szName)
	{
		if (szName && (!::wcsncmp(szName, L"Set", 3) || !::wcsncmp(szName, L"Get", 3)))
		{
			WCHAR szPrinter[MAX_PATH];
			::swprintf_s(szPrinter, L",XcvPort %s", Printer::_szPortName);
			CPrinterHandle printer(szPrinter, SERVER_ACCESS_ADMINISTER);

			if (printer)
			{
				DWORD cbOutputNeeded = 0, dwStatus = 0xffffffff;
				BOOL bXcvData = !::wcsncmp(szName, L"Set", 3) ?
					::XcvData(printer, szName, (PBYTE)&t, sizeof(t), NULL, 0, &cbOutputNeeded, &dwStatus) :
					::XcvData(printer, szName, NULL, 0, (PBYTE)&t, sizeof(t), &cbOutputNeeded, &dwStatus);

				if (bXcvData)
				{
					dwStatus == ERROR_SUCCESS ? Log.Info(__FUNCTION__ "(%S)", szName) : Log.Error(__FUNCTION__ "(%S, %d)", szName, dwStatus);
					::SetLastError(dwStatus);
					return dwStatus == ERROR_SUCCESS;
				}

				Log.Error(__FUNCTION__ " (%S, %d)", szName, ::GetLastError());
			}
		}
		else
		{
			Log.Error(__FUNCTION__ " (%S) ERROR_BAD_ARGUMENTS", szName);
			::SetLastError(ERROR_BAD_ARGUMENTS);
		}

		return FALSE;
	}
};

__declspec(selectany) WCHAR Printer::_szPortName[] = COMPANY_NAME L"PDF:";
__declspec(selectany) WCHAR Printer::_szMonitorName[] = COMPANY_NAME L" PDF";
__declspec(selectany) WCHAR Printer::_szPrinterName[] = COMPANY_NAME L" PDF-Export 6";
__declspec(selectany) WCHAR Printer::_szDriverName[] = L"Ghostscript PDF";
__declspec(selectany) WCHAR CONST Printer::_szDescription[] = COMPANY_NAME L" PDF-Export";
__declspec(selectany) WCHAR CONST Printer::_szSetJobOptions[] = L"SetJobOptions";
__declspec(selectany) WCHAR CONST Printer::_szGetConfiguration[] = L"GetConfiguration";
__declspec(selectany) WCHAR CONST Printer::_szSetConfiguration[] = L"SetConfiguration";
__declspec(selectany) WCHAR CONST Printer::_szMonitorUI[] = L"MonitorUI";
