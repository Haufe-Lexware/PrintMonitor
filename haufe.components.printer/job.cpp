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
#include "job.h"
#include "monitor.h"
#include "sessiontoken.h"
#include "shell.h"
#include "handle.h"
#include "log.h"

Job::Job()
{
}

Job::~Job()
{
	Log.Info(__FUNCTION__);
//	delete[] _pJobInfo1;
//	delete[] _pJobInfo2;
	_hEventWrite ? ::CloseHandle(_hEventWrite) : __noop;
	_hEventWritten ? ::CloseHandle(_hEventWritten) : __noop;
	_jo.hUserToken ? ::CloseHandle(_jo.hUserToken) : __noop;
	_jo.hEvent ? ::CloseHandle(_jo.hEvent) : __noop;
}

// static
void Job::SetJobOptions(PJOBOPTIONS pJobOptions)
{
	auto CreateEvent = [](DWORD dwJobID, HANDLE &hEvent) {
		WCHAR szEvent[MAX_PATH] = { 0 };
		GetEventNameForJobId(szEvent, dwJobID);

		// BA = Administrators, 0x001F0003 = EVENT_ALL_ACCESS
		// IU = Interactive Users, 0x00100000 = SYNCHRONIZE
		SECURITY_ATTRIBUTES saEvent = { sizeof(saEvent), 0 };
		if (::ConvertStringSecurityDescriptorToSecurityDescriptor(L"D:(A;;0x001F0003;;;BA)(A;;0x00100000;;;IU)", SDDL_REVISION_1, &saEvent.lpSecurityDescriptor, NULL))
		{
			hEvent = ::CreateEvent(&saEvent, FALSE, FALSE, szEvent);
			Log.Info("Job::SetJobOptions (CreateEvent, 0x%08x, %S)", hEvent, szEvent);
			::LocalFree(saEvent.lpSecurityDescriptor);
		}
		else
		{
			Log.Error("Job::SetJobOptions (ConvertStringSecurityDescriptorToSecurityDescriptor %d)", ::GetLastError());
		}
	};

	SessionToken::GetUserToken(pJobOptions->dwProcessID, pJobOptions->hUserToken);
	CreateEvent(pJobOptions->dwJobID, pJobOptions->hEvent);

	std::lock_guard<std::mutex> guard(_mutex);
	JOBS::_Pairib pib = _jobs.insert(std::pair<DWORD, JOBOPTIONS>(pJobOptions->dwJobID, *pJobOptions));

	if (!pib.second)
	{
		Log.Warning(__FUNCTION__);
	}
}

// static
bool Job::GetJobOptions(DWORD dwJobID, JOBOPTIONS &jo)
{
	std::lock_guard<std::mutex> guard(_mutex);
	JOBS::iterator it = _jobs.find(dwJobID);

	if (it != _jobs.end())
	{
		jo = it->second;
		_jobs.erase(it);
		return !!jo.hUserToken;
	}

	Log.Warning(__FUNCTION__);
	return false;
}

bool Job::Start(DWORD dwJobId, LPWSTR szJobTitle, LPWSTR szPrinterName)
{
	szJobTitle;
	if (GetJobOptions(dwJobId, _jo))
	{
		return Start(dwJobId, szPrinterName) && Ghostscript();
	}
	else if (SessionToken::GetUserToken(_jo.hUserToken))
	{
		if (Shell::BrowseForFile(_jo.hUserToken, _jo.szOutputPath))
		{
			_jo.dwJpegOption = 0;
			return Start(dwJobId, szPrinterName) && Ghostscript();
		}
	}

	return false;
}

