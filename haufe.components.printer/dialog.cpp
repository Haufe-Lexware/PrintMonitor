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
#include "dialog.h"
#include "printer.h"
#include "automation.h"

CDialog::CDialog()
{
}

CDialog::~CDialog()
{
}

LRESULT CDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	uMsg, wParam, lParam, bHandled;
	Port::GetConfiguration(_cfg);
	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO1 + _cfg.dwLogLevel);

	return TRUE;
}

LRESULT CDialog::OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
	wNotifyCode, hWndCtl, bHandled;
	switch (wID)
	{
	case IDOK:
		_cfg.dwLogLevel = IsDlgButtonChecked(IDC_RADIO1) ? Port::None : IsDlgButtonChecked(IDC_RADIO2) ? Port::Error : IsDlgButtonChecked(IDC_RADIO3) ? Port::All : Port::None;
		Port::SetConfiguration(_cfg);
		break;
	case IDCANCEL:
		break;
	}

	EndDialog(wID);
	return 0;
}

LRESULT CDialog::OnSyslink(int idCtrl, LPNMHDR pNMHdr, BOOL &bHandled)
{
	pNMHdr, bHandled;
	switch (idCtrl)
	{
	case IDC_SYSLINK1:
		{
#ifdef _WIN64
			PCWSTR szPath = L"HKLM\\SOFTWARE\\WOW6432Node\\" COMPANY_NAME "\\PrintMonitor";
#else
			PCWSTR szPath = Printer::IsWow64Process() ?
				L"HKLM\\SOFTWARE\\WOW6432Node\\" COMPANY_NAME "\\PrintMonitor" :
				L"HKLM\\SOFTWARE\\" COMPANY_NAME "\\PrintMonitor";
#endif
			Automation::RegEdit(szPath);
		}
		break;
	case IDC_SYSLINK2:
		{
#ifdef _WIN64
			WCHAR szGswin[MAX_PATH] = L"%CommonProgramFiles(x86)%\\" COMPANY_NAME;
#else
			WCHAR szGswin[MAX_PATH] = L"%CommonProgramFiles%\\" COMPANY_NAME;
#endif
			ULONG dwGswin = _countof(szGswin);

			ATL::CRegKey key;
			if (ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\" COMPANY_NAME "\\PrintMonitor", KEY_READ | KEY_WOW64_32KEY))
			{
				if (ERROR_SUCCESS == key.QueryStringValue(L"gswin", szGswin, &dwGswin))
				{
					::PathRemoveFileSpec(szGswin);
				}
			}

			SHELLEXECUTEINFO se = { sizeof(se), 0 };
			se.fMask = SEE_MASK_DEFAULT | SEE_MASK_DOENVSUBST;
			se.lpVerb = L"open";
			se.lpFile = szGswin;
			se.nShow = SW_SHOW;
			::ShellExecuteEx(&se);
		}
		break;
	case IDC_SYSLINK3:
		{
			SHELLEXECUTEINFO se = { sizeof(se), 0 };
			se.fMask = SEE_MASK_DEFAULT | SEE_MASK_DOENVSUBST;
			se.lpVerb = L"open";
			se.lpFile = L"%SystemRoot%\\System32\\winevt\\Logs\\Haufe Printer.evtx";
			se.nShow = SW_SHOW;
			::ShellExecuteEx(&se);
		}
		break;
	}

	return 0;
}
