// SampleView.cpp : implementation of the CSampleView class
#include "stdafx.h"
#include "Sample.h"
#include "SampleDoc.h"
#include "SampleView.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSampleView
IMPLEMENT_DYNCREATE(CSampleView, CRichEditView)

BEGIN_MESSAGE_MAP(CSampleView, CRichEditView)
	// Standard printing commands
	ON_COMMAND_EX(ID_FILE_GHOSTSCRIPTPDF, &OnFilePrintPdf)
	ON_COMMAND_EX(ID_FILE_GHOSTSCRIPTPDF_STRESS, &OnFilePrintPdfStress)
	ON_COMMAND_EX(ID_FILE_AMYUNIPDF, &OnFilePrintPdf)
	ON_COMMAND_EX(ID_FILE_AMYUNIPDF_STRESS, &OnFilePrintPdfStress)
	ON_COMMAND(ID_FILE_PRINT, &OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_AMYUNIPDF, ID_FILE_AMYUNIPDF_STRESS, OnUpdateCommand)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateCommand)
END_MESSAGE_MAP()

// CSampleView construction/destruction
CSampleView::CSampleView()
{
	// TODO: add construction code here
}

CSampleView::~CSampleView()
{
}

BOOL CSampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bPreCreateWindow = CRichEditView::PreCreateWindow(cs);
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	return bPreCreateWindow;
}

// CSampleView printing
BOOL CSampleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->m_pPD->m_pd.Flags &= ~PD_NOSELECTION;
	pInfo->m_pPD->m_pd.Flags &= ~PD_NOPAGENUMS;
	pInfo->m_pPD->m_pd.Flags |= PD_PAGENUMS;
	pInfo->m_pPD->m_pd.nMinPage = 1;
	pInfo->m_pPD->m_pd.nMaxPage = 8;

	m_csOutputPath = CreateOutputPath();
	pInfo->m_bDirect = m_bPrintDirect;						// bypasse Print Dialog				

	return DoPreparePrinting(pInfo);						// default preparation
}

void CSampleView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	if (pInfo->m_lpUserData)
	{
		ASSERT(pInfo->m_nJobNumber > 0);
		ASSERT(pInfo->m_pPD->GetDeviceName() == COMPANY_NAME L" PDF-Export 6");

		VERIFY(PrintMonitor::SetJobOptions(pInfo->m_nJobNumber, PrintMonitor::Default, m_csOutputPath));
	//	VERIFY(PrintMonitor::SetJobOptions(pInfo->m_nJobNumber, PrintMonitor::PDFA, m_csOutputPath));

		pInfo->m_lpUserData = NULL;
	}

	__super::OnPrepareDC(pDC, pInfo);
}

void CSampleView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	ATLASSERT(pInfo->m_nJobNumber == - 1);
	__super::OnBeginPrinting(pDC, pInfo);

	if (pInfo->m_pPD->GetDeviceName() == COMPANY_NAME L" PDF-Export 6")
	{
		if (!pInfo->m_bPreview && pInfo->m_bDirect)
		{
			pInfo->m_lpUserData = (LPVOID)1;
		}
	}
#ifdef DOCUMENTEVENT_LAST
	else if (pInfo->m_pPD->GetDeviceName() == COMPANY_NAME L" PDF-Export 5.5")
	{
		m_hAmyuni = ::PDFDriverInit(COMPANY_NAME " PDF-Export 5.5");
		if (m_hAmyuni)
		{
			m_csOutputPath = CreateOutputPath();

			ATLVERIFY(::EnablePrinter(m_hAmyuni, Company, Code));
			DWORD dwOptions = (m_bPrintDirect ? (NoPrompt | UseFileName) : 0) | EmbedFonts | FullEmbed | AutoCompression;

			ATLVERIFY(::SetDefaultFileNameW(m_hAmyuni, (PCWSTR)m_csOutputPath));
			::SetFileNameOptions(m_hAmyuni, dwOptions);
			
			::SetDefaultConfigEx(m_hAmyuni);
		}
		else
		{
			CSampleApp::TaskDialog(L"PDFDriverInit failed!", L"can't initialize Amyuni PDF printer.");
		}
	}
