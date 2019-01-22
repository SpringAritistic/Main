
// DialogTestView.cpp : implementation of the CDialogTestView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DialogTest.h"
#endif

#include "DialogTestDoc.h"
#include "DialogTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDialogTestView

IMPLEMENT_DYNCREATE(CDialogTestView, CView)

BEGIN_MESSAGE_MAP(CDialogTestView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CDialogTestView construction/destruction

CDialogTestView::CDialogTestView()
{
	// TODO: add construction code here

}

CDialogTestView::~CDialogTestView()
{
}

BOOL CDialogTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CDialogTestView drawing

void CDialogTestView::OnDraw(CDC* /*pDC*/)
{
	CDialogTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CDialogTestView printing

BOOL CDialogTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDialogTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDialogTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CDialogTestView diagnostics

#ifdef _DEBUG
void CDialogTestView::AssertValid() const
{
	CView::AssertValid();
}

void CDialogTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDialogTestDoc* CDialogTestView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDialogTestDoc)));
	return (CDialogTestDoc*)m_pDocument;
}
#endif //_DEBUG


// CDialogTestView message handlers
