//11/22/2018  4:37:13 PM
#include "stdafx.h"
#include "CheckComboBox.h"
#include "..\tools\StrTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;




#endif
_HM_GridControl_BEGIN

static WNDPROC m_pWndProc = 0;
static HMCheckComboBox *m_pComboBox = 0;


BEGIN_MESSAGE_MAP(HMCheckComboBox, CComboBox)
	//{{AFX_MSG_MAP(HMCheckComboBox)
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
	ON_MESSAGE(WM_GETTEXTLENGTH, OnGetTextLength)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropDown)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


//
// The subclassed COMBOLBOX message handler
//
extern "C" LRESULT FAR PASCAL ComboBoxListBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{

	switch (nMsg) {


	case WM_RBUTTONDOWN: {
		// If you want to select all/unselect all using the
		// right button, remove this ifdef. Personally, I don't really like it
#if FALSE

		if (m_pComboBox != 0) {
			INT nCount = m_pComboBox->GetCount();
			INT nSelCount = 0;

			for (INT i = 0; i < nCount; i++) {
				if (m_pComboBox->GetCheck(i))
					nSelCount++;
			}


			m_pComboBox->SelectAll(nSelCount != nCount);

			// Make sure to invalidate this window as well
			InvalidateRect(hWnd, 0, FALSE);
			m_pComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_pComboBox->m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)m_pComboBox->m_hWnd);

		}
#endif

		break;
	}

						 // Make the combobox always return -1 as the current selection. This
						 // causes the lpDrawItemStruct->itemID in DrawItem() to be -1
						 // when the always-visible-portion of the combo is drawn
	case LB_GETCURSEL: {
		return -1;
	}


	case WM_CHAR: {
		if (wParam == VK_SPACE) {
			// Get the current selection
			INT nIndex = CallWindowProcA(m_pWndProc, hWnd, LB_GETCURSEL, wParam, lParam);

			CRect rcItem;
			SendMessage(hWnd, LB_GETITEMRECT, nIndex, (LONG)(VOID *)&rcItem);
			InvalidateRect(hWnd, rcItem, FALSE);

			// Invert the check mark
			m_pComboBox->SetCheck(nIndex, !m_pComboBox->GetCheck(nIndex));

			// Notify that selection has changed
			m_pComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_pComboBox->m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)m_pComboBox->m_hWnd);
			return 0;
		}

		break;
	}


	case WM_LBUTTONDOWN: {

		CRect rcClient;
		GetClientRect(hWnd, rcClient);

		CPoint pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);


		if (PtInRect(rcClient, pt)) {
			INT nItemHeight = SendMessage(hWnd, LB_GETITEMHEIGHT, 0, 0);
			INT nTopIndex = SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);

			// Compute which index to check/uncheck
			INT nIndex = nTopIndex + pt.y / nItemHeight;

			CRect rcItem;
			SendMessage(hWnd, LB_GETITEMRECT, nIndex, (LONG)(VOID *)&rcItem);

			if (PtInRect(rcItem, pt)) {
				// Invalidate this window
				InvalidateRect(hWnd, rcItem, FALSE);
				m_pComboBox->SetCheck(nIndex, !m_pComboBox->GetCheck(nIndex));

				// Notify that selection has changed
				m_pComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_pComboBox->m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)m_pComboBox->m_hWnd);


			}
		}

		// Do the default handling now (such as close the popup
		// window when clicked outside)
		break;
	}

	case WM_LBUTTONUP: {
		// Don't do anything here. This causes the combobox popup
		// windows to remain open after a selection has been made
		return 0;
	}
	}

	return CallWindowProc(m_pWndProc, hWnd, nMsg, wParam, lParam);
}





HMCheckComboBox::HMCheckComboBox()
{
	m_hListBox = 0;
	m_bTextUpdated = FALSE;
	m_bItemHeightSet = FALSE;
}


HMCheckComboBox::~HMCheckComboBox()
{
	//if (m_pComboBox==this)
	//m_pComboBox = 0;//
}


BOOL HMCheckComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{

	// Remove the CBS_SIMPLE and CBS_DROPDOWN styles and add the one I'm designed for
	dwStyle &= ~0xF;
	dwStyle |= CBS_DROPDOWNLIST;

	// Make sure to use the CBS_OWNERDRAWVARIABLE style
	dwStyle |= CBS_OWNERDRAWVARIABLE;

	// Use default strings. We need the itemdata to store checkmarks
	dwStyle |= CBS_HASSTRINGS;
	//BOOL result = CComboBox::Create(dwStyle, rect, pParentWnd, nID);

	return CComboBox::Create(dwStyle, rect, pParentWnd, nID);
}


LRESULT HMCheckComboBox::OnCtlColorListBox(WPARAM wParam, LPARAM lParam)
{


	// If the listbox hasn't been subclassed yet, do so...
	if (m_hListBox == 0) {
		HWND hWnd = (HWND)lParam;

		if (hWnd != 0 && hWnd != m_hWnd) {
			// Save the listbox handle
			m_hListBox = hWnd;

			// Do the subclassing
			m_pWndProc = (WNDPROC)GetWindowLong(m_hListBox, GWL_WNDPROC);
			//添加选择项
			for (int item : m_vecSelect)
				SetItemData(item, TRUE);
			if (m_vecSelect.size() > 0)
				m_bTextUpdated = FALSE;

			SetWindowLong(m_hListBox, GWL_WNDPROC, (LONG)ComboBoxListBoxProc);
		}
	}


	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}


void HMCheckComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HDC dc = lpDrawItemStruct->hDC;

	CRect rcBitmap = lpDrawItemStruct->rcItem;
	CRect rcText = lpDrawItemStruct->rcItem;

	CString strText;

	// 0 - No check, 1 - Empty check, 2 - Checked
	INT nCheck = 0;

	// Check if we are drawing the static portion of the combobox
	if ((LONG)lpDrawItemStruct->itemID < 0) {
		// Make sure the m_strText member is updated
		RecalcText();

		// Get the text
		strText = m_strText;

		// Don't draw any boxes on this item
		nCheck = 0;
	}

	// Otherwise it is one of the items
	else {
		GetLBText(lpDrawItemStruct->itemID, strText);
		nCheck = 1 + (GetItemData(lpDrawItemStruct->itemID) != 0);

		TEXTMETRIC metrics;
		GetTextMetrics(dc, &metrics);

		rcBitmap.left = 0;
		rcBitmap.right = rcBitmap.left + metrics.tmHeight + metrics.tmExternalLeading + 6;
		rcBitmap.top += 1;
		rcBitmap.bottom -= 1;

		rcText.left = rcBitmap.right;
	}



	if (nCheck > 0) {
		SetBkColor(dc, GetSysColor(COLOR_WINDOW));
		SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));

		UINT nState = DFCS_BUTTONCHECK;

		if (nCheck > 1)
			nState |= DFCS_CHECKED;

		// Draw the checkmark using DrawFrameControl
		DrawFrameControl(dc, rcBitmap, DFC_BUTTON, nState);
	}

	if (lpDrawItemStruct->itemState & ODS_SELECTED) {
		SetBkColor(dc, GetSysColor(COLOR_HIGHLIGHT));
		SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else {
		SetBkColor(dc, GetSysColor(COLOR_WINDOW));
		SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
	}

	// Erase and draw
	ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rcText, 0, 0, 0);
	DrawText(dc, ' ' + strText, strText.GetLength() + 1, &rcText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

	if ((lpDrawItemStruct->itemState & (ODS_FOCUS | ODS_SELECTED)) == (ODS_FOCUS | ODS_SELECTED))
		DrawFocusRect(dc, &rcText);

}


void HMCheckComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CClientDC dc(this);
	CFont *pFont = dc.SelectObject(GetFont());

	if (pFont != 0) {

		TEXTMETRIC metrics;
		dc.GetTextMetrics(&metrics);

		lpMeasureItemStruct->itemHeight = metrics.tmHeight + metrics.tmExternalLeading;

		// An extra height of 2 looks good I think. 
		// Otherwise the list looks a bit crowded...
		lpMeasureItemStruct->itemHeight += 2;


		// This is needed since the WM_MEASUREITEM message is sent before
		// MFC hooks everything up if used in i dialog. So adjust the
		// static portion of the combo box now
		if (!m_bItemHeightSet) {
			m_bItemHeightSet = TRUE;
			SetItemHeight(-1, lpMeasureItemStruct->itemHeight);
		}

		dc.SelectObject(pFont);
	}
}
void HMCheckComboBox::SetMe()
{
	OnDropDown();
}

//
// Make sure the combobox window handle is updated since
// there may be many HMCheckComboBox windows active
//
void HMCheckComboBox::OnDropDown()
{
	m_pComboBox = this;

}


//
// Selects/unselects all items in the list
//
void HMCheckComboBox::SelectAll(BOOL bCheck)
{
	INT nCount = GetCount();

	for (INT i = 0; i < nCount; i++)
		SetCheck(i, bCheck);

}
// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL HMCheckComboBox::PreTranslateMessage(MSG* pMsg)
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

	return CComboBox::PreTranslateMessage(pMsg);
}

//
// By adding this message handler, we may use CWnd::GetText()
//
LRESULT HMCheckComboBox::OnGetText(WPARAM wParam, LPARAM lParam)
{
	// Make sure the text is updated
	RecalcText();

	if (lParam == 0)
		return 0;

	// Copy the 'fake' window text
	lstrcpynW((LPWSTR)lParam, (LPCWSTR)(LPCSTR)m_strText, (INT)wParam);
	return m_strText.GetLength();
}


//
// By adding this message handler, we may use CWnd::GetTextLength()
//
LRESULT HMCheckComboBox::OnGetTextLength(WPARAM, LPARAM)
{
	// Make sure the text is updated
	RecalcText();
	return m_strText.GetLength();
}

