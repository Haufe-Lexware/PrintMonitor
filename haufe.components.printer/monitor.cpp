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
#include "monitor.h"
#include "port.h"
#include "impersonate.h"
#include "log.h"

// static 
LPMONITOR2 Monitor::Initialize(PMONITORINIT pMonitorInit, PHANDLE phMonitor)
{
	phMonitor;
	if (pMonitorInit)
	{
		if (pMonitorInit->bLocal)
		{
			Log.Info(__FUNCTION__);
			_pMonitorInit = pMonitorInit;
			LoadPort();
			
			static MONITOR2 mon = { sizeof(MONITOR2), 0 };
			mon.pfnEnumPorts = EnumPorts;
			mon.pfnOpenPort = OpenPort;
			mon.pfnStartDocPort = StartDocPort;
			mon.pfnWritePort = WritePort;
			mon.pfnReadPort = ReadPort;
			mon.pfnEndDocPort = EndDocPort;
			mon.pfnClosePort = ClosePort;
			mon.pfnXcvOpenPort = XcvOpenPort;
			mon.pfnXcvDataPort = XcvDataPort;
			mon.pfnXcvClosePort = XcvClosePort;
			mon.pfnShutdown = Shutdown;

			return &mon;
		}
		else
		{
			Log.Error(__FUNCTION__ "(bLocal)");
			return NULL;
		}
	}

	Log.Error(__FUNCTION__ "(pMonitorInit)");
	return NULL;
}

// static 
void Monitor::LoadPort()
{
	if (PMONITORREG pMonitorReg = _pMonitorInit->pMonitorReg)
	{
		if (HKEYMONITOR hRoot = _pMonitorInit->hckRegistryRoot)
		{
		//	DWORD dwIndex = 0;
		//	WCHAR szPortName[_MAX_PATH] = { 0 };
		//	DWORD dwPortName = _countof(szPortName);

		//	LONG dwEnumKey = pMonitorReg->fpEnumKey(hRoot, dwIndex++, szPortName, &dwPortName, NULL, _pMonitorInit->hSpooler);
		//	if (dwEnumKey == ERROR_SUCCESS)
			{
				HKEYMONITOR hKeyPort = NULL;
				if (pMonitorReg->fpOpenKey(hRoot, Printer::_szPortName, KEY_QUERY_VALUE, &hKeyPort, _pMonitorInit->hSpooler) == ERROR_SUCCESS)
				{
					DWORD dwLogLevel = 0;
					DWORD dwData = sizeof(dwLogLevel);

					if (pMonitorReg->fpQueryValue(hKeyPort, L"LogLevel", NULL, (LPBYTE)&dwLogLevel, &dwData, _pMonitorInit->hSpooler) == ERROR_SUCCESS)
					{
						Log.Info(__FUNCTION__ " (%S, %d)", Printer::_szPortName, dwLogLevel);
						Port::SetLogLevel(dwLogLevel);
					}
					else
					{
						Log.Info(__FUNCTION__ " (fpQueryValue)");
					}

					pMonitorReg->fpCloseKey(hKeyPort, _pMonitorInit->hSpooler);
				}
				else
				{
					Log.Info(__FUNCTION__ " (fpOpenKey)");
				}
			}
		//	else if (dwEnumKey == ERROR_NO_MORE_ITEMS)
		//	{
		//		Log.Info(__FUNCTION__);
		//	}
		//	else
		//	{
		//		Log.Error(__FUNCTION__ " (fpEnumKey)");
		//	}
		}
		else
		{
			Log.Error(__FUNCTION__ " (hRoot)");
		}
	}
	else
	{
		Log.Error(__FUNCTION__ " (pMonitorReg)");
	}
}

