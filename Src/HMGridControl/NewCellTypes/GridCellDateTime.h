//11/22/2018  4:37:13 PM
#ifndef _GRIDCELLDATETIME_GRIDCONTROL_H_
#define _GRIDCELLDATETIME_GRIDCONTROL_H_
#pragma  once


_HM_GridControl_BEGIN

class HM_GridControl_EXT HMGridCellDateTime : public HMGridCell
{
  friend class HMGridCtrl ;
  DECLARE_DYNCREATE(HMGridCellDateTime)

  CTime m_cTime;
  DWORD m_dwStyle;

public:
	HMGridCellDateTime();
	HMGridCellDateTime(DWORD dwStyle);
	virtual ~HMGridCellDateTime();
    virtual CSize GetCellExtent(CDC* pDC);

  // editing cells
public:
	void Init(DWORD dwStyle);
	virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual CWnd* GetEditWnd() const;
	virtual void  EndEdit();


	CTime* GetTime();
	void   SetTime(CTime time);
};

class HM_GridControl_EXT HMInPlaceDateTime : public CDateTimeCtrl
{
// Construction
public:
	HMInPlaceDateTime(CWnd* pParent,         // parent
                   CRect& rect,           // dimensions & location
                   DWORD dwStyle,         // window/combobox style
                   UINT nID,              // control ID
                   int nRow, int nColumn, // row and column
                   COLORREF crFore, COLORREF crBack,  // Foreground, background colour
                   CTime* pcTime,
          		   UINT nFirstChar);      // first character to pass to control

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceList)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~HMInPlaceDateTime();
    void EndEdit();

// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceList)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	//afx_msg void OnSelendOK();

	DECLARE_MESSAGE_MAP()

private:
    CTime*   m_pcTime;
	int		 m_nRow;
	int		 m_nCol;
 	UINT     m_nLastChar; 
	BOOL	 m_bExitOnArrows; 
    COLORREF m_crForeClr, m_crBackClr;
};
_HM_GridControl_END
#endif
