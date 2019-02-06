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

class PrintMonitor
{
private:
	PrintMonitor();
	static TCHAR _szModule[];

	template <typename U, typename V>
	static bool Load(PCTSTR szModule, PCSTR szFunction, U u, V v)
	{
		bool bLoad = false;
		if (HMODULE hModule = ::LoadLibrary(szModule))
		{
			typedef BOOL (WINAPI *PF)(U, V);
			if (PF fn = (PF)::GetProcAddress(hModule, szFunction))
			{
				bLoad = !!fn(u, v);
			}
		}

		ATLASSERT(bLoad);
		return bLoad;
	}

	template <typename U, typename V, typename W>
	static bool Load(PCTSTR szModule, PCSTR szFunction, U u, V v, W w)
	{
		bool bLoad = false;
		if (HMODULE hModule = ::LoadLibrary(szModule))
		{
			typedef BOOL (WINAPI *PF)(U, V, W);
			if (PF fn = (PF)::GetProcAddress(hModule, szFunction))
			{
				bLoad = !!fn(u, v, w);
			}

			::FreeLibrary(hModule);
		}

		ATLASSERT(bLoad);
		return bLoad;
	}

public:
#pragma warning (push)
#pragma warning (disable:4480)		// warning C4480: nonstandard extension used: specifying underlying type for enum 'PrintMonitor::JobOptions'
	enum JobOptions : DWORD {
		Default = 0 << 0,
		PDFA = 1 << 0,
	};
#pragma warning (pop)

	static bool SetJobOptions(DWORD dwJobID, JobOptions dwOptions, PCSTR szPath)
	{
		return Load(_szModule, "SetJobOptionsA", dwJobID, dwOptions, szPath);
	}

	static bool SetJobOptions(DWORD dwJobID, JobOptions dwOptions, PCWSTR szPath)
	{
		return Load(_szModule, "SetJobOptionsW", dwJobID, dwOptions, szPath);
	}

	static bool WaitForJob(DWORD dwJobId, DWORD dwTimeout = INFINITE)
	{
		return Load(_szModule, "WaitForJob", dwJobId, dwTimeout);
	}
};

DEFINE_ENUM_FLAG_OPERATORS(PrintMonitor::JobOptions);
__declspec(selectany) TCHAR PrintMonitor::_szModule[] = _T("Haufe.Components.Printer.dll");