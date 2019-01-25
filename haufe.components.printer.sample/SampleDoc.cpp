// SampleDoc.cpp : implementation of the CSampleDoc class
#include "stdafx.h"
#include "SampleDoc.h"
#include "SampleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSampleDoc
IMPLEMENT_DYNCREATE(CSampleDoc, CDocument)

BEGIN_MESSAGE_MAP(CSampleDoc, CDocument)
//	ON_COMMAND(ID_FILE_SAVE, &CDocument::OnFileSave)
//	ON_COMMAND(ID_FILE_SAVE_AS, &CDocument::OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateCommand)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateCommand)
END_MESSAGE_MAP()

// CSampleDoc construction/destruction
CSampleDoc::CSampleDoc()
{
	// TODO: add one-time construction code here
}

CSampleDoc::~CSampleDoc()
{
}

static PCSTR GetRtfResource(LONG &dwSizeofResource)
{
	bool bControl = ::GetKeyState(VK_CONTROL) < 0;
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
	HRSRC hFindResource = ::FindResource(hInstance, bControl ? _T("2.RTF") : _T("1.RTF"), _T("RTF"));

	if (hFindResource)
	{
		HGLOBAL hLoadResource = ::LoadResource(hInstance, hFindResource);
		dwSizeofResource = ::SizeofResource(hInstance, hFindResource);
		ATLASSERT(hLoadResource);

		if (hLoadResource)
		{
			LPVOID pLockResource = ::LockResource(hLoadResource);
			ATLASSERT(pLockResource);
			ATLASSERT(::SizeofResource(hInstance, hFindResource) == ::strlen((PCSTR)pLockResource) + 1);
			return (PCSTR)pLockResource;
		}
	}

	return NULL;
}

struct Data
{
	LONG dwData;
	PCSTR szData;
};

static DWORD CALLBACK ReadRtf(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	Data *pData = (Data*)dwCookie;
	*pcb = 0;

	if (cb > pData->dwData)
	{
		cb = pData->dwData;
	}

	if (cb)
	{
		::CopyMemory(pbBuff, pData->szData, cb);
		pData->szData += cb;
		pData->dwData -= cb;
		*pcb = cb;
	}
	
	return 0;
}

void CSampleDoc::LoadRtfDocument()
{
	POSITION pos = GetFirstViewPosition();
	if (CView *pView = GetNextView(pos))
	{
		ASSERT(pView->IsKindOf(RUNTIME_CLASS(CSampleView)));
		if (CSampleView *pSampleView = DYNAMIC_DOWNCAST(CSampleView, pView))
		{
			Data data;
			data.szData = GetRtfResource(data.dwData);

			EDITSTREAM es = { 0 };
			es.dwCookie = (DWORD_PTR)&data;
			es.pfnCallback = ReadRtf;

			CRichEditCtrl &RichEditCtrl = pSampleView->GetRichEditCtrl();
			RichEditCtrl.StreamIn(SF_RTF, es);
			RichEditCtrl.SetReadOnly();

			pSampleView->SetMargins(CRect(640, 320, 960, 320));						// preview
		
			CEdit *pEdit = (CEdit*)pSampleView;										// view
			pEdit->SetMargins(64, 64);
		}
	}
}

BOOL CSampleDoc::OnNewDocument()
{
	if (CDocument::OnNewDocument())
	{
		LoadRtfDocument();
		SetTitle(_T("Lexware PDF-Export Sample"));

		return TRUE;
	}

	return FALSE;
}

// CSampleDoc serialization
void CSampleDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

// CSampleDoc diagnostics
#ifdef _DEBUG
void CSampleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSampleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CSampleDoc commands
void CSampleDoc::OnUpdateCommand(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
