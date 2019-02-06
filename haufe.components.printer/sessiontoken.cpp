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
#include "sessiontoken.h"
#include "impersonate.h"
#include "log.h"

// static 
bool SessionToken::GetUserToken(DWORD dwProcessID, HANDLE &hPrimaryToken)
{
	hPrimaryToken = NULL;

	if (dwProcessID)
	{
		if (HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID))
		{
			HANDLE hProcessToken = NULL;
			if (::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hProcessToken))
			{
				if (!::DuplicateTokenEx(hProcessToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hPrimaryToken))
				{
					Log.Error(__FUNCTION__ " (DuplicateTokenEx %d)", ::GetLastError());
				}

				::CloseHandle(hProcessToken);
			}
			else
			{
				Log.Error(__FUNCTION__ " (OpenProcessToken %d)", ::GetLastError());
			}

			::CloseHandle(hProcess);
		}
		else
		{
			Log.Error(__FUNCTION__ " (OpenProcess %d)", ::GetLastError());
		}
	}
	else
	{
		Log.Error(__FUNCTION__ " (dwProcessID)");
	}

	return !!hPrimaryToken;
}

// static, unused
bool SessionToken::EnablePrivilege()
{
	bool bEnablePrivilege = FALSE;
	HANDLE hToken = NULL;

	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		LUID luid = { 0 };
		if (::LookupPrivilegeValue(NULL, SE_TCB_NAME, &luid))
		{
			TOKEN_PRIVILEGES tkp = { 0 };
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Luid = luid;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if (::AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
			{
				bEnablePrivilege = TRUE;
			}
			else
			{
				Log.Error(__FUNCTION__ " (AdjustTokenPrivileges %d)", ::GetLastError());
			}
		}
		else
		{
			Log.Error(__FUNCTION__ " LookupPrivilegeValue %d)", ::GetLastError());
		}

		::CloseHandle(hToken);
	}
	else
	{
		Log.Error(__FUNCTION__ " OpenProcessToken %d)", ::GetLastError());
	}

	return bEnablePrivilege;
}

// static
bool SessionToken::GetUserToken(HANDLE &hUserToken)
{
//	ATL::CAccessToken at;
//	at.EnablePrivilege(SE_TCB_NAME);
//	EnablePrivilege();
	Impersonate imp;

	hUserToken = NULL;
	DWORD dwSessionId = ::WTSGetActiveConsoleSessionId();

	if (dwSessionId != 0xffffffff)
	{
		if (::WTSQueryUserToken(dwSessionId, &hUserToken))										// obtain the primary access token of the logged-on user specified by the session ID
		{
			return true;
		}
		else																					// remote desktop
		{
			DWORD dwLastError = ::GetLastError();
			if (dwLastError == ERROR_NO_TOKEN)													// SERVICE_ACCEPT_SESSIONCHANGE/SERVICE_CONTROL_SESSIONCHANGE
			{
				DWORD dwSessions = 0;
				PWTS_SESSION_INFO psi = NULL;
				if (::WTSEnumerateSessions(WTS_CURRENT_SERVER_NAME, 0, 1, &psi, &dwSessions))	// WTSQuerySessionInformation, WTSOpenServer "RDP_Tcp#0"
				{
					for (DWORD i = 0; i < dwSessions; i++)
					{
						if (::WTSQueryUserToken(psi[i].SessionId, &hUserToken))
						{
							break;
						}
					}

					::WTSFreeMemory(psi);
					if (!hUserToken)
					{
						Log.Error(__FUNCTION__ " (WTSQueryUserToken)");
					}
					
					return !!hUserToken;
				}
				else
				{
					Log.Error(__FUNCTION__ " (WTSEnumerateSessions %d)", ::GetLastError());
				}
			}
			else
			{
				Log.Error(__FUNCTION__ " (WTSQueryUserToken %d)", dwLastError);					// ERROR_PRIVILEGE_NOT_HELD
			}
		}
	}
	else
	{
		Log.Error(__FUNCTION__ " (WTSGetActiveConsoleSessionId %d)", ::GetLastError());
	}

	return false;
}
