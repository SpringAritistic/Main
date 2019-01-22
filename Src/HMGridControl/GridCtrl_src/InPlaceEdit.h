//11/22/2018  4:37:13 PM
#ifndef _INPLACEEDIT_GRIDCONTROL_H_
#define _INPLACEEDIT_GRIDCONTROL_H_
#pragma  once

_HM_GridControl_BEGIN

class HM_GridControl_EXT HMInPlaceEdit : public CEdit
{
// Construction
public:
    HMInPlaceEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
                 int nRow, int nColumn, CString sInitText, UINT nFirstChar);

// Attributes
public:
 
// Operations
public:
     void EndEdit();
 
// Overrides
     // ClassWizard generated virtual function overrides
     //{{AFX_VIRTUAL(HMInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
 
// Implementation
public:
     virtual ~HMInPlaceEdit();
 
// Generated message map functions
protected:
    //{{AFX_MSG(HMInPlaceEdit)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    int     m_nRow;
    int     m_nColumn;
    CString m_sInitText;
    UINT    m_nLastChar;
    BOOL    m_bExitOnArrows;
    CRect   m_Rect;
};
 
/////////////////////////////////////////////////////////////////////////////




_HM_GridControl_END
#endif
