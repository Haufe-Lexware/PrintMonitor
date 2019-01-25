#pragma once
#include "stdafx.h"
#include "handle.h"
#include "job.h"
#include "log.h"
class Port;

class Monitor
{
private:
	Monitor();

public:
	static LPMONITOR2 Initialize(PMONITORINIT pMonitorInit, PHANDLE phMonitor);
	static void LoadPort();
	static void SavePort();

	static BOOL WINAPI EnumPorts(HANDLE hMonitor, LPWSTR pName, DWORD Level, LPBYTE pPorts, DWORD cbBuf, LPDWORD pcbNeeded, LPDWORD pcReturned);
	static BOOL WINAPI OpenPort(HANDLE hMonitor, LPWSTR pName, PHANDLE pHandle);
	static BOOL WINAPI StartDocPort(HANDLE hPort, LPWSTR pPrinterName, DWORD JobId, DWORD Level, LPBYTE pDocInfo);
	static BOOL WINAPI WritePort(HANDLE hPort, LPBYTE pBuffer, DWORD cbBuf, LPDWORD pcbWritten);
	static BOOL WINAPI ReadPort(HANDLE hPort, LPBYTE pBuffer, DWORD cbBuffer, LPDWORD pcbRead);
	static BOOL WINAPI EndDocPort(HANDLE hPort);
	static BOOL WINAPI ClosePort(HANDLE hPort);
	static BOOL WINAPI XcvOpenPort(HANDLE hMonitor, LPCWSTR pszObject, ACCESS_MASK GrantedAccess, PHANDLE phXcv);
	static DWORD WINAPI XcvDataPort(HANDLE hXcv, LPCWSTR pszDataName, PBYTE pInputData, DWORD cbInputData, PBYTE pOutputData, DWORD cbOutputData, PDWORD pcbOutputNeeded);
	static BOOL WINAPI XcvClosePort(HANDLE hXcv);
	static VOID WINAPI Shutdown(HANDLE hMonitor);

	typedef struct
	{
		Port *pPort = NULL;
		ACCESS_MASK dwGrantedAccess = 0;
	} XCVDATA, *PXCVDATA;

	template <DWORD dwCommand, DWORD dwDirectory>
	static bool GetCommandLine(Job::JOBOPTIONS jo, WCHAR(&szCommand)[dwCommand], WCHAR(&szDirectory)[dwDirectory], PCWSTR szOutputPath)
	{
		return GetCommandLine(jo, szCommand, dwCommand, szDirectory, dwDirectory, szOutputPath);
	}

protected:
	static PMONITORINIT _pMonitorInit;
	static bool GetCommandLine(Job::JOBOPTIONS jo, PWSTR szCommand, DWORD dwCommand, PWSTR szDirectory, DWORD dwDirectory, PCWSTR szOutputPath);
};

__declspec(selectany) PMONITORINIT Monitor::_pMonitorInit = NULL;