CString HMCheckComboBox::GetSeperatorStr()const
{
	// Get the list separator
	TCHAR szBuffer[10] = { 0 };
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szBuffer, sizeof(szBuffer));
	CString strSeparator = szBuffer;

	// If none found, the the ';'
	if (strSeparator.GetLength() == 0)
		strSeparator = ',';

	// Trim extra spaces
	strSeparator.TrimRight();

	// And one...
	//strSeparator += ' ';
	return strSeparator;
}

void HMCheckComboBox::InitSelec(const CString& str)
{
	if (str.IsEmpty())
		return;
	std::vector<CString>vecStr;
	split_string(vecStr, str, GetSeperatorStr());
	m_vecSelect = FindIndexs(vecStr,GetOptions());
}
vector<CString>HMCheckComboBox::GetOptions() const
{
	vector<CString>vec;
	for (INT i = 0; i < GetCount(); i++) {
		CString strItem;
		GetLBText(i, strItem);
		//一般情况下，不会为空，判断应该放到外部
		//if (!strItem.IsEmpty())
			vec.push_back(strItem);
	}
	return vec;
}

//
// This routine steps thru all the items and builds
// a string containing the checked items
//
CString HMCheckComboBox::GetTitle() const
{
	CString strText;

	// Get the list count
	INT nCount = GetCount();
	CString strSeparator = GetSeperatorStr();
	for (size_t i : m_vecSelect)
	{
		CString strItem;
		GetLBText(i, strItem);

		if (!strText.IsEmpty())
			strText += strSeparator;

		strText += strItem;
	}
	return strText;
}

void HMCheckComboBox::RecalcText()
{
	if (!m_bTextUpdated) {
		CString strText;

		// Get the list count
		INT nCount = GetCount();



		CString strSeparator = GetSeperatorStr();

		for (INT i = 0; i < nCount; i++) {

			if (GetItemData(i)) {
				CString strItem;
				GetLBText(i, strItem);

				if (!strText.IsEmpty())
					strText += strSeparator;

				strText += strItem;
			}
		}

		// Reset the text
		m_strText = strText;

		m_bTextUpdated = TRUE;
	}
}

INT HMCheckComboBox::SetCheck(INT nIndex, BOOL bFlag)
{
	INT nResult = SetItemData(nIndex, bFlag);

	if (nResult < 0)
		return nResult;

	// Signal that the text need updating
	m_bTextUpdated = FALSE;

	// Redraw the window
	Invalidate(FALSE);

	return nResult;
}

BOOL HMCheckComboBox::GetCheck(INT nIndex)
{
	return GetItemData(nIndex);
}


void HMCheckComboBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	// Don't do anything here. This causes the combobox popup
	// windows to remain open after a selection has been made


	CComboBox::OnLButtonUp(nFlags, point);
}


void HMCheckComboBox::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	// If you want to select all/unselect all using the
	// right button, remove this ifdef. Personally, I don't really like it
#if FALSE


		INT nCount =GetCount();
		INT nSelCount = 0;

		for (INT i = 0; i < nCount; i++) {
			if (GetCheck(i))
				nSelCount++;



		SelectAll(nSelCount != nCount);

		// Make sure to invalidate this window as well
		InvalidateRect( 0, FALSE);
		GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)m_hWnd);

	}
#endif



	CComboBox::OnRButtonDown(nFlags, point);
}


void HMCheckComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if(nChar == VK_SPACE) {
		// Get the current selection
		//INT nIndex = SendMessage(LB_GETCURSEL, nChar, nFlags);
		INT nIndex = GetCurSel();

		CRect rcItem;
		//SendMessage(LB_GETITEMRECT, nIndex, (LONG)(VOID *)&rcItem);
		//GetItemRect(nIndex);
		((CListBox*)GetWindow(GW_CHILD))->GetItemRect(nIndex,&rcItem);
		InvalidateRect( rcItem, FALSE);

		// Invert the check mark
		SetCheck(nIndex, !GetCheck(nIndex));
		// Notify that selection has changed
		GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)m_hWnd);
	}
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}


void HMCheckComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	//CRect rcClient;
	//GetClientRect( rcClient);

	//CPoint point;

	//if (PtInRect(rcClient, point)) {
	//	INT nItemHeight = GetItemHeight(0);
	//	INT nTopIndex = GetTopIndex();
	//	
	//	// Compute which index to check/uncheck
	//	INT nIndex = nTopIndex + point.y / nItemHeight;

	//	CRect rcItem;
	//	SendMessage(hWnd, LB_GETITEMRECT, nIndex, (LONG)(VOID *)&rcItem);
	//	GetItemData(nIndex);
	//	if (PtInRect(rcItem, point)) {
	//		// Invalidate this window
	//		InvalidateRect(hWnd, rcItem, FALSE);
	//		m_pComboBox->SetCheck(nIndex, !m_pComboBox->GetCheck(nIndex));

	//		// Notify that selection has changed
	//		m_pComboBox->GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetWindowLong(m_pComboBox->m_hWnd, GWL_ID), CBN_SELCHANGE), (LPARAM)m_pComboBox->m_hWnd);


	//	}
	//}
	// Do the default handling now (such as close the popup
	// window when clicked outside)
	CComboBox::OnLButtonDown(nFlags, point);
}
_HM_GridControl_END