bool Job::Start(DWORD dwJobId, LPWSTR szPrinterName)
{
	CPrinterHandle printer(szPrinterName);

	if (printer)
	{
		_dwJobId = dwJobId;
		::wcscpy_s(_szPrinterName, szPrinterName);

	//	::GetJob(printer, dwJobId, 1, NULL, 0, &_dwJobInfo1);
	//	_pJobInfo1 = (PJOB_INFO_1W)new BYTE[_dwJobInfo1];

	//	if (!::GetJob(printer, dwJobId, 1, (LPBYTE)_pJobInfo1, _dwJobInfo1, &_dwJobInfo1))
	//	{
	//		Log.Error(__FUNCTION__ " (GetJob(1) %d)", ::GetLastError());
	//		return false;
	//	}

	//	::GetJob(printer, dwJobId, 2, NULL, 0, &_dwJobInfo2);
	//	_pJobInfo2 = (PJOB_INFO_2W)new BYTE[_dwJobInfo2];

	//	if (!::GetJob(printer, dwJobId, 2, (LPBYTE)_pJobInfo2, _dwJobInfo2, &_dwJobInfo2))
	//	{
	//		Log.Error(__FUNCTION__ " (GetJob(2) %d)", ::GetLastError());
	//		return false;
	//	}

		_hEventWrite = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!_hEventWrite)
		{
			Log.Error(__FUNCTION__ " (CreateEvent %d)", ::GetLastError());
			return false;
		}

		_hEventWritten = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!_hEventWritten)
		{
			Log.Error(__FUNCTION__ " (CreateEvent %d)", ::GetLastError());
			return false;
		}

		_threadData.pJob = this;
		_hWriteThread = ::CreateThread(NULL, 0, WriteThread, (LPVOID)&_threadData, 0, NULL);
		if (!_hWriteThread)
		{
			Log.Error(__FUNCTION__ " (CreateThread %d)", ::GetLastError());
			return false;
		}
	}
	else
	{
		Log.Error(__FUNCTION__ " (OpenPrinter %d)", ::GetLastError());
		return false;
	}

	return true;
}

void Job::End()
{
	::FlushFileBuffers(_hFile);
	::CloseHandle(_hFile);
	_hFile = NULL;

	if (_pi.hProcess)
	{
		switch (DWORD dwWait = ::WaitForSingleObject(_pi.hProcess, INFINITE))
		{
		case WAIT_OBJECT_0:
			SetPrintMonitorEvent(true);
		//	Log.Info(__FUNCTION__ " (done.)");
			break;
		default:
			SetPrintMonitorEvent(false);
			Log.Error(__FUNCTION__ " (%d)", dwWait);
			break;
		}

		::CloseHandle(_pi.hProcess);
		::CloseHandle(_pi.hThread);
	}
	else
	{
		SetPrintMonitorEvent(false);
		Log.Error(__FUNCTION__ " (hProcess)");
	}
}

void Job::SetPrintMonitorEvent(bool bSuccess)
{
	if (_jo.hEvent)
	{
		bSuccess ? Log.Info(__FUNCTION__ " (%d)", ::PathFileExists(_jo.szOutputPath)) : Log.Error(__FUNCTION__);
		::SetEvent(_jo.hEvent);
	//	::CloseHandle(jo.hEvent);
	}
}

