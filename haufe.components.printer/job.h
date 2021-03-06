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
#include "printer.h"
#include "log.h"

class Job
{
public:
	Job();
	~Job();

	bool Start(DWORD dwJobId, LPWSTR szJobTitle, LPWSTR szPrinterName);
	void End();
	bool Write(LPCVOID pBuffer, DWORD dwBuffer, PDWORD pdwWritten);

	DWORD GetJobId() { return _dwJobId; }
	PWSTR GetPrinterName() { return _szPrinterName; }

	typedef struct
	{
		DWORD dwProcessID = 0;
		DWORD dwJobID = 0;
		DWORD dwOptions = PrintMonitor::JobOptions::Default;	// PrintMonitor::SetJobOptions
		WCHAR szOutputPath[0x200] = { 0 };						// PrintMonitor::SetJobOptions
		DWORD dwPdfA = 0;										// Registry
	//	DWORD dwImageOption = 0;								// Registry
		DWORD dwJpegOption = 7;									// Registry
		HANDLE hUserToken = NULL;
		HANDLE hEvent = NULL;
	} JOBOPTIONS, *PJOBOPTIONS;

	static BOOL SetJobOptions(JOBOPTIONS &jo)
	{
		return Printer::SetXcvData(jo, Printer::_szSetJobOptions);
	}

	static void SetJobOptions(PJOBOPTIONS pJobOptions);
	static bool GetJobOptions(DWORD dwJobID, JOBOPTIONS &jo);

	template <DWORD dwEvent>
	static void GetEventNameForJobId(WCHAR(&szEvent)[dwEvent], DWORD dwJobId)
	{
		::swprintf_s(szEvent, dwEvent, L"Global\\Haufe.PrintMonitor.%d", dwJobId);
	}

protected:
	void SetPrintMonitorEvent(bool bSuccess);
	bool Start(DWORD dwJobId, LPWSTR szPrinterName);
	bool Ghostscript();

	typedef std::map<DWORD, JOBOPTIONS> JOBS;
	static JOBS _jobs;
	static std::mutex _mutex;

	DWORD _dwJobId = 0;
//	PJOB_INFO_1W _pJobInfo1 = NULL;
//	PJOB_INFO_2W _pJobInfo2 = NULL;
//	DWORD _dwJobInfo1 = 0;
//	DWORD _dwJobInfo2 = 0;
	HANDLE _hWriteThread = NULL;
	HANDLE _hEventWrite = NULL;
	HANDLE _hEventWritten = NULL;
	HANDLE _hFile = NULL;
	PROCESS_INFORMATION _pi = { 0 };
	WCHAR _szPrinterName[MAX_PATH + 1] = { 0 };
	JOBOPTIONS _jo;

//	WCHAR _szOutputPath[MAX_PATH + 1] = { 0 };
//	HANDLE _hUserToken = NULL;
//	HANDLE _hEvent = NULL;

	typedef struct ThreadData
	{
		Job* pJob = NULL;
		LPCVOID pBuffer = NULL;
		DWORD dwBuffer = NULL;
		PDWORD pdwWritten = NULL;
		std::mutex mutex;
	} THREADDATA, *PTHREADDATA;
	THREADDATA _threadData;

	static DWORD WINAPI WriteThread(PVOID pParam);
	static DWORD WINAPI ReadThread(PVOID pParam);
};

__declspec(selectany) Job::JOBS Job::_jobs;
__declspec(selectany) std::mutex Job::_mutex;
