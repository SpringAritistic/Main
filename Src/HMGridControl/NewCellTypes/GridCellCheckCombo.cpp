//11/22/2018  4:37:13 PM
#include "stdafx.h"
#include "..\GridCtrl_src\GridCell.h"
#include "..\GridCtrl_src\GridCtrl.h"


#include "CheckComboBox.h"
#include "GridCellCheckCombo.h"

#include "..\tools\StrTool.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
_HM_GridControl_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CCheckComboEdit

CCheckComboEdit::CCheckComboEdit()
{
}

CCheckComboEdit::~CCheckComboEdit()
{
}

// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL CCheckComboEdit::PreTranslateMessage(MSG* pMsg) 
{
	// Make sure that the keystrokes continue to the appropriate handlers
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE;
	}	

	// Catch the Alt key so we don't choke if focus is going to an owner drawn button
	if (pMsg->message == WM_SYSCHAR)
		return TRUE;

	return CEdit::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CCheckComboEdit, CEdit)
	//{{AFX_MSG_MAP(CCheckComboEdit)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckComboEdit message handlers

void CCheckComboEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
    CInPlaceCheckList* pOwner = (CInPlaceCheckList*) GetOwner();  // This MUST be a CInPlaceCheckList
    if (pOwner)
        pOwner->EndEdit();	
}

void CCheckComboEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
		 nChar == VK_DOWN  || nChar == VK_UP   ||
		 nChar == VK_RIGHT || nChar == VK_LEFT) &&
		(GetKeyState(VK_CONTROL) < 0 && GetDlgCtrlID() == IDC_COMBOEDIT))
    {
        CWnd* pOwner = GetOwner();
        if (pOwner)
            pOwner->SendMessage(WM_KEYDOWN, nChar, nRepCnt+ (((DWORD)nFlags)<<16));
        return;
    }

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCheckComboEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE) 
	{
        CWnd* pOwner = GetOwner();
        if (pOwner)
            pOwner->SendMessage(WM_KEYUP, nChar, nRepCnt + (((DWORD)nFlags)<<16));
        return;
    }

	if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
    {
        CWnd* pOwner = GetOwner();
        if (pOwner)
            pOwner->SendMessage(WM_KEYUP, nChar, nRepCnt + (((DWORD)nFlags)<<16));
        return;
    }

	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}
void  HMGridCellCheckCombo::SetStyle(DWORD dwStyle)           { m_dwStyle = dwStyle; }
DWORD HMGridCellCheckCombo::GetStyle()                        { return m_dwStyle; }

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCheckList

CInPlaceCheckList::CInPlaceCheckList(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
                           int nRow, int nColumn, 
                           COLORREF crFore, COLORREF crBack,
						   std::vector<CString>& Items, CString sInitText, 
						   UINT nFirstChar)
{
    m_crForeClr = crFore;
    m_crBackClr = crBack;

	m_nNumLines = 4;
	//m_sInitText = sInitText;
 	m_nRow		= nRow;
 	m_nCol      = nColumn;
 	m_nLastChar = 0; 
	m_bExitOnArrows = FALSE; //(nFirstChar != VK_LBUTTON);	// If mouse click brought us here,


	// Create the combobox
 	DWORD dwComboStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|
 					     CBS_AUTOHSCROLL | dwStyle;
	int nHeight = rect.Height();
	rect.bottom = rect.bottom + m_nNumLines*nHeight + ::GetSystemMetrics(SM_CYHSCROLL);
	if (!Create(dwComboStyle, rect, pParent, nID)) return;

	SetMe();
	// Add the strings
	for (auto & item:Items) 
	{
		AddString(item);
	}
	SetFont(pParent->GetFont());
	SetItemHeight(-1, nHeight);
	if (false)
	{
		vector<int> height{ GetItemHeight(-1), GetItemHeight(0), GetItemHeight(1), GetItemHeight(2) };
		int a(0);
	}
    int nMaxLength = GetCorrectDropWidth();
    /*
    if (nMaxLength > rect.Width())
	    rect.right = rect.left + nMaxLength;
	// Resize the edit window and the drop down window
	MoveWindow(rect);
    */

	SetDroppedWidth(nMaxLength);

	SetHorizontalExtent(0); // no horz scrolling

	// Reset the initial text to m_sInitText
	if (::IsWindow(m_hWnd))
	{
		//if (SelectString(-1, m_sInitText) == CB_ERR)
		//	SetWindowText(m_sInitText);		// No text selected, so restore what was there before
		InitSelec(sInitText);
		m_sInitText = GetTitle();
		if (m_sInitText.IsEmpty())
			m_sInitText = sInitText;
		SetWindowText(m_sInitText);
	}

    ShowDropDown();

    // Subclass the combobox edit control if style includes CBS_DROPDOWN
    //if ((dwStyle & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST)
    //{
    //    m_edit.SubclassDlgItem(IDC_COMBOEDIT, this);
 	  //  SetFocus();
    //    switch (nFirstChar)
    //    {
    //        case VK_LBUTTON: 
    //        case VK_RETURN:   m_edit.SetSel((int)_tcslen(m_sInitText), -1); return;
    //        case VK_BACK:     m_edit.SetSel((int)_tcslen(m_sInitText), -1); break;
    //        case VK_DOWN: 
    //        case VK_UP:   
    //        case VK_RIGHT:
    //        case VK_LEFT:  
    //        case VK_NEXT:  
    //        case VK_PRIOR: 
    //        case VK_HOME:  
    //        case VK_END:      m_edit.SetSel(0,-1); return;
    //        default:          m_edit.SetSel(0,-1);
    //    }
    //    SendMessage(WM_CHAR, nFirstChar);
    //}
    //else
 	    SetFocus();
		//m_isAlreadyEndEdit = false;
}

