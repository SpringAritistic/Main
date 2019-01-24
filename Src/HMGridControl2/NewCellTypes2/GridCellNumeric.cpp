// GridCellNumeric.cpp: implementation of the HMGridCellNumeric class.
//
// Written by Andrew Truckle [ajtruckle@wsatkins.co.uk]
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMGridControl/GridCellNumeric.h"
#include "HMGridControl/inplaceedit.h"
#include "HMGridControl/GridCtrl.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
_HM_GridControl_BEGIN
IMPLEMENT_DYNCREATE(HMGridCellNumeric, HMGridCell)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Create a control to do the editing
BOOL HMGridCellNumeric::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    m_bEditing = TRUE;
    
    // HMInPlaceEdit auto-deletes itself
    m_pEditWnd = new HMInPlaceEdit(GetGrid(), rect, /*GetStyle() |*/ ES_NUMBER, nID, nRow, nCol,
		GetText(), nChar);

    return TRUE;
}

// Cancel the editing.
void HMGridCellNumeric::EndEdit()
{
    if (m_pEditWnd)
        ((HMInPlaceEdit*)m_pEditWnd)->EndEdit();
}

_HM_GridControl_END
