//11/22/2018  4:37:13 PM
#ifndef _GRIDDROPTARGET_GRIDCONTROL_H_
#define _GRIDDROPTARGET_GRIDCONTROL_H_
#pragma  once

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



_HM_GridControl_END
#endif
