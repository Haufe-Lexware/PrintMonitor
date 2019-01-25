// Sample.cpp : Defines the class behaviors for the application.
#include "stdafx.h"
#include "Sample.h"
#include "MainFrm.h"
#include "SampleDoc.h"
#include "SampleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSampleApp
BEGIN_MESSAGE_MAP(CSampleApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSampleApp::OnAppAbout)
	// Standard file based document commands
//	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSampleApp construction
CSampleApp::CSampleApp()
{
	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Sample.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CSampleApp object
CSampleApp theApp;

// CSampleApp initialization
BOOL CSampleApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSampleDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSampleView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int CSampleApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CSampleApp message handlers
BOOL CSampleApp::SelectPrinter(PTSTR szPrinter)
{
	HGLOBAL hDevNames, hDevMode;
	if (GetPrinterDevice(szPrinter, &hDevNames, &hDevMode))
	{
		__super::SelectPrinter(hDevNames, hDevMode);
		return TRUE;
	}

	TaskDialog(L"SelectPrinter failed!", L"can't open printer '%s'.", szPrinter);
	return FALSE;
}

// https://support.microsoft.com/en-us/help/166129/how-to-programmatically-print-to-a-non-default-printer-in-mfc
// static
// returns a DEVMODE and DEVNAMES for the printer name specified
BOOL CSampleApp::GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode)
{
	// if NULL is passed, then assume we are setting app object's
	// devmode and devnames
	if (phDevMode == NULL || phDevNames == NULL)
		return FALSE;

	// Open printer
	HANDLE hPrinter;
	if (OpenPrinter(pszPrinterName, &hPrinter, NULL) == FALSE)
		return FALSE;

	// obtain PRINTER_INFO_2 structure and close printer
	DWORD dwBytesReturned, dwBytesNeeded;
	GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);
	PRINTER_INFO_2* p2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR, dwBytesNeeded);
	if (GetPrinter(hPrinter, 2, (LPBYTE)p2, dwBytesNeeded, &dwBytesReturned) == 0) 
	{
		GlobalFree(p2);
		ClosePrinter(hPrinter);
		return FALSE;
	}
	ClosePrinter(hPrinter);

	// Allocate a global handle for DEVMODE
	HGLOBAL  hDevMode = GlobalAlloc(GHND, sizeof(*p2->pDevMode) + p2->pDevMode->dmDriverExtra);
	ASSERT(hDevMode);
	DEVMODE* pDevMode = (DEVMODE*)GlobalLock(hDevMode);
	ASSERT(pDevMode);

	// copy DEVMODE data from PRINTER_INFO_2::pDevMode
	memcpy(pDevMode, p2->pDevMode, sizeof(*p2->pDevMode) + p2->pDevMode->dmDriverExtra);
	GlobalUnlock(hDevMode);

	// Compute size of DEVNAMES structure from PRINTER_INFO_2's data
	DWORD drvNameLen = lstrlen(p2->pDriverName) + 1;		// driver name
	DWORD ptrNameLen = lstrlen(p2->pPrinterName) + 1;		// printer name
	DWORD porNameLen = lstrlen(p2->pPortName) + 1;			// port name

	// Allocate a global handle big enough to hold DEVNAMES.
	HGLOBAL hDevNames = GlobalAlloc(GHND,
		sizeof(DEVNAMES) +
		(drvNameLen + ptrNameLen + porNameLen) * sizeof(TCHAR));
	ASSERT(hDevNames);
	DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(hDevNames);
	ASSERT(pDevNames);

	// Copy the DEVNAMES information from PRINTER_INFO_2
	// tcOffset = TCHAR Offset into structure
	int tcOffset = sizeof(DEVNAMES) / sizeof(TCHAR);
	ASSERT(sizeof(DEVNAMES) == tcOffset * sizeof(TCHAR));

	pDevNames->wDriverOffset = tcOffset;
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pDriverName, drvNameLen * sizeof(TCHAR));
	tcOffset += drvNameLen;

	pDevNames->wDeviceOffset = tcOffset;
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pPrinterName, ptrNameLen * sizeof(TCHAR));
	tcOffset += ptrNameLen;

	pDevNames->wOutputOffset = tcOffset;
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pPortName, porNameLen * sizeof(TCHAR));
	pDevNames->wDefault = 0;

	GlobalUnlock(hDevNames);
	GlobalFree(p2);											// free PRINTER_INFO_2

	// set the new hDevMode and hDevNames
	*phDevMode = hDevMode;
	*phDevNames = hDevNames;
	return TRUE;
}

void CSampleApp::TaskDialog(PCWSTR szMainInstruction, PCWSTR szFormat, ...)
{
	va_list va;
	va_start(va, szFormat);

	WCHAR szContent[0x1000];
	::vswprintf_s(szContent, szFormat, va);
	::TaskDialog(0, 0, _T("TITLE"), szMainInstruction, szContent, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);

	va_end(va);
}

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CSampleApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSampleApp message handlers
