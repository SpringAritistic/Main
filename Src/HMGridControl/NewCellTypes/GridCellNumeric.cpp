//11/22/2018  4:37:13 PM
#include "stdafx.h"
#include "..\GridCtrl_src\GridCell.h"
#include "GridCellNumeric.h"
#include "..\GridCtrl_src\inplaceedit.h"
#include "..\GridCtrl_src\GridCtrl.h"


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
