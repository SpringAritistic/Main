//11/22/2018  4:37:13 PM
#include "stdafx.h"
#include "GridCtrl.h"
#include "GridDropTarget.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
_HM_GridControl_BEGIN
/////////////////////////////////////////////////////////////////////////////
// HMGridDropTarget

HMGridDropTarget::HMGridDropTarget()
{
    m_pGridCtrl = NULL;
    m_bRegistered = FALSE;
}

HMGridDropTarget::~HMGridDropTarget()
{
}

// Overloaded Register() function performs the normal COleDropTarget::Register
// but also serves to connect this COleDropTarget with the parent grid control,
// where all drop messages will ultimately be forwarded.
BOOL HMGridDropTarget::Register(HMGridCtrl *pGridCtrl)
{
    if (m_bRegistered)
        return FALSE;

    // Stop re-entry problems
    static BOOL bInProcedure = FALSE;
    if (bInProcedure)
        return FALSE;
    bInProcedure = TRUE;

    ASSERT(pGridCtrl->IsKindOf(RUNTIME_CLASS(HMGridCtrl)));
    ASSERT(pGridCtrl);

    if (!pGridCtrl || !pGridCtrl->IsKindOf(RUNTIME_CLASS(HMGridCtrl)))
    {
        bInProcedure = FALSE;
        return FALSE;
    }

    m_pGridCtrl = pGridCtrl;

    m_bRegistered = COleDropTarget::Register(pGridCtrl);

    bInProcedure = FALSE;
    return m_bRegistered;
}

void HMGridDropTarget::Revoke()
{
    m_bRegistered = FALSE;
    COleDropTarget::Revoke();
}

BEGIN_MESSAGE_MAP(HMGridDropTarget, COleDropTarget)
    //{{AFX_MSG_MAP(HMGridDropTarget)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HMGridDropTarget message handlers

DROPEFFECT HMGridDropTarget::OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint /*point*/)
{
//    TRACE("In HMGridDropTarget::OnDragScroll\n");
    if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
    {
        if (dwKeyState & MK_CONTROL)
            return DROPEFFECT_COPY;
        else
            return DROPEFFECT_MOVE;
    } else
        return DROPEFFECT_NONE;
}

DROPEFFECT HMGridDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, 
                                        DWORD dwKeyState, CPoint point)
{
    TRACE(_T("In HMGridDropTarget::OnDragEnter\n"));
    ASSERT(m_pGridCtrl);

    if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
        return m_pGridCtrl->OnDragEnter(pDataObject, dwKeyState, point);
    else
        return DROPEFFECT_NONE;
}

void HMGridDropTarget::OnDragLeave(CWnd* pWnd)
{
    TRACE(_T("In HMGridDropTarget::OnDragLeave\n"));
    ASSERT(m_pGridCtrl);

    if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
        m_pGridCtrl->OnDragLeave();
}

DROPEFFECT HMGridDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, 
                                       DWORD dwKeyState, CPoint point)
{
//    TRACE("In HMGridDropTarget::OnDragOver\n");
    ASSERT(m_pGridCtrl);

    if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
        return m_pGridCtrl->OnDragOver(pDataObject, dwKeyState, point);
    else
        return DROPEFFECT_NONE;
}

BOOL HMGridDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
                             DROPEFFECT dropEffect, CPoint point)
{
    TRACE(_T("In HMGridDropTarget::OnDrop\n"));
    ASSERT(m_pGridCtrl);

    if (pWnd->GetSafeHwnd() == m_pGridCtrl->GetSafeHwnd())
        return m_pGridCtrl->OnDrop(pDataObject, dropEffect, point);
    else
        return FALSE;
}
_HM_GridControl_END
