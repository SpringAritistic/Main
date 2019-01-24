// GridCellNumeric.h: interface for the HMGridCellNumeric class.
//
// Written by Andrew Truckle [ajtruckle@wsatkins.co.uk]
//
//////////////////////////////////////////////////////////////////////

#if !defined(__GRIDCELLNUMERIC_H__)
#define __GRIDCELLNUMERIC_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HMGridControl/GridCell.h"
_HM_GridControl_BEGIN

class HM_GridControl_EXT HMGridCellNumeric : public HMGridCell
{
    DECLARE_DYNCREATE(HMGridCellNumeric)

public:
    virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void EndEdit();

};

_HM_GridControl_END
#endif // !defined(__GRIDCELLNUMERIC_H__)
