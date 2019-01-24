//11/22/2018  4:37:13 PM
#include "stdafx.h"

#include "..\GridCtrl_src\GridCell.h"
#include "..\GridCtrl_src\GridCtrl.h"

#include "GridCellDateTime.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
_HM_GridControl_BEGIN
//////////////////////////////////////////////////////////////////////
// HMGridCellDateTime

IMPLEMENT_DYNCREATE(HMGridCellDateTime, HMGridCell)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HMGridCellDateTime::HMGridCellDateTime() : HMGridCell()
{
	m_dwStyle = 0;
	m_cTime   = CTime::GetCurrentTime();
}

HMGridCellDateTime::HMGridCellDateTime(DWORD dwStyle) : HMGridCell()
{
	Init(dwStyle);
}

HMGridCellDateTime::~HMGridCellDateTime()
{
}

CSize HMGridCellDateTime::GetCellExtent(CDC* pDC)
{    
    CSize sizeScroll (GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CYHSCROLL));	
    CSize sizeCell (HMGridCell::GetCellExtent(pDC));	
    sizeCell.cx += sizeScroll.cx;	
    sizeCell.cy = max(sizeCell.cy,sizeScroll.cy);	
    return sizeCell;
}

BOOL HMGridCellDateTime::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, 
							 UINT nID, UINT nChar)
{
	m_bEditing = TRUE;

	// HMInPlaceDateTime auto-deletes itself
	m_pEditWnd = new HMInPlaceDateTime(GetGrid(), rect,
		m_dwStyle|DTS_UPDOWN, nID, nRow, nCol, 
		GetTextClr(), GetBackClr(), GetTime(), nChar);
	return TRUE;
}

CWnd* HMGridCellDateTime::GetEditWnd() const
{
	return m_pEditWnd;
}

void HMGridCellDateTime::EndEdit()
{
	if (m_pEditWnd) ((HMInPlaceDateTime*)m_pEditWnd)->EndEdit();
}

void HMGridCellDateTime::Init(DWORD dwStyle)
{
	m_dwStyle = dwStyle;

	SetTime(CTime::GetCurrentTime());

	SetFormat(DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX
#ifndef _WIN32_WCE
		|DT_END_ELLIPSIS
#endif
		);
}

// Should be changed to use locale settings
void HMGridCellDateTime::SetTime(CTime time)
{
	m_cTime = time;

	if (DTS_TIMEFORMAT == m_dwStyle) 
	{
#ifdef _WIN32_WCE
		CString strTemp;
		strTemp.Format(_T("%02d:%02d:%02d"), 
			           m_cTime.GetHour(), m_cTime.GetMinute(), m_cTime.GetSecond());
		SetText(strTemp);
#else

//		SetText(m_cTime.Format(_T("%H:%M:%S")));
    	SetText(m_cTime.Format(_T("%X")));
#endif
	}
	else if (DTS_SHORTDATEFORMAT == m_dwStyle) 
	{
#ifdef _WIN32_WCE
		CString strTemp;
		strTemp.Format(_T("%02d/%02d/%02d"), 
			           m_cTime.GetMonth(), m_cTime.GetDay(), m_cTime.GetYear());
		SetText(strTemp);
#else
//		SetText(m_cTime.Format(("%d/%m/%Y")));
		SetText(m_cTime.Format(("%x")));
#endif
	}
}
CTime* HMGridCellDateTime::GetTime() { return &m_cTime; };

/////////////////////////////////////////////////////////////////////////////
// HMInPlaceDateTime

HMInPlaceDateTime::HMInPlaceDateTime(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
								   int nRow, int nColumn, 
								   COLORREF crFore, COLORREF crBack,
								   CTime* pcTime,
								   UINT nFirstChar)
{
	m_crForeClr     = crFore;
	m_crBackClr     = crBack;
	m_nRow          = nRow;
	m_nCol          = nColumn;
	m_nLastChar     = 0; 
	m_bExitOnArrows = FALSE;
	m_pcTime        = pcTime;

	DWORD dwStl = WS_BORDER|WS_VISIBLE|WS_CHILD|dwStyle;

	if (!Create(dwStl, rect, pParent, nID)) {
		return;
	}

	SetTime(m_pcTime);

	SetFont(pParent->GetFont());
	SetFocus();

	switch (nFirstChar) 
	{
		case VK_LBUTTON: 
		case VK_RETURN: return;
		case VK_BACK:   break;
		case VK_DOWN: 
		case VK_UP:   
		case VK_RIGHT:
		case VK_LEFT:  
		case VK_NEXT:  
		case VK_PRIOR: 
		case VK_HOME:  
		case VK_END:    return;
		default:        break;
	}
	SendMessage(WM_CHAR, nFirstChar);
}

HMInPlaceDateTime::~HMInPlaceDateTime()
{
}

void HMInPlaceDateTime::EndEdit()
{
	CString str;
	if (::IsWindow(m_hWnd)) 
	{
		GetWindowText(str);
		GetTime(*m_pcTime);
	}

	// Send Notification to parent
	GV_DISPINFO dispinfo;

	dispinfo.hdr.hwndFrom = GetSafeHwnd();
	dispinfo.hdr.idFrom   = GetDlgCtrlID();
	dispinfo.hdr.code     = GVN_ENDLABELEDIT;

	dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM;
	dispinfo.item.row     = m_nRow;
	dispinfo.item.col     = m_nCol;
	dispinfo.item.strText = str;
	dispinfo.item.lParam  = (LPARAM) m_nLastChar; 

	CWnd* pOwner = GetOwner();
	if (IsWindow(pOwner->GetSafeHwnd())) {
		pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo);
	}

	// Close this window (PostNcDestroy will delete this)
	if (::IsWindow(m_hWnd)) {
		PostMessage(WM_CLOSE, 0, 0);
	}
}

void HMInPlaceDateTime::PostNcDestroy() 
{
	CDateTimeCtrl::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(HMInPlaceDateTime, CDateTimeCtrl)
	//{{AFX_MSG_MAP(HMInPlaceDateTime)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// HMInPlaceDateTime message handlers

void HMInPlaceDateTime::OnKillFocus(CWnd* pNewWnd) 
{
	CDateTimeCtrl::OnKillFocus(pNewWnd);

	if (GetSafeHwnd() == pNewWnd->GetSafeHwnd()) {
		return;
	}
	EndEdit();
}

UINT HMInPlaceDateTime::OnGetDlgCode() 
{
	return DLGC_WANTALLKEYS;
}

void HMInPlaceDateTime::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (( nChar == VK_PRIOR || nChar == VK_NEXT ||
		nChar == VK_DOWN  || nChar == VK_UP   ||
		nChar == VK_RIGHT || nChar == VK_LEFT) &&
		(m_bExitOnArrows  || GetKeyState(VK_CONTROL) < 0))
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();
		return;
	}

	CDateTimeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void HMInPlaceDateTime::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();    // This will destroy this window
		return;
	}

	CDateTimeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}
_HM_GridControl_END
