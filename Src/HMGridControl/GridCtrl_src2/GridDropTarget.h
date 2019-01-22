//////////////////////////////////////////////////////////////////////
// GridDropTarget.h : header file
//
// MFC Grid Control - Drag/Drop target implementation
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

#if !defined(__GRIDDROPTARGET_H__)
#define __GRIDDROPTARGET_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include <afxole.h>
_HM_GridControl_BEGIN

class HMGridCtrl;

/////////////////////////////////////////////////////////////////////////////
// HMGridDropTarget command target

class HM_GridControl_EXT HMGridDropTarget : public COleDropTarget
{
public:
    HMGridDropTarget();
    virtual ~HMGridDropTarget();

// Attributes
public:
    HMGridCtrl* m_pGridCtrl;
    BOOL       m_bRegistered;

// Operations
public:
    BOOL Register(HMGridCtrl *pGridCtrl);
    virtual void Revoke();

    BOOL        OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
    DROPEFFECT  OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    void        OnDragLeave(CWnd* pWnd);
    DROPEFFECT  OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    DROPEFFECT  OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(HMGridDropTarget)
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(HMGridDropTarget)
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

_HM_GridControl_END
#endif // !defined(__GRIDDROPTARGET_H__)
