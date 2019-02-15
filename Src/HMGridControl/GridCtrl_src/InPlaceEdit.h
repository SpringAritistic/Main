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
    CRect   m_Rect;

	enum PRO { FORCE = 1, ALTENTER = FORCE << 1, EXITONARROW = ALTENTER<<1 };
	UINT m_pro;
	
	//
	void SetForce(bool isTrue);
	void SetAltEnter(bool isTrue);
	void SetExitOnArrows(bool isTrue);
	bool IsForce()const;
	bool IsAltEnter() const;
	bool IsExitOnArrows()const;



	//重新计算窗口大小
	void Refresh(const CString &str);
	bool IsFit(const CString &str) const;
	CSize FitSize(const CString &str) const;
};
 
/////////////////////////////////////////////////////////////////////////////




_HM_GridControl_END
#endif