CInPlaceCheckList::~CInPlaceCheckList()
{
	//m_isAlreadyEndEdit = true;
}

void CInPlaceCheckList::EndEdit()
{
	//if (m_isAlreadyEndEdit)
	//	return;
    CString str;
    if (::IsWindow(m_hWnd))
        GetWindowText(str);
 
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
	if (false)
	{
		UINT id = GetDlgCtrlID();
		int a(0);
	}

    CWnd* pOwner = GetOwner();
    if (IsWindow(pOwner->GetSafeHwnd()))
        pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
 
    // Close this window (PostNcDestroy will delete this)
    if (::IsWindow(m_hWnd))
        PostMessage(WM_CLOSE, 0, 0);
}

int CInPlaceCheckList::GetCorrectDropWidth()
{
    const int nMaxWidth = 200;  // don't let the box be bigger than this

    // Reset the dropped width
    int nNumEntries = GetCount();
    int nWidth = 0;
    CString str;

    CClientDC dc(this);
    int nSave = dc.SaveDC();
    dc.SelectObject(GetFont());

    int nScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
    for (int i = 0; i < nNumEntries; i++)
    {
        GetLBText(i, str);
        int nLength = dc.GetTextExtent(str).cx + nScrollWidth;
        nWidth = max(nWidth, nLength);
    }
    
    // Add margin space to the calculations
    nWidth += dc.GetTextExtent(_T("0")).cx;

    dc.RestoreDC(nSave);

    nWidth = min(nWidth, nMaxWidth);

    return nWidth;
    //SetDroppedWidth(nWidth);
}

/*
// Fix by Ray (raybie@Exabyte.COM)
void CInPlaceCheckList::OnSelendOK() 
{
    int iIndex = GetCurSel(); 
    if( iIndex != CB_ERR) 
    { 
        CString strLbText; 
        GetLBText( iIndex, strLbText); 
 
        if (!((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)) 
           m_edit.SetWindowText( strLbText); 
    } 
 
    GetParent()->SetFocus(); 	
}
*/

void CInPlaceCheckList::PostNcDestroy() 
{
	HMCheckComboBox::PostNcDestroy();

	delete this;
}

BEGIN_MESSAGE_MAP(CInPlaceCheckList, HMCheckComboBox)
	//{{AFX_MSG_MAP(CInPlaceCheckList)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_WM_GETDLGCODE()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
	//ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendOK)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInPlaceCheckList message handlers

UINT CInPlaceCheckList::OnGetDlgCode() 
{
    return DLGC_WANTALLKEYS;
}

void CInPlaceCheckList::OnDropdown() 
{
    SetDroppedWidth(GetCorrectDropWidth());
	//
	//int a(0);

}

