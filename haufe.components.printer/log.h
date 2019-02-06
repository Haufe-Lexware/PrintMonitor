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
#include "event.h"

class CLog
{
protected:
	CLog();
	~CLog();

	void Report(WORD wType, WORD wCategory, DWORD dwEvent, PCSTR szFormat, va_list va);

public:
	static CLog& Log();
	void Success(PCSTR szFormat, ...);
	void Info(PCSTR szFormat, ...);
	void Info(bool bLog, PCSTR szFormat, ...);
	void Warning(PCSTR szFormat, ...);
	void Error(PCSTR szFormat, ...);
	void Error(bool bLog, PCSTR szFormat, ...);

	static HRESULT Register();
	static HRESULT Unregister();

protected:
	HANDLE _hEvent;
	static TCHAR _szEventLog[];
};

__declspec(selectany) TCHAR CLog::_szEventLog[] = _T("Haufe Printer");
extern CLog& Log;
