#include "stdafx.h"
#include "monitorui.h"
#include "dialog.h"
#include "log.h"

// static
BOOL WINAPI CMonitorUI::AddPortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszMonitorNameIn, PWSTR* ppszPortNameOut)
{
	pszServer, hWnd, pszMonitorNameIn, ppszPortNameOut;
	Log.Info(__FUNCTION__);

	::SetLastError(ERROR_CAN_NOT_COMPLETE);
	return FALSE;
}

// static 
BOOL WINAPI CMonitorUI::ConfigurePortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszPortName)
{
	pszServer, pszPortName;
	Log.Info(__FUNCTION__);

	CDialog dlg;
	dlg.DoModal(hWnd);
	
	return TRUE;
}

// static 
BOOL WINAPI CMonitorUI::DeletePortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszPortName)
{
	pszServer, hWnd, pszPortName;
	Log.Info(__FUNCTION__);

	::SetLastError(ERROR_CAN_NOT_COMPLETE);
	return FALSE;
}
