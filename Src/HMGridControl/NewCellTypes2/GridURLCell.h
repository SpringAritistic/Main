// GridURLCell.h: interface for the HMGridURLCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__GRIDURLCELL_H__)
#define __GRIDURLCELL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HMGridControl/GridCell.h"
_HM_GridControl_BEGIN

typedef struct {
    LPCTSTR szURLPrefix;
    size_t  nLength;
} URLStruct;



class HM_GridControl_EXT HMGridURLCell : public HMGridCell
{
    DECLARE_DYNCREATE(HMGridURLCell)

public:
	HMGridURLCell();
	virtual ~HMGridURLCell();

    virtual BOOL     Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual BOOL     Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual LPCTSTR  GetTipText() { return NULL; }
	void SetAutoLaunchUrl(BOOL bLaunch = TRUE) { m_bLaunchUrl = bLaunch;	}
	BOOL GetAutoLaunchUrl() { return m_bLaunchUrl;	}

protected:
    virtual BOOL OnSetCursor();
    virtual void OnClick(CPoint PointCellRelative);

	BOOL HasUrl(CString str);
    BOOL OverURL(CPoint& pt, CString& strURL);

protected:
#ifndef _WIN32_WCE
    static HCURSOR g_hLinkCursor;		// Hyperlink mouse cursor
	HCURSOR GetHandCursor();
#endif
    static URLStruct g_szURIprefixes[];

protected:
	COLORREF m_clrUrl;
	BOOL     m_bLaunchUrl;
    CRect    m_Rect;
};

_HM_GridControl_END
#endif // !defined(__GRIDURLCELL_H__)
