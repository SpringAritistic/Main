/////////////////////////////////////////////////////////////////////////////
// Titletip.h : header file
//
// MFC Grid Control - cell titletips
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with HMGridCtrl v2.10+
//
//////////////////////////////////////////////////////////////////////

#if !defined(__TITLETIP_H__)
#define __TITLETIP_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

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
    void SetParentWnd(CWnd* pParentWnd)  { m_pParentWnd = pParentWnd; }
    CWnd* GetParentWnd()                 { return m_pParentWnd;       }

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
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
_HM_GridControl_END

#endif // !defined(__TITLETIP_H__)
