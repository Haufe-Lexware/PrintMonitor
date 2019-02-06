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
#include "resource.h"
#include "port.h"
#include "log.h"

CLog& Log = CLog::Log();

//
CLog::CLog()
{
	_hEvent = ::OpenEventLog(NULL, _szEventLog);
}

CLog::~CLog()
{
	if (_hEvent)
	{
		::CloseEventLog(_hEvent);
	}
}

// static
CLog& CLog::Log()
{
	static CLog log;
	return log;
}

// static, _vstprintf
void CLog::Report(WORD wType, WORD wCategory, DWORD dwEvent, PCSTR szFormat, va_list va)
{
	switch (Port::GetLogLevel())
	{
		case Port::None:
			return;
		case Port::Error:
			switch (wCategory)
			{
				case idcSuccess:
				case idcInfo:
				case idcWarning:
					return;
				case idcError:
					break;
			}
			break;
		case Port::All:
			break;
	}

	if (_hEvent != (HANDLE)ERROR_INVALID_HANDLE)
	{
		size_t length = ::_vscprintf(szFormat, va) + 1;					// _vscprintf doesn't count the '\0'
		if (PSTR szMessage = new CHAR[length])
		{
			::vsprintf_s(szMessage, length, szFormat, va);
			ATLVERIFY(::ReportEventA(_hEvent, wType, wCategory, dwEvent, NULL, 1, 0, (PCSTR*)&szMessage, NULL));

			delete [] szMessage;
		}
	}
}

void CLog::Success(PCSTR szFormat, ...)
{
	va_list va = NULL;
	va_start(va, szFormat);
	Report(EVENTLOG_SUCCESS, idcSuccess, ideSuccess, szFormat, va);
	va_end(va);
}

void CLog::Info(PCSTR szFormat, ...)
{
	va_list va = NULL;
	va_start(va, szFormat);
	Report(EVENTLOG_INFORMATION_TYPE, idcInfo, ideInfo, szFormat, va);
	va_end(va);
}

void CLog::Info(bool bLog, PCSTR szFormat, ...)
{
	if (bLog)
	{
		va_list va = NULL;
		va_start(va, szFormat);
		Report(EVENTLOG_INFORMATION_TYPE, idcInfo, ideInfo, szFormat, va);
		va_end(va);
	}
}

void CLog::Warning(PCSTR szFormat, ...)
{
	va_list va = NULL;
	va_start(va, szFormat);
	Report(EVENTLOG_WARNING_TYPE, idcWarning, ideWarning, szFormat, va);
	va_end(va);
}

void CLog::Error(PCSTR szFormat, ...)
{
	va_list va = NULL;
	va_start(va, szFormat);
	Report(EVENTLOG_ERROR_TYPE, idcError, ideError, szFormat, va);
	va_end(va);
}

void CLog::Error(bool bLog, PCSTR szFormat, ...)
{
	if (bLog)
	{
		va_list va = NULL;
		va_start(va, szFormat);
		Report(EVENTLOG_ERROR_TYPE, idcError, ideError, szFormat, va);
		va_end(va);
	}
}

// static 
HRESULT CLog::Register()
{
	HRESULT hr = ATL::_pAtlModule->UpdateRegistryFromResource(IDR_EVENT, TRUE, NULL);

	// %SystemRoot%\System32\winevt\Logs\Haufe Printer.evtx
	HANDLE hEvent = ::RegisterEventSource(NULL, _szEventLog);
	::DeregisterEventSource(hEvent);
	
	ATLASSERT(hr == S_OK);
	return hr;
}

// static
HRESULT CLog::Unregister()
{
	HRESULT hr = ATL::_pAtlModule->UpdateRegistryFromResource(IDR_EVENT, FALSE, NULL);
	return hr;
}
