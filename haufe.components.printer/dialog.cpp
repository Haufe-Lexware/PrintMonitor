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
			PCWSTR szPath = L"HKLM\\SOFTWARE\\WOW6432Node\\Lexware\\PrintMonitor";
#else
			PCWSTR szPath = Printer::IsWow64Process() ?
				L"HKLM\\SOFTWARE\\WOW6432Node\\Lexware\\PrintMonitor" :
				L"HKLM\\SOFTWARE\\Lexware\\PrintMonitor";
#endif
			Automation::RegEdit(szPath);
		}
		break;
	case IDC_SYSLINK2:
		{
#ifdef _WIN64
			WCHAR szGswin[MAX_PATH] = L"%CommonProgramFiles(x86)%\\Lexware";
#else
			WCHAR szGswin[MAX_PATH] = L"%CommonProgramFiles%\\Lexware";
#endif
			ULONG dwGswin = _countof(szGswin);

			ATL::CRegKey key;
			if (ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Lexware\\PrintMonitor", KEY_READ | KEY_WOW64_32KEY))
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
