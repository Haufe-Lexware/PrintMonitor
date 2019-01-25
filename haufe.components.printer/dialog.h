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
