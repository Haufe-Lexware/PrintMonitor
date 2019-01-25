// SampleDoc.h : interface of the CSampleDoc class
#pragma once

class CSampleDoc : public CDocument
{
protected: // create from serialization only
	CSampleDoc();
	DECLARE_DYNCREATE(CSampleDoc)

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CSampleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void LoadRtfDocument();

// Generated message map functions
protected:
	void OnUpdateCommand(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};
