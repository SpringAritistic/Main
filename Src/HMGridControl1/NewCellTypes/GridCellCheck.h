//11/22/2018  4:37:13 PM
#ifndef _GRIDCELLCHECK_GRIDCONTROL_H_
#define _GRIDCELLCHECK_GRIDCONTROL_H_
#pragma  once



_HM_GridControl_BEGIN

class HM_GridControl_EXT CGridCellCheck : public HMGridCell
{
    friend class HMGridCtrl;
    DECLARE_DYNCREATE(CGridCellCheck)

public:
    CGridCellCheck();

public:
	BOOL SetCheck(BOOL bChecked = TRUE);
	BOOL GetCheck();

// Operations
	virtual CSize GetCellExtent(CDC* pDC);
    virtual void OnClick( CPoint PointCellRelative);
    virtual BOOL GetTextRect( LPRECT pRect);

protected:
	CRect GetCheckPlacement();

    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);

protected:
    BOOL  m_bChecked;
    CRect m_Rect;
};



_HM_GridControl_END
#endif