void CInPlaceCheckList::OnKillFocus(CWnd* pNewWnd) 
{
		

	HMCheckComboBox::OnKillFocus(pNewWnd);

	if (pNewWnd && GetSafeHwnd() == pNewWnd->GetSafeHwnd())
        return;

    // Only end editing on change of focus if we're using the CBS_DROPDOWNLIST style
    if ((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
        EndEdit();

}

// If an arrow key (or associated) is pressed, then exit if
//  a) The Ctrl key was down, or
//  b) m_bExitOnArrows == TRUE
void CInPlaceCheckList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
		 nChar == VK_DOWN  || nChar == VK_UP   ||
		 nChar == VK_RIGHT || nChar == VK_LEFT) &&
		(m_bExitOnArrows || GetKeyState(VK_CONTROL) < 0))
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();
		return;
	}

	HMCheckComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Need to keep a lookout for Tabs, Esc and Returns.
void CInPlaceCheckList::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE) 
		SetWindowText(m_sInitText);	// restore previous text

	if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();	// This will destroy this window
		return;
	}

	HMCheckComboBox::OnKeyUp(nChar, nRepCnt, nFlags);
}

HBRUSH CInPlaceCheckList::CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/) 
{
    /*
    static CBrush brush(m_crBackClr);
    pDC->SetTextColor(m_crForeClr);
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH) brush.GetSafeHandle();
    */
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// HMGridCellCheckCombo 
/////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(HMGridCellCheckCombo, HMGridCell)

HMGridCellCheckCombo::HMGridCellCheckCombo() : HMGridCell()
{
    SetStyle(CBS_DROPDOWN);  // CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE, CBS_SORT
}

// Create a control to do the editing
BOOL HMGridCellCheckCombo::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    m_bEditing = TRUE;

    // CInPlaceCheckList auto-deletes itself
	CInPlaceCheckList* wnd = new CInPlaceCheckList(GetGrid(), rect, GetStyle(), nID, nRow, nCol,
		GetTextClr(), GetBackClr(), m_strOpts, GetText(), nChar);
	m_pEditWnd = wnd;
    return TRUE;
}

CWnd* HMGridCellCheckCombo::GetEditWnd() const
{
	if (m_pEditWnd && (m_pEditWnd->GetStyle() & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST)
	{
		CInPlaceCheckList* wnd = dynamic_cast<CInPlaceCheckList*>(m_pEditWnd);
		if (wnd)
			return &(wnd->m_edit);
	}

	return NULL;
}

CSize HMGridCellCheckCombo::GetCellExtent(CDC* pDC)
{
    CSize sizeScroll(GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CYHSCROLL));

	return HMGridCell::GetCellExtent(pDC) + sizeScroll;
}

// Cancel the editing.
void HMGridCellCheckCombo::EndEdit()
{
    if (m_pEditWnd)
        ((CInPlaceCheckList*)m_pEditWnd)->EndEdit();
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL HMGridCellCheckCombo::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
#ifdef _WIN32_WCE
	return HMGridCell::Draw(pDC, nRow, nCol, rect, bEraseBkgnd);
#else
    // Cell selected?
    //if ( !IsFixed() && IsFocused())
    if (GetGrid()->IsCellEditable(nRow, nCol) && !IsEditing())
    {
        // Get the size of the scroll box
        CSize sizeScroll(GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CYHSCROLL));

        // enough room to draw?
        if (sizeScroll.cy < rect.Width() && sizeScroll.cy < rect.Height())
        {
			EnsureExistVScroll(true);
            // Draw control at RHS of cell
            CRect ScrollRect = rect;
            ScrollRect.left   = rect.right - sizeScroll.cx;
            ScrollRect.top = rect.bottom - sizeScroll.cy;

            // Do the draw 
            pDC->DrawFrameControl(ScrollRect, DFC_SCROLL, DFCS_SCROLLDOWN);

            // Adjust the remaining space in the cell
            rect.right = ScrollRect.left;
        }
    }

    CString strTempText = GetText();
    //if (IsEditing())
    //    SetText(_T(""));
	if (IsEditing())
		SetText(strTempText);
    // drop through and complete the cell drawing using the base class' method
	BOOL bResult = HMGridCell::Draw(pDC, nRow, nCol, rect, bEraseBkgnd);

    //if (IsEditing())
    //    SetText(strTempText);

	return bResult;
#endif
}

// For setting the strings that will be displayed in the drop list
void HMGridCellCheckCombo::SetOptions(std::vector<CString>& ar)
{ 
	m_strOpts = ar;
	remove_if(m_strOpts.begin(), m_strOpts.end(), [](const CString& str){return str.IsEmpty(); });
}




_HM_GridControl_END
