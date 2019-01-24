//11/22/2018  4:37:13 PM
#ifndef _TITLETIP_GRIDCONTROL_H_
#define _TITLETIP_GRIDCONTROL_H_
#pragma  once
#define TITLETIP_CLASSNAME _T("ZTitleTip")
_HM_GridControl_BEGIN

/////////////////////////////////////////////////////////////////////////////
// HMTitleTip window

class HM_GridControl_EXT HMTitleTip : public CWnd
{
// Construction
public:
	HMTitleTip();
	virtual ~HMTitleTip();
	virtual BOOL Create( CWnd *pParentWnd);

// Attributes
public:
    void SetParentWnd(CWnd* pParentWnd) ;
    CWnd* GetParentWnd()                ;

// Operations
public:
	void Show(CRect rectTitle, LPCTSTR lpszTitleText, 
              int xoffset = 0, LPRECT lpHoverRect = NULL, 
              const LOGFONT* lpLogFont = NULL,
              COLORREF crTextClr = CLR_DEFAULT, COLORREF crBackClr = CLR_DEFAULT);
    void Hide();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HMTitleTip)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd  *m_pParentWnd;
	CRect  m_rectTitle;
    CRect  m_rectHover;
    DWORD  m_dwLastLButtonDown;
    DWORD  m_dwDblClickMsecs;
    BOOL   m_bCreated;

	// Generated message map functions
protected:
	//{{AFX_MSG(HMTitleTip)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


_HM_GridControl_END
#endif
