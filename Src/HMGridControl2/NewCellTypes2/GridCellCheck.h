#if !defined(__GRIDCELLCHECK_H__)
#define __GRIDCELLCHECK_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// GridCellCheck.h : header file
//
// MFC Grid Control - Grid combo cell class header file
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
// For use with HMGridCtrl v2.22+
//
//////////////////////////////////////////////////////////////////////


#include "HMGridControl/GridCell.h"

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

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

_HM_GridControl_END
#endif // !defined(__GRIDCELLCHECK_H__)
