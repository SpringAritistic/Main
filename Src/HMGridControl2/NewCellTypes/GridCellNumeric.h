//11/22/2018  4:37:13 PM
#ifndef _GRIDCELLNUMERIC_GRIDCONTROL_H_
#define _GRIDCELLNUMERIC_GRIDCONTROL_H_
#pragma  once

_HM_GridControl_BEGIN

class HM_GridControl_EXT HMGridCellNumeric : public HMGridCell
{
    DECLARE_DYNCREATE(HMGridCellNumeric)

public:
    virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void EndEdit();

};

_HM_GridControl_END
#endif