bool Job::Ghostscript()
{
//	ImpersonateUser imp(_hUserToken);

	HANDLE hStdinRead, hStdoutWrite, hStdoutRead;
	SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };

	if (::CreatePipe(&hStdinRead, &_hFile, &sa, 0) && ::CreatePipe(&hStdoutRead, &hStdoutWrite, &sa, 0) && 
		::SetHandleInformation(_hFile, HANDLE_FLAG_INHERIT, 0) && ::SetHandleInformation(hStdoutRead, HANDLE_FLAG_INHERIT, 0))
	{
		WCHAR szDesktop[] = L"winsta0\\default";

		STARTUPINFO si = { sizeof(si), 0};
		si.hStdInput = hStdinRead;
		si.hStdOutput = hStdoutWrite;
		si.hStdError = hStdoutWrite;
		si.wShowWindow = SW_HIDE;
		si.lpDesktop = szDesktop;
		si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

		WCHAR szCommand[0x400];
		WCHAR szDirectory[MAX_PATH];
		if (Monitor::GetCommandLine(_jo, szCommand, szDirectory, _jo.szOutputPath))
		{
			if (::CreateProcessAsUser(_jo.hUserToken, NULL, szCommand, NULL, NULL, TRUE, 0, NULL, szDirectory, &si, &_pi))
		//	if (::CreateProcess(NULL, szCommand, NULL, NULL, TRUE, 0, NULL, szDirectory, &si, &_pi))
			{
				::CloseHandle(hStdinRead);
				::CloseHandle(hStdoutWrite);

				if (HANDLE hThread = ::CreateThread(NULL, 0, ReadThread, (LPVOID)hStdoutRead, 0, NULL))
				{
					::CloseHandle(hThread);
					return true;
				}
				else
				{
					Log.Error(__FUNCTION__ " (CreateThread %d)", ::GetLastError());
					return false;
				}
			}
			else
			{
				::CloseHandle(hStdinRead);
				::CloseHandle(hStdoutWrite);
				::CloseHandle(hStdoutRead);

				Log.Error(__FUNCTION__ " (CreateProcess %d)", ::GetLastError());
				return false;
			}
		}

		return false;
	}
	else
	{
		Log.Error(__FUNCTION__ " (CreatePipe)");
		return false;
	}
}

bool Job::Write(LPCVOID pBuffer, DWORD dwBuffer, PDWORD pdwWritten)
{
	DWORD dwExitCode = 0;
	if (!::GetExitCodeProcess(_pi.hProcess, &dwExitCode) || dwExitCode != STILL_ACTIVE)
	{
		Log.Error(__FUNCTION__ " (GetExitCodeProcess %d, %d)", ::GetLastError(), dwExitCode);
		return false;
	}

	{
		std::lock_guard<std::mutex> guard(_threadData.mutex);
		_threadData.pBuffer = pBuffer;
		_threadData.dwBuffer = dwBuffer;
		_threadData.pdwWritten = pdwWritten;
	}

	::SetEvent(_hEventWrite);

	switch (DWORD dwWait = ::WaitForSingleObject(_hEventWritten, 10000))
	{
	case WAIT_OBJECT_0:
		return true;
	case WAIT_TIMEOUT:
		Log.Error(__FUNCTION__ " (WAIT_TIMEOUT)");
		break;
	default:
		Log.Error(__FUNCTION__ " (%d)", dwWait);
		break;
	}

	::TerminateThread(_hWriteThread, 1);
	::CloseHandle(_hWriteThread);
	_hWriteThread = NULL;

	return false;
}

// static 
DWORD WINAPI Job::WriteThread(LPVOID pParam)
{
	PTHREADDATA pData = (PTHREADDATA)pParam;

	while (true)
	{
		if (::WaitForSingleObject(pData->pJob->_hEventWrite, INFINITE) == WAIT_OBJECT_0)
		{
			std::lock_guard<std::mutex> guard(pData->mutex);
			if (pData->pBuffer == NULL)
			{
				::SetEvent(pData->pJob->_hEventWritten);
				return 0;
			}

			::WriteFile(pData->pJob->_hFile, pData->pBuffer, pData->dwBuffer, pData->pdwWritten, NULL);
		}

		::SetEvent(pData->pJob->_hEventWritten);
	}
}

// static
DWORD WINAPI Job::ReadThread(LPVOID lpParam)
{
	if (HANDLE hRead = (HANDLE)lpParam)
	{
		BYTE pBytes[0x1000];
		DWORD dwRead;

		while (::ReadFile(hRead, pBytes, sizeof(pBytes), &dwRead, NULL) && dwRead)
		{
		//	Log.Info(__FUNCTION__ " (%.*s)", dwRead, pBytes);									// ignore ghostscript output %%[Page: 1]%%, %%[Page: 2]%%, ..
		}

		::CloseHandle(hRead);
	}

	return 0;
}
