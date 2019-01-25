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
