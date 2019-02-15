//11/22/2018  4:37:13 PM
#include "stdafx.h"
#include "TCHAR.h"
#include "InPlaceEdit.h"
#include "GridCtrl.h"
#include "..\Tools\Tool.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
_HM_GridControl_BEGIN
/////////////////////////////////////////////////////////////////////////////
// HMInPlaceEdit

HMInPlaceEdit::HMInPlaceEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
int nRow, int nColumn, CString sInitText,
UINT nFirstChar)
{
	m_sInitText = sInitText;
	m_nRow = nRow;
	m_nColumn = nColumn;
	m_nLastChar = 0;
	// If mouse click brought us here,
	// then no exit on arrows
	SetExitOnArrows(nFirstChar != VK_LBUTTON);

	m_Rect = rect;  // For bizarre CE bug.

	DWORD dwEditStyle =// WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL /*| ES_AUTOHSCROLL*/ | ES_MULTILINE /*| ES_WANTRETURN*/
		WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOVSCROLL | EN_VSCROLL
		| dwStyle;
	if (!Create(dwEditStyle, rect, pParent, nID)) return;

	SetFont(pParent->GetFont());

	SetWindowText(sInitText);
	SetFocus();

	switch (nFirstChar){
	case VK_LBUTTON:
	case VK_RETURN:   SetSel((int)_tcslen(m_sInitText), -1); return;
	case VK_BACK:     SetSel((int)_tcslen(m_sInitText), -1); break;
	case VK_TAB:
	case VK_DOWN:
	case VK_UP:
	case VK_RIGHT:
	case VK_LEFT:
	case VK_NEXT:
	case VK_PRIOR:
	case VK_HOME:
	case VK_SPACE:
	case VK_END:      SetSel(0, -1); return;
	default:          SetSel(0, -1);
	}
	m_pro = 0;
	// Added by KiteFly. When entering DBCS chars into cells the first char was being lost
	// SenMessage changed to PostMessage (John Lagerquist)
	if (nFirstChar < 0x80)
		PostMessage(WM_CHAR, nFirstChar);
	else
		PostMessage(WM_IME_CHAR, nFirstChar);
}

HMInPlaceEdit::~HMInPlaceEdit()
{
}

BEGIN_MESSAGE_MAP(HMInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(HMInPlaceEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// HMInPlaceEdit message handlers

// If an arrow key (or associated) is pressed, then exit if
//  a) The Ctrl key was down, or
//  b) m_bExitOnArrows == TRUE
void HMInPlaceEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
		nChar == VK_DOWN || nChar == VK_UP ||
		nChar == VK_RIGHT || nChar == VK_LEFT) &&
		(IsExitOnArrows() || GetKeyState(VK_CONTROL) < 0))
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

// As soon as this edit loses focus, kill it.
void HMInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	EndEdit();
}

void HMInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool isEnter = nChar == VK_RETURN;

	if (nChar == VK_TAB || (isEnter && !IsAltEnter()))
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();    // This will destroy this window
		return;
	}
	if (nChar == VK_ESCAPE)
	{
		SetWindowText(m_sInitText);    // restore previous text
		m_nLastChar = nChar;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString str;
	GetWindowText(str);

	// add some extra buffer
	//str += _T("  ");

	CWindowDC dc(this);
	CFont *pFontDC = dc.SelectObject(GetFont());
	CSize size = dc.GetTextExtent(str);
	dc.SelectObject(pFontDC);



	// Check whether control needs to be resized
	// and whether there is space to grow
	if (size.cx > m_Rect.Width())
	{
		// Get client rect
		CRect ParentRect;
		GetParent()->GetClientRect(&ParentRect);

		if (size.cx + m_Rect.left < ParentRect.right)
			m_Rect.right = m_Rect.left + size.cx;
		else
			m_Rect.right = ParentRect.right;
		MoveWindow(&m_Rect);
	}
	//
	SetForce(false);
}

UINT HMInPlaceEdit::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

////////////////////////////////////////////////////////////////////////////
// HMInPlaceEdit overrides

// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL HMInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
	//
	bool isAlt = IsForce();
	if (!isAlt)
		isAlt = pMsg->message == WM_SYSCHAR;
	SetAltEnter(isAlt && (pMsg->wParam == VK_RETURN));
	// Catch the Alt key so we don't choke if focus is going to an owner drawn button
	if (pMsg->message == WM_SYSCHAR)
	{
		if (!IsAltEnter()) //Alt+Enter 保留
			return TRUE;
		else
		{
			//Alt+Enter 保留,转换为键盘按下消息，重新进入消息循环
			pMsg->message = WM_KEYDOWN;
			SetForce(true);
		}
	}

	//if (::GetFocus() == m_hWnd && pMsg->message == WM_KEYDOWN && (::GetKeyState(VK_CONTROL) & 0x8000))	
	//{	
	//	// 全选   
	//	if (pMsg->wParam == 'A' || pMsg->wParam == 'a')
	//	{
	//	this->SetSel(0, -1);
	//	return(TRUE);
	//	}
	//	// 拷贝   
	//	if (pMsg->wParam == 'C' || pMsg->wParam == 'c')
	//	{
	//	this->Copy();
	//	return(TRUE);
	//	}
	//	// 剪切   
	//	if (pMsg->wParam == 'X' || pMsg->wParam == 'x')
	//	{
	//	this->Cut();
	//	return(TRUE);
	//	}
	//	// 粘贴   
	//	if (pMsg->wParam == 'V' || pMsg->wParam == 'v')
	//	{
	//	this->Paste();
	//	return(TRUE);
	//	}
	//	// 粘贴   
	//	if (pMsg->wParam == 'Z' || pMsg->wParam == 'z')
	//	{
	//	this->Undo();
	//	return(TRUE);
	//	}
	//	//
	//	
	//}	
	if (pMsg->message == WM_MOUSEWHEEL)
	{
		short zDelta = (short)HIWORD(pMsg->wParam);
		short xPos = (short)LOWORD(pMsg->lParam);
		short yPos = (short)HIWORD(pMsg->lParam);
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		if (::GetFocus() == m_hWnd)
		{
			CRect rectControl;
			GetWindowRect(&rectControl);
			ScreenToClient(&rectControl);
			if (rectControl.PtInRect(pt))
			{
				int lineIndex = LineFromChar();
				if (zDelta < 0)
				{
					lineIndex += 1;
				}
				else
				{
					lineIndex -= 1;
				}
				int charindex = LineIndex(lineIndex);
				SetSel(charindex, charindex);
			}
			return TRUE;
		}

	}

	return CWnd::PreTranslateMessage(pMsg);
}

// Auto delete
void HMInPlaceEdit::PostNcDestroy()
{
	CEdit::PostNcDestroy();

	delete this;
}

////////////////////////////////////////////////////////////////////////////
// HMInPlaceEdit implementation

void HMInPlaceEdit::EndEdit()
{
	CString str;

	// EFW - BUG FIX - Clicking on a grid scroll bar in a derived class
	// that validates input can cause this to get called multiple times
	// causing assertions because the edit control goes away the first time.
	static BOOL bAlreadyEnding = FALSE;

	if (bAlreadyEnding)
		return;

	bAlreadyEnding = TRUE;
	GetWindowText(str);

	// Send Notification to parent
	GV_DISPINFO dispinfo;

	dispinfo.hdr.hwndFrom = GetSafeHwnd();
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = GVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT | LVIF_PARAM;
	dispinfo.item.row = m_nRow;
	dispinfo.item.col = m_nColumn;
	dispinfo.item.strText = str;
	dispinfo.item.lParam = (LPARAM)m_nLastChar;

	CWnd* pOwner = GetOwner();
	if (pOwner)
		pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo);

	// Close this window (PostNcDestroy will delete this)
	if (IsWindow(GetSafeHwnd()))
		SendMessage(WM_CLOSE, 0, 0);
	bAlreadyEnding = FALSE;
}

void HMInPlaceEdit::SetForce(bool isTrue)
{
	SetProEnable(m_pro, FORCE, isTrue);
}
void HMInPlaceEdit::SetAltEnter(bool isTrue)
{
	SetProEnable(m_pro, ALTENTER, isTrue);

}
void HMInPlaceEdit::SetExitOnArrows(bool isTrue)
{
	SetProEnable(m_pro, EXITONARROW, isTrue);

}

bool HMInPlaceEdit::IsForce()const
{
	return m_pro & FORCE;
}
bool HMInPlaceEdit::IsAltEnter() const
{
	return m_pro & ALTENTER;

}
bool HMInPlaceEdit::IsExitOnArrows()const
{
	return m_pro & EXITONARROW;

}

_HM_GridControl_END
