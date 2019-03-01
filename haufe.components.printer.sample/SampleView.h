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

class CSampleView : public CRichEditView
{
protected: // create from serialization only
	CSampleView();
	DECLARE_DYNCREATE(CSampleView)

// Attributes
public:
	CSampleDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSampleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bPrintDirect = FALSE;
	BOOL m_bStressTest = FALSE;
	CString m_csOutputPath;
	CString CreateOutputPath();
#ifdef DOCUMENTEVENT_LAST
	HANDLE m_hAmyuni = NULL;
	PSTR Company = AMYUNI_LICENCE_COMPANY;
	PSTR Code = AMYUNI_LICENCE_CODE;
#endif

// Generated message map functions
protected:
	afx_msg void OnFilePrint();
	afx_msg BOOL OnFilePrintPdf(UINT nId);
	afx_msg BOOL OnFilePrintPdfStress(UINT nId);
	afx_msg void OnFilePrintPreview();
	void OnUpdateCommand(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SampleView.cpp
inline CSampleDoc* CSampleView::GetDocument() const
   { return reinterpret_cast<CSampleDoc*>(m_pDocument); }
#endif
