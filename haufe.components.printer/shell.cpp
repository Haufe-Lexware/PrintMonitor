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
#include "impersonate.h"
#include "shell.h"
#include "log.h"

// static
bool Shell::BrowseForFile(HANDLE hUserToken, PWSTR szPath, DWORD dwPath)
{
	szPath, dwPath;
	ImpersonateUser imp(hUserToken);

	bool bBrowseForFile = false;
	WCHAR szDesktop[] = L"winsta0\\default";

	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { sizeof(si), 0 };
	si.lpDesktop = szDesktop;

	GUID guid = { 0 };
	::CoCreateGuid(&guid);
	ATL::CComBSTR bstrGuid(guid);

	WCHAR szGuid[MAX_PATH] = { 0 };
	::swprintf_s(szGuid, L"Global\\%s", (BSTR)bstrGuid);

	WCHAR szCommand[0x200];
	::swprintf_s(szCommand, L"rundll32.exe \"%s\",QueryOutputPath %s", Printer::GetModuleName(), szGuid);

	if (HANDLE hShared = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, Size, szGuid))
	{
		if (PWSTR pShared = (PWSTR)::MapViewOfFile(hShared, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, Size))
		{
			if (::CreateProcessAsUser(hUserToken, NULL, szCommand, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				pShared[0] = L'\0';
				switch (DWORD dwWait = ::WaitForSingleObject(pi.hProcess, INFINITE))
				{
				case WAIT_OBJECT_0:
					::wcscpy_s(szPath, dwPath, pShared);
					bBrowseForFile = pShared[0] != L'\0';
					break;
				case WAIT_TIMEOUT:
					Log.Error(__FUNCTION__ " (WAIT_TIMEOUT)");
					break;
				default:
					Log.Error(__FUNCTION__ " (%d)", dwWait);
					break;
				}

				::CloseHandle(pi.hThread);
				::CloseHandle(pi.hProcess);
			}
			else
			{
				Log.Info(__FUNCTION__ " (MapViewOfFile %d)", ::GetLastError());
			}

			::UnmapViewOfFile(pShared);
		}
		else
		{
			Log.Info(__FUNCTION__ " (CreateFileMapping %d)", ::GetLastError());
		}

		::CloseHandle(hShared);
	}
	else
	{
		Log.Info(__FUNCTION__ " (MapViewOfFile %d)", ::GetLastError());
	}

	return bBrowseForFile;
}

// static 
void Shell::QueryOutputPath(PWSTR szGuid)
{
	if (szGuid)
	{
		Log.Info(__FUNCTION__ " (%S)", szGuid);

	//	if (HANDLE hShared = ::OpenFileMapping(PAGE_READWRITE, FALSE, szGuid))
		if (HANDLE hShared = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, Size, szGuid))
		{
			if (PWSTR pShared = (PWSTR)::MapViewOfFile(hShared, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, Size))
			{
				Shell::BrowseForFile(pShared, Size);

				::UnmapViewOfFile(pShared);
			}
			else
			{
				Log.Info(__FUNCTION__ " (MapViewOfFile %d)", ::GetLastError());
			}

			::CloseHandle(hShared);
		}
		else
		{
			Log.Info(__FUNCTION__ " (OpenFileMapping %d)", ::GetLastError());
			Log.Info(__FUNCTION__ " (CreateFileMapping %d)", ::GetLastError());
		}
	}
	else
	{
		Log.Info(__FUNCTION__ " (szGuid)");
	}
}

struct ComInitialize
{
	ComInitialize() { ::CoInitialize(NULL); }
	~ComInitialize() { ::CoUninitialize(); }
};

// static 
bool Shell::BrowseForFile(PWSTR szPath, DWORD dwPath)
{
	_COM_SMARTPTR_TYPEDEF(IFileDialog, __uuidof(IFileDialog));
	_COM_SMARTPTR_TYPEDEF(IShellItem, __uuidof(IShellItem));

	bool bBrowseFolder = false;
	ComInitialize com;

	try
	{
		COMDLG_FILTERSPEC fs[1] = { L"PDF Dokument", L"*.pdf" };
		IFileDialogPtr spFileDialog(CLSID_FileSaveDialog);
		spFileDialog->SetOptions(FOS_OVERWRITEPROMPT | FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM | FOS_DONTADDTORECENT);
		spFileDialog->SetDefaultExtension(L".pdf");
		spFileDialog->SetFileTypes(1, fs);

		TCHAR szDocuments[_MAX_PATH];
		::SHGetSpecialFolderPath(NULL, szDocuments, CSIDL_MYDOCUMENTS, FALSE);

		IShellItemPtr spShellItem;
		::SHCreateItemFromParsingName(szDocuments, NULL, __uuidof(spShellItem), (PVOID*)&spShellItem);
		spFileDialog->SetFolder(spShellItem);

		spFileDialog->Show(NULL);
		spFileDialog->GetResult(&spShellItem);

		if (spShellItem)
		{
			PWSTR pszPath = NULL;
			spShellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszPath);
			::wcscpy_s(szPath, dwPath, pszPath);
			::CoTaskMemFree(pszPath);

			bBrowseFolder = true;
		}
	}
	catch (_com_error &e)
	{
		Log.Error(__FUNCTION__ " (%S)", e.ErrorMessage());
	}

	return bBrowseFolder;
}
