// Sample.h : main header file for the Sample application
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

// CSampleApp:
// See Sample.cpp for the implementation of this class
class CSampleApp : public CWinApp
{
public:
	CSampleApp();

private:
	static BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);

public:
	BOOL SelectPrinter(PTSTR szPrinter);
	static void TaskDialog(PCWSTR szMainInstruction, PCWSTR szFormat, ...);

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSampleApp theApp;
