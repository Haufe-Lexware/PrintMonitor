#pragma once
#include "stdafx.h"

class CMonitorUI
{
private:
	CMonitorUI();

public:
	static PMONITORUI Initialize();

protected:
	static BOOL WINAPI AddPortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszMonitorNameIn, PWSTR* ppszPortNameOut);
	static BOOL WINAPI ConfigurePortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszPortName);
	static BOOL WINAPI DeletePortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszPortName);
};