
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DialogTest.h"

#include "MainFrm.h"
#include "excelDlg.h"
//#include "vector"
//#include "Matrix.h"
#include "Test/TestAll.h"
#include "GridTab.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_TEST_EXCEL, &CMainFrame::OnTestExcel)
	ON_COMMAND(ID_TEST_TAB, &CMainFrame::OnTestTab)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers



void CMainFrame::OnTestExcel()
{
	// TODO: Add your command handler code here



	TestAll();

	excelDlg dlg;
	dlg.DoModal();

	//int rows(3), columns(4);
	//int A[3][4];
	//int(*p)[4] = A;

	//int *C = new int[rows*columns];
	//int **D = &C;
	//using namespace std;
	//vector<CString>vecStr,vecStrD;
	//for (int i = 0; i < rows; i++)
	//{
	//	CString str,strD;
	//	bool isFirst = true;
	//	for (int j = 0; j < columns; j++)
	//	{
	//		//*(*(A + i) + j) = i*columns + j;
	//		*(*(p + i) + j) = i*columns + j;
	//		//A[i][j] = i*columns + j+1;
	//		C[i*columns + j] = i*columns + j + 1;

	//		if (j == columns - 1 && i<rows-1 &&isFirst)
	//		{
	//			D[i + 1] = &C[i*columns + j + 1];
	//			isFirst = false;
	//		}
	//		CString mystr, mystrD;
	//		mystr.Format("%d:[%d,%d]=%d;", &C[i*columns + j], i, j, C[i*columns + j]);
	//		mystrD.Format("%d:[%d,%d]=%d;", &D[i][j],i, j, D[i][j]);
	//		str += mystr;
	//		strD += mystrD;

	//	}
	//	vecStr.push_back(str);
	//	vecStrD.push_back(strD);
	//}
	//int i = 2;
	//CString vecStrTemp;
	//for (int j = 0; j < columns*(rows - i); j++)
	//{
	//	CString mystr, mystrD1;
	//	mystrD1.Format("%d:[%d,%d]=%d;", &D[i][j], i, j, D[i][j]);
	//	vecStrTemp += mystrD1;
	//}
	//

	//int **B = new int *[rows];
	//for (int i = 0; i < rows; i++)
	//	B[i] = new int[columns];
	//for (int i = 0; i < rows; i++)
	//{
	//	for (int j = 0; j < columns; j++)
	//	{
	//		//*(*(A + i) + j) = i*columns + j;
	//		B[i][j] = i*columns + j+1;
	//		*(*(p + i) + j) = B[i][j];
	//		//*(*(p + i) + j) = i*columns + j;

	//		//A[i][j] = i*columns + j+1;
	//	}
	//}

}


void CMainFrame::OnTestTab()
{
	// TODO: Add your command handler code here
	GridTab tab(this);
	tab.DoModal();

}
