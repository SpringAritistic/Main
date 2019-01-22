//11/22/2018  4:37:13 PM
#ifndef _GRIDURLCELL_GRIDCONTROL_H_
#define _GRIDURLCELL_GRIDCONTROL_H_
#pragma  once

_HM_GridControl_BEGIN

 struct HM_GridControl_EXT URLStruct{
    LPCTSTR szURLPrefix;
    size_t  nLength;
} ;



class HM_GridControl_EXT HMGridURLCell : public HMGridCell
{
    DECLARE_DYNCREATE(HMGridURLCell)

public:
	HMGridURLCell();
	virtual ~HMGridURLCell();

    virtual BOOL     Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual BOOL     Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual LPCTSTR  GetTipText();
	void SetAutoLaunchUrl(BOOL bLaunch = TRUE);
	BOOL GetAutoLaunchUrl();

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
#endif
