
// DialogTestView.h : interface of the CDialogTestView class
//

#pragma once


class CDialogTestView : public CView
{
protected: // create from serialization only
	CDialogTestView();
	DECLARE_DYNCREATE(CDialogTestView)

// Attributes
public:
	CDialogTestDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CDialogTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DialogTestView.cpp
inline CDialogTestDoc* CDialogTestView::GetDocument() const
   { return reinterpret_cast<CDialogTestDoc*>(m_pDocument); }
#endif

