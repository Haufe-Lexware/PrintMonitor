#pragma once
class Job;

class Port
{
public:
	Port();
	~Port();

	Job *_pJob = NULL;

public:
	enum LogLevel : DWORD { None, Error, All };
	struct Configuration
	{
		DWORD dwLogLevel;
	};

	static void GetConfiguration(Configuration *pConfiguration) { if (pConfiguration) *pConfiguration = _cfg; }
	static void SetConfiguration(Configuration *pConfiguration) { if (pConfiguration) _cfg = *pConfiguration; }

	static DWORD GetLogLevel() { return _cfg.dwLogLevel; }
	static void SetLogLevel(DWORD dwLogLevel) { _cfg.dwLogLevel = dwLogLevel; }

	static BOOL GetConfiguration(Configuration &cfg);
	static BOOL SetConfiguration(Configuration &cfg);

private:
	static Configuration _cfg;

	static DWORD GetPortSize(DWORD dwLevel, bool bStrings = false);
	static BOOL CopyPort(DWORD dwLevel, PBYTE pBytes, DWORD dwBytes);

public:
	static BOOL CopyPortToBuffer(DWORD dwLevel, LPBYTE pBytes, DWORD dwBytes, PDWORD pdwNeeded, PDWORD pdwPorts);
};

__declspec(selectany) Port::Configuration Port::_cfg = { 
#ifdef _DEBUG
	Port::All
#else
	Port::Error
#endif
};
