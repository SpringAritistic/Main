//11/22/2018  4:37:13 PM
#ifndef _GRIDCELLBASE_GRIDCONTROL_H_
#define _GRIDCELLBASE_GRIDCONTROL_H_
#pragma  once

_HM_GridControl_BEGIN

// Used for Get/SetItem calls.
struct HM_GridControl_EXT GV_ITEM {
    int      row,col;     // Row and Column of item
    UINT     mask;        // Mask for use in getting/setting cell data
    UINT     nState;      // cell state (focus/hilighted etc)
    DWORD    nFormat;     // Format of cell
    int      iImage;      // index of the list view item¡¯s icon
    COLORREF crBkClr;     // Background colour (or CLR_DEFAULT)
    COLORREF crFgClr;     // Forground colour (or CLR_DEFAULT)
    LPARAM   lParam;      // 32-bit value to associate with item
    LOGFONT  lfFont;      // Cell font
    UINT     nMargin;     // Internal cell margin
    CString  strText;     // Text in cell
} ;


// Each cell contains one of these. Fields "row" and "column" are not stored since we
// will usually have acces to them in other ways, and they are an extra 8 bytes per
// cell that is probably unnecessary.

class HM_GridControl_EXT HMGridCellBase : public CObject
{
    friend class HMGridCtrl;
    DECLARE_DYNAMIC(HMGridCellBase)

// Construction/Destruction
public:
    HMGridCellBase();
    virtual ~HMGridCellBase();

// Attributes
public:
    virtual void SetText(LPCTSTR /* szText */)              = 0 ;
    virtual void SetImage(int /* nImage */)                 = 0 ;
    virtual void SetData(LPARAM /* lParam */)               = 0 ;
	virtual void SetState(DWORD nState);
    virtual void SetFormat(DWORD /* nFormat */)             = 0 ;
    virtual void SetTextClr(COLORREF /* clr */)             = 0 ;
    virtual void SetBackClr(COLORREF /* clr */)             = 0 ;
    virtual void SetFont(const LOGFONT* /* plf */)          = 0 ;
    virtual void SetMargin( UINT /* nMargin */)             = 0 ;
    virtual void SetGrid(HMGridCtrl* /* pGrid */)            = 0 ;
    virtual void SetCoords( int /* nRow */, int /* nCol */) = 0 ;

    virtual LPCTSTR    GetText()       const                = 0 ;
	virtual LPCTSTR    GetTipText()    const;// may override TitleTip return
    virtual int        GetImage()      const                = 0 ;
    virtual LPARAM     GetData()       const                = 0 ;
	virtual DWORD      GetState()      const;
    virtual DWORD      GetFormat()     const                = 0 ;
    virtual COLORREF   GetTextClr()    const                = 0 ;
    virtual COLORREF   GetBackClr()    const                = 0 ;
    virtual LOGFONT  * GetFont()       const                = 0 ;
    virtual CFont    * GetFontObject() const                = 0 ;
    virtual HMGridCtrl* GetGrid()       const                = 0 ;
    virtual CWnd     * GetEditWnd()    const                = 0 ;
    virtual UINT       GetMargin()     const                = 0 ;

    virtual HMGridCellBase* GetDefaultCell() const;

    virtual BOOL IsDefaultFont()       const                = 0 ;
    virtual BOOL IsEditing()           const                = 0 ;
	virtual BOOL IsFocused()           const;
	virtual BOOL IsFixed()             const;
    virtual BOOL IsFixedCol()          const;
    virtual BOOL IsFixedRow()          const;
    virtual BOOL IsSelected()          const;
    virtual BOOL IsReadOnly()          const;
    virtual BOOL IsModified()          const;
    virtual BOOL IsDropHighlighted()   const;

// Operators
public:
    virtual void operator=(HMGridCellBase& cell);

// Operations
public:
	bool IsMerged();
	void SetMergeRange(HMCellRange range);
	void Show(bool IsShow);
    virtual void Reset();

    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual BOOL GetTextRect( LPRECT pRect);    // i/o:  i=dims of cell rect; o=dims of text rect
    virtual BOOL GetTipTextRect( LPRECT pRect) { return GetTextRect( pRect); }  // may override for btns, etc.
    virtual CSize GetTextExtent(LPCTSTR str, CDC* pDC = NULL);
    virtual CSize GetCellExtent(CDC* pDC);

    // Editing
    virtual BOOL Edit( int /* nRow */, int /* nCol */, CRect /* rect */, CPoint /* point */, 
                       UINT /* nID */, UINT /* nChar */) { ASSERT( FALSE); return FALSE;}
	virtual BOOL ValidateEdit(LPCTSTR str);
	virtual void EndEdit();

    // EFW - Added to print cells properly
    virtual BOOL PrintCell(CDC* pDC, int nRow, int nCol, CRect rect);

    // add additional protected grid members required of cells
    LRESULT SendMessageToParent(int nRow, int nCol, int nMessage);

protected:
    virtual void OnEndEdit();
    virtual void OnMouseEnter();
    virtual void OnMouseOver();
    virtual void OnMouseLeave();
    virtual void OnClick( CPoint PointCellRelative);
    virtual void OnClickDown( CPoint PointCellRelative);
    virtual void OnRClick( CPoint PointCellRelative);
    virtual void OnDblClick( CPoint PointCellRelative);
    virtual BOOL OnSetCursor();

public:
	void UnMerge();
	virtual bool IsShow() ;
	virtual HMCellRange GetMergeRange();
	virtual bool IsMergeWithOthers();
	virtual HMCellID GetMergeCellID();
	virtual void SetMergeCellID(HMCellID cell);
protected:
    DWORD    m_nState;      // Cell state (selected/focus etc)
private:

	HMCellRange m_MergeRange;
	bool m_IsMergeWithOthers;
	HMCellID m_MergeCellID;
	bool m_Hide;
};



_HM_GridControl_END
#endif
