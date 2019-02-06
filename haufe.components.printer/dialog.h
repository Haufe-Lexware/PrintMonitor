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
#include "monitor.h"
#include "port.h"
#include "resource.h"

class CDialog :
	public ATL::CDialogImpl<CDialog>
{
public:
	CDialog();
	~CDialog();

	CONST DWORD IDD = IDD_DIALOG;

protected:
	Port::Configuration _cfg;

	BEGIN_MSG_MAP(CWindow)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnClose)
		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
		NOTIFY_RANGE_CODE_HANDLER(IDC_SYSLINK1, IDC_SYSLINK3, NM_CLICK, OnSyslink)
		NOTIFY_RANGE_CODE_HANDLER(IDC_SYSLINK1, IDC_SYSLINK3, NM_RETURN, OnSyslink)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
	LRESULT OnSyslink(int idCtrl, LPNMHDR pNMHdr, BOOL &bHandled);
};