// static 
void Monitor::SavePort()
{
	Impersonate imp;

	if (PMONITORREG pMonitorReg = _pMonitorInit->pMonitorReg)
	{
		if (HKEYMONITOR hRoot = _pMonitorInit->hckRegistryRoot)
		{
			DWORD dwDisposition = 0;
			HKEYMONITOR hKeyPort = NULL;

			if (pMonitorReg->fpCreateKey(hRoot, Printer::_szPortName, 0, KEY_WRITE, NULL, &hKeyPort, &dwDisposition, _pMonitorInit->hSpooler) == ERROR_SUCCESS)
			{
				if (dwDisposition == REG_CREATED_NEW_KEY)
				{
					Log.Info(__FUNCTION__ " (%S)", Printer::_szPortName);
				}
				else if (dwDisposition == REG_OPENED_EXISTING_KEY)
				{
					DWORD dwLogLevel = Port::GetLogLevel();
					if (pMonitorReg->fpSetValue(hKeyPort, L"LogLevel", REG_DWORD, (LPBYTE)&dwLogLevel, sizeof(dwLogLevel), _pMonitorInit->hSpooler) == ERROR_SUCCESS)
					{
						Log.Info(__FUNCTION__ " (%S, %d)", Printer::_szPortName, dwLogLevel);
					}
					else
					{
						Log.Error(__FUNCTION__ " (fpSetValue)");
					}
				}
					
				pMonitorReg->fpCloseKey(hKeyPort, _pMonitorInit->hSpooler);
			}
			else
			{
				Log.Error(__FUNCTION__ " (fpCreateKey)");
			}
		}
		else
		{
			Log.Error(__FUNCTION__ " (hRoot)");
		}
	}
	else
	{
		Log.Error(__FUNCTION__ " (pMonitorReg)");
	}
}

// static 
bool Monitor::GetCommandLine(Job::JOBOPTIONS jo, PWSTR szCommand, DWORD dwCommand, PWSTR szDirectory, DWORD dwDirectory, PCWSTR szOutputPath)
{
	bool bPdfA = jo.dwPdfA || !!(jo.dwOptions & PrintMonitor::JobOptions::PDFA);
	DWORD dwSettings = 0;

	if (!bPdfA)
	{
		switch (jo.dwJpegOption)
		{
		default:
		case 0:		// Default
			dwSettings = 0;
			break;
		case 3:		// Hoch
			dwSettings = 1;
			break;
		case 7:		// Mittel
			dwSettings = 2;
			break;
		case 9:		// Niedrig
			dwSettings = 3;
			break;
		}
	}

#ifdef _WIN64
	PCWSTR szGswin = L"%CommonProgramFiles(x86)%\\" COMPANY_NAME "\\Ghostscript\\gswin32c.exe";
#else
	PCWSTR szGswin = Printer::IsWow64Process() ?
		L"%CommonProgramFiles(x86)%\\" COMPANY_NAME "\\Ghostscript\\gswin32c.exe" :
		L"%CommonProgramFiles%\\" COMPANY_NAME "\\Ghostscript\\gswin32c.exe";
#endif

	// https://ghostscript.com/pipermail/gs-bugs/2016-May/044483.html
	WCHAR szCmds[][0x200] = {
		L"\"%s\" -dBATCH -dNOPAUSE -dNOPROMPT -sDEVICE=pdfwrite -dAutoRotatePages=/PageByPage %s -dEmbedAllFonts=true -dSubsetFonts=true -dCompressFonts=true -dCompatibilityLevel=1.7 -sOutputFile=\"%s\" -c .setpdfwrite \"<</AlwaysEmbed [] /NeverEmbed []>> setdistillerparams\" -",
		L"\"%s\" -dBATCH -dNOPAUSE -dNOPROMPT -sDEVICE=pdfwrite -dAutoRotatePages=/PageByPage -dPDFA=1 -sOutputFile=\"%s\" PDFA_def.ps -",
	};

	WCHAR szSettings[][0xAF] = {
		L"-dPDFSETTINGS=/prepress",
		L"-dDownsampleColorImages=true -dColorImageResolution=72",
		L"-dDownsampleColorImages=true -dColorImageResolution=150",
		L"-dDownsampleColorImages=true -dColorImageResolution=300",
	};

	WCHAR szPath[MAX_PATH];
	ULONG dwPath = _countof(szPath);
	::ExpandEnvironmentStrings(szGswin, szPath, dwPath);

	if (::PathFileExists(szPath))
	{
		::wcscpy_s(szDirectory, dwDirectory, szPath);
		::PathRemoveFileSpec(szDirectory);
		bPdfA ? 
			::swprintf_s(szCommand, dwCommand, szCmds[1], szPath, szOutputPath) :
			::swprintf_s(szCommand, dwCommand, szCmds[0], szPath, szSettings[dwSettings], szOutputPath);

		Log.Info(__FUNCTION__ " (%S, %S)", szCommand, szDirectory);
		return true;
	}
	else
	{
		Log.Error(__FUNCTION__ " (PathFileExists %S)", szGswin);
		return false;
	}
}