#endif
}

void CSampleView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	__super::OnEndPrinting(pDC, pInfo);

#ifdef DOCUMENTEVENT_LAST
	if (m_hAmyuni)
	{
	//	::DriverEnd(m_hAmyuni);
		m_hAmyuni = NULL;
	}
#endif
	if (!m_bStressTest && pInfo->m_bDirect && pInfo->m_bContinuePrinting)
	{
		ASSERT(!pInfo->m_bPreview);
		if (pInfo->m_pPD->GetDeviceName() == COMPANY_NAME L" PDF-Export 6")
		{
			Log.Info(__FUNCTION__ " waiting..");
			ATLVERIFY(PrintMonitor::WaitForJob(pInfo->m_nJobNumber));
			Log.Info(__FUNCTION__ " done.");
		
			ASSERT(::PathFileExists(m_csOutputPath));
			::ShellExecute(m_hWnd, _T("open"), m_csOutputPath, NULL, NULL, SW_SHOWNORMAL);
		}
	}

	m_csOutputPath.Empty();
}

// CSampleView diagnostics
#ifdef _DEBUG
void CSampleView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CSampleView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CSampleDoc* CSampleView::GetDocument() const				// non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSampleDoc)));
	return (CSampleDoc*)m_pDocument;
}
#endif //_DEBUG

CString CSampleView::CreateOutputPath()
{
	ATL::CTime now = ATL::CTime::GetCurrentTime();
	CString csNow = now.Format(_T("%y%m%d%H%M%S"));

	TCHAR szPath[_MAX_PATH];
	::SHGetSpecialFolderPath(m_hWnd, szPath, CSIDL_DESKTOP, FALSE);
	::PathRemoveFileSpec(szPath);

	static DWORD dw = 0;
	CString csOutput;
	csOutput.Format(_T("%s\\Downloads\\test-(%s.%d).pdf"), szPath, (PCTSTR)csNow, dw++);
	return csOutput;
}

// CSampleView message handlers
void CSampleView::OnFilePrint()
{
	m_bPrintDirect = FALSE;
	m_bStressTest = FALSE;
	__super::OnFilePrint();
}

BOOL CSampleView::OnFilePrintPdf(UINT nId)
{
	if (theApp.SelectPrinter(nId == ID_FILE_GHOSTSCRIPTPDF ? _T(COMPANY_NAME " PDF-Export 6") : _T(COMPANY_NAME " PDF-Export 5.5")))
	{
		m_bPrintDirect = TRUE;
		m_bStressTest = FALSE;
		__super::OnFilePrint();
	}
	
	return TRUE;
}

BOOL CSampleView::OnFilePrintPdfStress(UINT nId)
{
	if (theApp.SelectPrinter(nId == ID_FILE_GHOSTSCRIPTPDF_STRESS ? _T(COMPANY_NAME " PDF-Export 6") : _T(COMPANY_NAME " PDF-Export 5.5")))
	{
		m_bPrintDirect = TRUE;
		m_bStressTest = TRUE;
		::ShellExecute(NULL, _T("open"), _T("printui"), nId == ID_FILE_GHOSTSCRIPTPDF_STRESS ? _T("/o /n\"" COMPANY_NAME " PDF-Export 6\"") : _T("/o /n\"" COMPANY_NAME " PDF-Export 5.5\""), NULL, SW_SHOW);

		for (int i = 0; i < 10; i++)
		{
			__super::OnFilePrint();
		}
	}
	
	return TRUE;
}

void CSampleView::OnFilePrintPreview()
{
	m_bPrintDirect = FALSE;
	m_bStressTest = FALSE;
	__super::OnFilePrintPreview();
}

void CSampleView::OnUpdateCommand(CCmdUI *pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
#ifdef DOCUMENTEVENT_LAST
#else
	case ID_FILE_AMYUNIPDF:
	case ID_FILE_AMYUNIPDF_STRESS:
		pCmdUI->Enable(FALSE);
		break;
#endif
	case ID_EDIT_PASTE:
		pCmdUI->Enable(FALSE);
		break;
	}
}
