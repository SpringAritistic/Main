#if !defined(__TITLETIP_H__)
#define __TITLETIP_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TitleTip.h : header file
//

#define TITLETIP_CLASSNAME _T("ZTitleTip")
_HM_GridControl_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CTitleTip window

class CTitleTip : public CWnd
{
// Construction
public:
	CTitleTip();
	virtual ~CTitleTip();
	virtual BOOL Create( CWnd *pParentWnd);

// Attributes
public:

// Operations
public:
	void Show(CRect rectTitle, LPCTSTR lpszTitleText, 
              int xoffset = 0, int nMaxChars = -1, 
              LPRECT lpHoverRect = NULL, LPLOGFONT lpLogFont = NULL,
              DWORD dwFormat = DT_LEFT|DT_NOCLIP|DT_EDITCONTROL|DT_NOPREFIX|DT_WORDBREAK);
    void Hide();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitleTip)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:

protected:
	CWnd *m_pParentWnd;
	CRect m_rectTitle;
    CRect m_rectHover;
    CRect m_rectDisplay;
    CString m_strTitle;		// for use in OnPaint()
    DWORD m_dwFormat;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTitleTip)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

_HM_GridControl_END
#endif // !defined(__TITLETIP_H__)
