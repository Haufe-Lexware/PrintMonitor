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
#include "printer.h"
#include "log.h"

Printer _printer;

//	$PRINTERNAME = "Lexware PDF-Export 6"
//	$DRIVERNAME = "Ghostscript PDF"
//	$PORTNAME = "LexwarePDF:"
//
//	# install the printer (using the previously registered port)
//	runDll "printui.dll,PrintUIEntry /if /f ""$INF"" /r ""$PORTNAME"" /m ""$DRIVERNAME"" /b ""$PRINTERNAME"" /u /Y"
//
//	# uninstall local printer
//	runDll "printui.dll,PrintUIEntry /dl /q /n ""$PRINTERNAME"""
//
//	# uninstall local printer driver
//	runDll "printui.dll,PrintUIEntry /dd /q /m ""$DRIVERNAME"""

// static 
void Printer::Register(PWSTR szInfPath)
{
//	ATL::CAccessToken at;
//	at.EnablePrivilege(SE_LOAD_DRIVER_NAME);

	WCHAR szINFPATH[_MAX_PATH] = { 0 };

	if (szInfPath && szInfPath[0])
	{
		if (szInfPath[0] == L'"')
		{
			szInfPath++;
			if (PWSTR sz = ::wcsrchr(szInfPath, L'"'))
			{
				sz[0] = L'\0';
			}
		}
	}
	else
	{
#ifdef _WIN64
		::ExpandEnvironmentStrings(L"%CommonProgramFiles(x86)%\\" COMPANY_NAME "\\Ghostscript\\driver\\ghostpdf.inf", szINFPATH, _countof(szINFPATH));
#else
		::ExpandEnvironmentStrings(L"%CommonProgramFiles%\\" COMPANY_NAME "\\Ghostscript\\driver\\ghostpdf.inf", szINFPATH, _countof(szINFPATH));
#endif
		szInfPath = szINFPATH;
	}

	::PathFileExists(szInfPath) ? Log.Info(__FUNCTION__ " (%S)", szInfPath) : Log.Error(__FUNCTION__ " (%S)", szInfPath);
	Unregister(false);

	// install printer driver package
	WCHAR szPath[MAX_PATH] = { 0 };
	DWORD dwPath = _countof(szPath);

	HRESULT hr = ::UploadPrinterDriverPackage(NULL, szInfPath, NULL, UPDP_CHECK_DRIVERSTORE, NULL, szPath, &dwPath);
	if (hr != S_OK)
	{
		hr = ::UploadPrinterDriverPackage(NULL, szInfPath, NULL, UPDP_UPLOAD_ALWAYS, NULL, szPath, &dwPath);
	}

	if (hr != S_OK)
	{
		Log.Error(__FUNCTION__ " UploadPrinterDriverPackage (0x%08x)", hr);
	//	return;
	}
	
	hr = ::InstallPrinterDriverFromPackage(NULL, szPath, L"Ghostscript PDF", NULL, IPDFP_COPY_ALL_FILES);
	if (hr != S_OK)
	{
		Log.Error(__FUNCTION__ " InstallPrinterDriverFromPackage (0x%08x)", hr);
		return;
	}

	// install printer monitor
	MONITOR_INFO_2W mi = { 0 };
	mi.pName = Printer::_szMonitorName;
	mi.pDLLName = GetModuleName();

	if (!::AddMonitor(NULL, 2, (PBYTE)&mi))
	{
		Log.Error(__FUNCTION__ " AddMonitor (%S, %S)", mi.pName, mi.pDLLName);
		return;
	}

	// install printer
	PRINTER_INFO_2 pi = { 0 };
	pi.pPrinterName = Printer::_szPrinterName;
	pi.pPortName = Printer::_szPortName;
	pi.pDriverName = Printer::_szDriverName;
	WCHAR szPrintProcessor[] = L"winprint";
	pi.pPrintProcessor = szPrintProcessor;

	if (HANDLE hPrinter = ::AddPrinter(NULL, 2, (LPBYTE)&pi))
	{
		::ClosePrinter(hPrinter);
	}
	else
	{
		Log.Error(__FUNCTION__ " AddPrinter");
	}
}

// static 
void Printer::Unregister(bool bLog)
{
//	ATL::CAccessToken at;
//	at.EnablePrivilege(SE_LOAD_DRIVER_NAME);

	Log.Info(bLog, __FUNCTION__);

	// delete the printer
	HANDLE hPrinter = NULL;
	PRINTER_DEFAULTS pd = { 0 };
	pd.DesiredAccess = PRINTER_ALL_ACCESS;

	if (::OpenPrinter(Printer::_szPrinterName, &hPrinter, &pd))
	{
		if (!::DeletePrinter(hPrinter))
		{
			Log.Error(bLog, __FUNCTION__ " (DeletePrinter)");
		}

	//	::DeletePrinterKey
	//	::DeletePrinterData

		::ClosePrinter(hPrinter);
	}
	else
	{
		Log.Error(bLog, __FUNCTION__ " (OpenPrinter %d)", ::GetLastError());
	}

	// delete the printer driver
	if (!::DeletePrinterDriverEx(NULL, NULL, Printer::_szDriverName, DPD_DELETE_UNUSED_FILES, 0))		// DeletePrinterDriverPackage
	{
		Log.Error(bLog, __FUNCTION__ " (DeletePrinterDriverEx)");
	}

	// delete the print monitor
	if (!::DeleteMonitor(NULL, NULL, Printer::_szMonitorName))
	{
		Log.Error(bLog, __FUNCTION__ " (DeleteMonitor)");
	}
}

// static
PWSTR Printer::GetModuleName()
{
	static PWSTR szDllName = NULL;
	static WCHAR szDefault[] = L"";

	if (!szDllName)
	{
		static WCHAR szModule[_MAX_PATH + 1] = { 0 };
		::GetModuleFileName(reinterpret_cast<HINSTANCE>(&__ImageBase), szModule, _MAX_PATH);
		szDllName = ::PathFindFileName(szModule);
	}

	return szDllName ? szDllName : szDefault;
}

#ifndef _WIN64
// static
bool Printer::IsWow64Process()
{
	BOOL bIsWow64Process = FALSE;
	typedef BOOL(WINAPI *ISWOW64PROCESS)(HANDLE hProcess, PBOOL Wow64Process);
	if (HMODULE hModule = ::GetModuleHandle(_T("kernel32")))
	{
		if (ISWOW64PROCESS IsWow64Process = (ISWOW64PROCESS)::GetProcAddress(hModule, "IsWow64Process"))
		{
			if (!IsWow64Process(::GetCurrentProcess(), &bIsWow64Process))
			{
				ATLASSERT(0 && "IsWow64Process failed!");
			}
		}
	}

	return bIsWow64Process == TRUE ? true : false;
}
#endif