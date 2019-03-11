//11/22/2018  4:37:13 PM
#ifndef _GRIDCTRL_GRIDCONTROL_H_
#define _GRIDCTRL_GRIDCONTROL_H_
#pragma  once
#include "CellRange.h"
#include "GridCell.h"
#include <afxtempl.h>
#include <vector>


///////////////////////////////////////////////////////////////////////////////////
// Defines - these determine the features (and the final size) of the final code
///////////////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////////////
// Conditional includes
///////////////////////////////////////////////////////////////////////////////////

#include "TitleTip.h"

#include "GridDropTarget.h"

#include <afxole.h>


///////////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////////

// Handy functions
#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

// Backwards compatibility for pre 2.20 grid versions
#define DDX_GridControl(pDX, nIDC, rControl)  DDX_Control(pDX, nIDC, rControl)     

_HM_GridControl_BEGIN

///////////////////////////////////////////////////////////////////////////////////
// Structures
///////////////////////////////////////////////////////////////////////////////////

// This structure sent to Grid's parent in a WM_NOTIFY message
struct HM_GridControl_EXT NM_GRIDVIEW {
    NMHDR hdr;
    int   iRow;
    int   iColumn;
} ;

// This is sent to the Grid from child in-place edit controls
struct HM_GridControl_EXT GV_DISPINFO {
    NMHDR   hdr;
    GV_ITEM item;
} ;

// This is sent to the Grid from child in-place edit controls
 struct HM_GridControl_EXT GV_CACHEHINT {
    NMHDR      hdr;
    HMCellRange range;
} ;

 //
 // This is sent to the Grid from child in-place edit controls
 struct HM_GridControl_EXT GV_GRIDSEL {
	 NMHDR      hdr;
	 HMExcle cells;
 };

// storage typedef for each row in the grid
 //typedef CTypedPtrArray<CObArray, HMGridCellBase*> GRID_ROW;
 typedef std::vector<HMGridCellBase*> GRID_ROW;

// For virtual mode callback
typedef BOOL (CALLBACK* GRIDCALLBACK)(GV_DISPINFO *, LPARAM);




/////////////////////////////////////////////////////////////////////////////
// HMGridCtrl window

typedef bool (*PVIRTUALCOMPARE)(int, int);

class HM_GridControl_EXT HMGridCtrl : public CWnd
{
    DECLARE_DYNCREATE(HMGridCtrl )
    friend class HMGridCell;
    friend class HMGridCellBase;

// Construction
public:
    HMGridCtrl(int nRows = 0, int nCols = 0, int nFixedRows = 0, int nFixedCols = 0);

    BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
                DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

///////////////////////////////////////////////////////////////////////////////////
// Attributes
///////////////////////////////////////////////////////////////////////////////////
public:
    int  GetRowCount() const                   ;
    int  GetColumnCount() const                ;
    int  GetFixedRowCount() const              ;
    int  GetFixedColumnCount() const           ;
    BOOL SetRowCount(int nRows = 10);
    BOOL SetColumnCount(int nCols = 10);
    BOOL SetFixedRowCount(int nFixedRows = 1);
    BOOL SetFixedColumnCount(int nFixedCols = 1);

    int  GetRowHeight(int nRow) const;
    BOOL SetRowHeight(int row, int height);
    int  GetColumnWidth(int nCol) const;
    BOOL SetColumnWidth(int col, int width);

	BOOL GetCellOrigin(int nRow, int nCol, LPPOINT p)const;
	BOOL GetCellOrigin(const HMCellID& cell, LPPOINT p)const;
	BOOL GetCellRect(int nRow, int nCol, LPRECT pRect)const;
	BOOL GetCellRect(const HMCellID& cell, LPRECT pRect)const;
	CRect GetCellRect(int nRow, int nCol)const;
	CRect GetCellRect(const HMCellID& cell)const;

	BOOL GetTextRect(const HMCellID& cell, LPRECT pRect)const;
	BOOL GetTextRect(int nRow, int nCol, LPRECT pRect)const;

	HMCellID GetCellFromPt(CPoint point, BOOL bAllowFixedCellCheck = TRUE) const;
	CRect GetCellRectFromPt(CPoint point, BOOL bAllowFixedCellCheck = TRUE)const;


    int  GetFixedRowHeight() const;
    int  GetFixedColumnWidth() const;
    long GetVirtualWidth() const;
    long GetVirtualHeight() const;

    CSize GetTextExtent(int nRow, int nCol, LPCTSTR str);
    // EFW - Get extent of current text in cell
	CSize GetCellTextExtent(int nRow, int nCol);
    void     SetGridBkColor(COLORREF clr)         ;
    COLORREF GetGridBkColor() const               ;
    void     SetGridLineColor(COLORREF clr)       ;
    COLORREF GetGridLineColor() const             ;

	void	 SetTitleTipBackClr(COLORREF clr = CLR_DEFAULT) ;
	COLORREF GetTitleTipBackClr()				            ;
	void	 SetTitleTipTextClr(COLORREF clr = CLR_DEFAULT) ;
	COLORREF GetTitleTipTextClr()				            ;

    // ***************************************************************************** //
    // These have been deprecated. Use GetDefaultCell and then set the colors
    void     SetTextColor(COLORREF clr)      ;
    COLORREF GetTextColor()                  ;
    void     SetTextBkColor(COLORREF clr)    ;
    COLORREF GetTextBkColor()                ;
    void     SetFixedTextColor(COLORREF clr) ;
                                             

    COLORREF GetFixedTextColor() const       ;
    void     SetFixedBkColor(COLORREF clr)   ;
                                            

    COLORREF GetFixedBkColor() const         ;
    void     SetGridColor(COLORREF clr)      ;
    COLORREF GetGridColor()                  ;
    void     SetBkColor(COLORREF clr)        ;
    COLORREF GetBkColor()                    ;

    void     SetDefCellMargin( int nMargin)  ;
                                           

    int      GetDefCellMargin() const        ;
											
    int      GetDefCellHeight() const        ;
    void     SetDefCellHeight(int nHeight)   ;
                                             
    int      GetDefCellWidth() const         ;
    void     SetDefCellWidth(int nWidth)     ;
                                            

    // ***************************************************************************** //

	int GetSelectedCount() const;

    HMCellID SetFocusCell(HMCellID cell);
    HMCellID SetFocusCell(int nRow, int nCol);
	HMCellID GetFocusCell() const;


	void SetCallbackFunc(GRIDCALLBACK pCallback,LPARAM lParam);
	GRIDCALLBACK GetCallbackFunc();


    void SetImageList(CImageList* pList)          ;
    CImageList* GetImageList() const              ;
												  ;
    void SetGridLines(int nWhichLines = GVL_BOTH);;
    int  GetGridLines() const                     ;



    void SetSortColumn(int nCol);				  ;
    int  GetSortColumn() const                    ;



    void SetAutoSizeStyle(int nStyle = GVS_BOTH)  ;
    int  GetAutoSizeStyle()                       ;
												  ;

												  ;



///////////////////////////////////////////////////////////////////////////////////
// default Grid cells. Use these for setting default values such as colors and fonts
///////////////////////////////////////////////////////////////////////////////////
public:
    HMGridCellBase* GetDefaultCell(BOOL bFixedRow, BOOL bFixedCol) const;

///////////////////////////////////////////////////////////////////////////////////
// Grid cell Attributes
///////////////////////////////////////////////////////////////////////////////////
public:
	HMGridCellBase* GetCell(int nRow, int nCol, bool isReal = true) const;   // Get the actual cell!

    void SetModified(bool bModified = true, int nRow = -1, int nCol = -1);
    bool IsModified(int nRow = -1, int nCol = -1);
    BOOL IsCellFixed(int nRow, int nCol);

    BOOL   SetItem(const GV_ITEM* pItem);
    BOOL   GetItem(GV_ITEM* pItem);
    BOOL   SetItemText(int nRow, int nCol, LPCTSTR str);
    // The following was virtual. If you want to override, use 
    //  HMGridCellBase-derived class's GetText() to accomplish same thing
    CString GetItemText(int nRow, int nCol) const;

    // EFW - 06/13/99 - Added to support printf-style formatting codes.
    // Also supports use with a string resource ID
#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 210)
    BOOL   SetItemTextFmt(int nRow, int nCol, LPCTSTR szFmt, ...);
    BOOL   SetItemTextFmtID(int nRow, int nCol, UINT nID, ...);
#endif

    BOOL   SetItemData(int nRow, int nCol, LPARAM lParam);
    LPARAM GetItemData(int nRow, int nCol) const;
    BOOL   SetItemImage(int nRow, int nCol, int iImage);
    int    GetItemImage(int nRow, int nCol) const;
    BOOL   SetItemState(int nRow, int nCol, UINT state);
    UINT   GetItemState(int nRow, int nCol) const;
    BOOL   SetItemFormat(int nRow, int nCol, UINT nFormat);
    UINT   GetItemFormat(int nRow, int nCol) const;
    BOOL   SetItemBkColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    COLORREF GetItemBkColour(int nRow, int nCol) const;
    BOOL   SetItemFgColour(int nRow, int nCol, COLORREF cr = CLR_DEFAULT);
    COLORREF GetItemFgColour(int nRow, int nCol) const;
    BOOL SetItemFont(int nRow, int nCol, const LOGFONT* lf);
    const LOGFONT* GetItemFont(int nRow, int nCol);

    BOOL IsItemEditing(int nRow, int nCol);

    BOOL SetCellType(int nRow, int nCol, CRuntimeClass* pRuntimeClass);
    BOOL SetDefaultCellType( CRuntimeClass* pRuntimeClass);

///////////////////////////////////////////////////////////////////////////////////
// Operations
///////////////////////////////////////////////////////////////////////////////////
public:
    int  InsertColumn(LPCTSTR strHeading, UINT nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE,
                      int nColumn = -1);
    int  InsertRow(LPCTSTR strHeading, int nRow = -1);
    BOOL DeleteColumn(int nColumn);
    BOOL DeleteRow(int nRow);
    BOOL DeleteNonFixedRows();
    BOOL DeleteAllItems();

	void ClearCells(HMCellRange Selection);

    BOOL AutoSizeRow(int nRow, BOOL bResetScroll = TRUE);
    BOOL AutoSizeColumn(int nCol, UINT nAutoSizeStyle = GVS_DEFAULT, BOOL bResetScroll = TRUE);
    void AutoSizeRows();
    void AutoSizeColumns(UINT nAutoSizeStyle = GVS_DEFAULT);
    void AutoSize(UINT nAutoSizeStyle = GVS_DEFAULT);
    void ExpandColumnsToFit(BOOL bExpandFixed = TRUE);
    void ExpandLastColumn();
    void ExpandRowsToFit(BOOL bExpandFixed = TRUE);
    void ExpandToFit(BOOL bExpandFixed = TRUE);

    void Refresh();
    void AutoFill();   // Fill grid with blank cells

	void EnsureVisible(HMCellID &cell);
    void EnsureVisible(int nRow, int nCol);
    BOOL IsCellVisible(int nRow, int nCol);
    BOOL IsCellVisible(HMCellID cell);
    BOOL IsCellEditable(int nRow, int nCol) const;
    BOOL IsCellEditable(HMCellID &cell) const;
    BOOL IsCellSelected(int nRow, int nCol) const;
    BOOL IsCellSelected(HMCellID &cell) const;

    // SetRedraw stops/starts redraws on things like changing the # rows/columns
    // and autosizing, but not for user-intervention such as resizes
    void SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars = FALSE);
    BOOL RedrawCell(int nRow, int nCol, CDC* pDC = NULL);
    BOOL RedrawCell(const HMCellID& cell, CDC* pDC = NULL);
    BOOL RedrawRow(int row);
    BOOL RedrawColumn(int col);

#ifndef _WIN32_WCE
    BOOL Save(LPCTSTR filename, TCHAR chSeparator = _T(','));
    BOOL Load(LPCTSTR filename, TCHAR chSeparator = _T(','));
#endif

///////////////////////////////////////////////////////////////////////////////////
// Cell Ranges
///////////////////////////////////////////////////////////////////////////////////
 public:
    HMCellRange GetCellRange() const;
    HMCellRange GetSelectedCellRange() const;

	//获取不连续的单元格
	HMExcle GetSelectCells(bool isPre=false) const;
	//
    void SetSelectedRange(const HMCellRange& Range, BOOL bForceRepaint = FALSE, BOOL bSelectCells = TRUE);
    void SetSelectedRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol,
                          BOOL bForceRepaint = FALSE, BOOL bSelectCells = TRUE);
	//
	std::vector< HMGridCellBase*> GetMergeCellsInRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol) const;
	std::vector< HMGridCellBase*> GetMergeCellsInRange(const HMCellRange&range) const;

	std::vector< HMCellID> GetMergeIDsInRange(const HMCellRange&range) const;
	std::vector< HMCellID> GetMergeIDsInRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol) const;

	HMCellRange  GetCellRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol) const;
	HMCellRange  GetCellRange(const HMCellRange&range) const;


    BOOL IsValid(int nRow, int nCol) const;
    BOOL IsValid(const HMCellID& cell) const;
    BOOL IsValid(const HMCellRange& range) const;
	bool  IsRowValid(int nRow) const;
	bool  IsColValid(int nCol) const;
///////////////////////////////////////////////////////////////////////////////////
// Clipboard, drag and drop, and cut n' paste operations
///////////////////////////////////////////////////////////////////////////////////
#ifndef GRIDCONTROL_NO_CLIPBOARD
    virtual void CutSelectedText();
    virtual COleDataSource* CopyTextFromGrid();
    virtual BOOL PasteTextToGrid(HMCellID cell, COleDataObject* pDataObject, BOOL bSelectPastedCells=TRUE);
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
 public:
    virtual void OnBeginDrag();
    virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave();
    virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
#endif

#ifndef GRIDCONTROL_NO_CLIPBOARD
    virtual void OnEditCut();
    virtual void OnEditCopy();
    virtual void OnEditPaste();
#endif
    virtual void OnEditSelectAll();

///////////////////////////////////////////////////////////////////////////////////
// Misc.
///////////////////////////////////////////////////////////////////////////////////
public:
    HMCellID GetNextItem(HMCellID& cell, int nFlags) const;

	BOOL SortItems(int nCol, BOOL bAscending, LPARAM data = 0);
    BOOL SortTextItems(int nCol, BOOL bAscending, LPARAM data = 0);
    BOOL SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data = 0);

	void SetCompareFunction(PFNLVCOMPARE pfnCompare);

	// in-built sort functions
	static int CALLBACK pfnCellTextCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK pfnCellNumericCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

///////////////////////////////////////////////////////////////////////////////////
// Printing
///////////////////////////////////////////////////////////////////////////////////
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
public:
    void Print(CPrintDialog* pPrntDialog = NULL);

												   ;


    // Use -1 to have it keep the existing value
    void SetPrintMarginInfo(int nHeaderHeight, int nFooterHeight,
        int nLeftMargin, int nRightMargin, int nTopMargin,
        int nBottomMargin, int nGap);

    void GetPrintMarginInfo(int &nHeaderHeight, int &nFooterHeight,
        int &nLeftMargin, int &nRightMargin, int &nTopMargin,
        int &nBottomMargin, int &nGap);

///////////////////////////////////////////////////////////////////////////////////
// Printing overrides for derived classes
///////////////////////////////////////////////////////////////////////////////////
public:
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnPrint(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);

#endif // #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)

// Implementation
public:
	//Merge the selected cells 
	//by Huang Wei
	HMGridCellBase* GetCell(HMCellID cell,bool isReal=true) const;
	HMCellID GetMergeCellID(HMCellID cell)const;
	HMCellID GetMergeCellID(int nRow, int nCol)const;
	void UnMergeSelectedCells();
	void MergeSelectedCells();
	void UnMergeCells(int nStartRow, int nStartCol, int nEndRow, int nEndCol);
	void MergeCells(int nStartRow, int nStartCol, int nEndRow, int nEndCol);
	int  GetMergeCellWidth(HMCellID cell);
	int  GetMergeCellHeight(HMCellID cell);
	BOOL GetCellOriginNoMerge(int nRow, int nCol, LPPOINT p);
	BOOL GetCellOriginNoMerge(const HMCellID& cell, LPPOINT p);
    virtual ~HMGridCtrl();
	public:
		BOOL RegisterWindowClass(HINSTANCE hInstance);

protected:
    BOOL RegisterWindowClass();

    BOOL Initialise();
    void SetupDefaultCells();

    LRESULT SendMessageToParent(int nRow, int nCol, int nMessage) const;
    LRESULT SendDisplayRequestToParent(GV_DISPINFO* pDisplayInfo) const;
    LRESULT SendCacheHintToParent(const HMCellRange& range) const;

    BOOL InvalidateCellRect(const int row, const int col);
    BOOL InvalidateCellRect(const HMCellID& cell);
    BOOL InvalidateCellRect(const HMCellRange& cellRange);
    void EraseBkgnd(CDC* pDC);

	BOOL GetCellRangeRect(const HMCellRange& cellRange, LPRECT lpRect)const;

    BOOL SetCell(int nRow, int nCol, HMGridCellBase* pCell);

    int  SetMouseMode(int nMode);
    int  GetMouseMode() const   ;

    BOOL MouseOverRowResizeArea(CPoint& point);
    BOOL MouseOverColumnResizeArea(CPoint& point);

	HMCellID GetTopleftNonFixedCell(BOOL bForceRecalculation = FALSE)const;
	HMCellRange GetUnobstructedNonFixedCellRange(BOOL bForceRecalculation = FALSE)const;
	HMCellRange GetVisibleNonFixedCellRange(LPRECT pRect = NULL, BOOL bForceRecalculation = FALSE)const;
	HMCellRange GetVisibleFixedCellRange(LPRECT pRect = NULL, BOOL bForceRecalculation = FALSE)const;

    BOOL IsVisibleVScroll() ;
    BOOL IsVisibleHScroll() ;
    void ResetSelectedRange();
    void ResetScrollBars();
    void EnableScrollBars(int nBar, BOOL bEnable = TRUE);
    int  GetScrollPos32(int nBar, BOOL bGetTrackPos = FALSE);
    BOOL SetScrollPos32(int nBar, int nPos, BOOL bRedraw = TRUE);

    BOOL SortTextItems(int nCol, BOOL bAscending, int low, int high);
    BOOL SortItems(PFNLVCOMPARE pfnCompare, int nCol, BOOL bAscending, LPARAM data,
                   int low, int high);

    CPoint GetPointClicked(int nRow, int nCol, const CPoint& point);

	void ValidateAndModifyCellContents(int nRow, int nCol, LPCTSTR strText);

// Overrrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(HMGridCtrl)
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

protected:
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    // Printing
	virtual void PrintFixedRowCells(int nStartColumn, int nStopColumn, int& row, CRect& rect,
                                    CDC *pDC, BOOL& bFirst);
    virtual void PrintColumnHeadings(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
    virtual void PrintRowButtons(CDC *pDC, CPrintInfo* /*pInfo*/);
#endif

#ifndef GRIDCONTROL_NO_DRAGDROP
    // Drag n' drop
    virtual CImageList* CreateDragImage(CPoint *pHotSpot);    // no longer necessary
#endif

    // Mouse Clicks
    virtual void  OnFixedColumnClick(HMCellID& cell);
    virtual void  OnFixedRowClick(HMCellID& cell);

    // Editing
    virtual void  OnEditCell(int nRow, int nCol, CPoint point, UINT nChar);
    virtual void  OnEndEditCell(int nRow, int nCol, CString str);
	virtual BOOL  ValidateEdit(int nRow, int nCol, LPCTSTR str);
    virtual void  EndEditing();

    // Drawing
    virtual void  OnDraw(CDC* pDC);

    // HMGridCellBase Creation and Cleanup
    virtual HMGridCellBase* CreateCell(int nRow, int nCol);
    virtual void DestroyCell(int nRow, int nCol);

// Attributes
protected:
    // General attributes
    COLORREF    m_crFixedTextColour, m_crFixedBkColour;
    COLORREF    m_crGridBkColour, m_crGridLineColour;
    COLORREF    m_crWindowText, m_crWindowColour, m_cr3DFace,     // System colours
                m_crShadow;
    COLORREF    m_crTTipBackClr, m_crTTipTextClr;                 // Titletip colours - FNA
    
    LPARAM      m_lParam;                                           // lParam for callback
    GRIDCALLBACK m_pfnCallback;                                     // The callback function

    int         m_nGridLines;
	int         m_nBarState;




    // Cell size details
	int         m_nRows, m_nFixedRows, m_nCols, m_nFixedCols;
    std::vector<int>  m_arRowHeights, m_arColWidths;
    int         m_nVScrollMax, m_nHScrollMax;

    // Fonts and images
    CRuntimeClass*   m_pRtcDefault; // determines kind of Grid Cell created by default
    HMGridDefaultCell m_cellDefault;  // "default" cell. Contains default colours, font etc.
    HMGridDefaultCell m_cellFixedColDef, m_cellFixedRowDef, m_cellFixedRowColDef;
    CFont       m_PrinterFont;  // for the printer
    CImageList* m_pImageList;

    // Cell data
    //CTypedPtrArray<CObArray, GRID_ROW*> m_RowData;
	std::vector<GRID_ROW*>m_RowData;

    // Mouse operations such as cell selection
    int         m_MouseMode;
    CPoint      m_LeftClickDownPoint, m_LastMousePoint;
    HMCellID     m_LeftClickDownCell, m_SelectionStartCell;
   mutable  HMCellID     m_idCurrentCell, m_idTopLeftCell;
    INT_PTR     m_nTimerID;
    int         m_nTimerInterval;
    int         m_nResizeCaptureRange;
    int         m_nRowsPerWheelNotch;
    CMap<DWORD,DWORD, HMCellID, HMCellID&> m_SelectedCellMap, m_PrevSelectedCellMap;

#ifndef GRIDCONTROL_NO_TITLETIPS
    HMTitleTip   m_TitleTip;             // Title tips for cells
#endif

    // Drag and drop
    HMCellID     m_LastDragOverCell;
#ifndef GRIDCONTROL_NO_DRAGDROP
    HMGridDropTarget m_DropTarget;       // OLE Drop target for the grid
#endif

    // Printing information
    CSize       m_CharSize;
    int         m_nPageHeight;
    CSize       m_LogicalPageSize,      // Page size in gridctrl units.
                m_PaperSize;            // Page size in device units.
    // additional properties to support Wysiwyg printing
    int         m_nPageWidth;
    int         m_nPrintColumn;
    int         m_nCurrPrintRow;
    int         m_nNumPages;
    int         m_nPageMultiplier;

    // sorting
    int          m_nSortColumn;
	PFNLVCOMPARE m_pfnCompare;



    // EFW - Added support for user-definable margins.  Top and bottom are in 
    // lines.  Left, right, and gap are in characters (avg width is used).
    int         m_nHeaderHeight, m_nFooterHeight, m_nLeftMargin,
                m_nRightMargin, m_nTopMargin, m_nBottomMargin, m_nGap;

protected:
    void SelectAllCells();
    void SelectColumns(HMCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void SelectRows(HMCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void SelectCells(HMCellID currentCell, BOOL bForceRedraw=FALSE, BOOL bSelectCells=TRUE);
    void OnSelecting(const HMCellID& currentCell);

    // Generated message map functions
    //{{AFX_MSG(HMGridCtrl)
    afx_msg void OnPaint();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
    //}}AFX_MSG
#ifndef _WIN32_WCE_NO_CURSOR
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
#endif
#ifndef _WIN32_WCE
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);    // EFW - Added
    afx_msg void OnSysColorChange();
#endif
#ifndef _WIN32_WCE_NO_CURSOR
    afx_msg void OnCaptureChanged(CWnd *pWnd);
#endif
#ifndef GRIDCONTROL_NO_CLIPBOARD
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
#endif
#if (_MFC_VER >= 0x0421) || (_WIN32_WCE >= 210)
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
#endif
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
#endif
    afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnImeChar(WPARAM wCharCode, LPARAM lParam);
    afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

	enum eMouseModes {
		MOUSE_NOTHING, MOUSE_SELECT_ALL, MOUSE_SELECT_COL, MOUSE_SELECT_ROW,
		MOUSE_SELECT_CELLS, MOUSE_SCROLLING_CELLS,
		MOUSE_OVER_ROW_DIVIDE, MOUSE_SIZING_ROW,
		MOUSE_OVER_COL_DIVIDE, MOUSE_SIZING_COL,
		MOUSE_PREPARE_EDIT,
#ifndef GRIDCONTROL_NO_DRAGDROP
		MOUSE_PREPARE_DRAG, MOUSE_DRAGGING
		//格式填充

#endif
    };
//      for sort in virtual mode, and column order, save and load layer
public:
	typedef std::vector<int> intlist;
	void Reorder(int From, int To);
	void SetVirtualCompare(PVIRTUALCOMPARE VirtualCompare);
	int m_CurCol;
	int GetLayer(int** pLayer); //  gives back the number of ints of the area (do not forget to delete *pLayer)
	void SetLayer(int* pLayer); // coming from a previous GetLayer (ignored if not same number of column, or the same revision number)
//    allow acces?
	intlist m_arRowOrder, m_arColOrder;
	static HMGridCtrl* m_This;
protected:
	virtual void AddSubVirtualRow(int Num, int Nb);
	int m_CurRow;
private:
	void ResetVirtualOrder();
	PVIRTUALCOMPARE m_pfnVirtualCompare;
	static bool NotVirtualCompare(int c1, int c2);
	UINT        m_nAutoSizeColumnStyle;

	//


	enum /*class*/ GRIDSTATE:unsigned long long
	{
		ALLOWROWRESIZE = 1,                           // 0_1
		ALLOWCOLUMNRESIZE = ALLOWROWRESIZE << 1,      // 1_2
		ALLOWDRAGANDDROP = ALLOWCOLUMNRESIZE << 1,    // 2_4
		ALLOWDRAW = ALLOWDRAGANDDROP << 1,            // 3_8
		ALLOWREORDERCOLUMN = ALLOWDRAW << 1,          // 4_16
		ALLOWCOLHIDE = ALLOWREORDERCOLUMN << 1,       // 5_32
		ALLOWROWHIDE = ALLOWCOLHIDE << 1,             // 6_64
		ALLOWSELECTROWINFIXEDCOL = ALLOWROWHIDE << 1, // 7_128
		ASCEND = ALLOWSELECTROWINFIXEDCOL << 1,       // 8_256
		AUTOSIZESKIPCOLHDR = ASCEND << 1,             // 9_512
		DOUBLEBUFFER = AUTOSIZESKIPCOLHDR << 1,       // 10_1024
		DROPCOPY = DOUBLEBUFFER << 1,                 // 11_2048
		EDITABLE = DROPCOPY << 1,                     // 12_4096
		ENABLESELECTION = EDITABLE << 1,              // 13_8192
		FIXEDROWSELECTION = ENABLESELECTION << 1,     // 14_16384
		FIXEDCOLUMNSELECTION = FIXEDROWSELECTION << 1,// 15_32768
		FRAMEFOCUS = FIXEDCOLUMNSELECTION << 1,       // 16_65536
		HANDLETABKEY = FRAMEFOCUS << 1,               // 17_131072
		HIDDENCOLUNHIDE = HANDLETABKEY << 1,          // 18_262144
		HIDDENROWUNHIDE = HIDDENCOLUNHIDE << 1,       // 19_524288
		INDESTRUCTOR = HIDDENROWUNHIDE << 1,          // 20_1048576
		LISTMODE = INDESTRUCTOR << 1,                 // 21_2097152
		MODIFIED = LISTMODE << 1,                     // 22_4194304
		QUITFOCUSONTAB = MODIFIED << 1,               // 23_8388608
		LMOUSEBUTTONDOWN = QUITFOCUSONTAB << 1,       // 24_16777216
		RMOUSEBUTTONDOWN = LMOUSEBUTTONDOWN << 1,     // 25_33554432
		// EFW - Added to support shaded/unshaded printout.  If true, colored                                              
		// cells will print as-is.  If false, all text prints as black on white.                                              
		SHADEDPRINTOUT = RMOUSEBUTTONDOWN << 1,       // 26_67108864
		SINGLECOLSELECTION = SHADEDPRINTOUT << 1,     // 27_134217728
		SINGLEROWSELECTION = SINGLECOLSELECTION << 1, // 28_268435456
		SORTONCLICK = SINGLEROWSELECTION << 1,        // 29_536870912
		STARTCUT = SORTONCLICK << 1,                  // 30_1073741824
		TITLETIPS = STARTCUT << 1,                    // 31_2147483648
		TRACKFOCUSCELL = TITLETIPS << 1,              // 32_4294967296
		VIRTUALMODE = TRACKFOCUSCELL << 1,            // 33_8589934592
		// EFW - New printing support functions                                              
		WYSIWYGPRINTING = VIRTUALMODE << 1,           // 34_17179869184
		DRAGROWMODE = WYSIWYGPRINTING << 1,           // 35_34359738368





	};
	unsigned long long m_state;
	public:
		bool IsRowResize() const;
		bool IsColumnResize() const;
		bool IsDragAndDrop() const;
		bool IsDraw() const;
		bool IsReorderColumn() const;
		bool IsRowHide() const;
		bool IsSelectRowInFixedCol() const;
		bool IsSortAscend() const;
		bool IsAutoSizeSkipColHdr() const;
		bool IsDoubleBuffer() const;
		bool IsDropCopy() const;
		bool IsEditable() const;
		bool IsEnableSelection() const;
		bool IsFixedColumnSelection() const;
		bool IsFrameFocus() const;
		bool IsHandleTabKey() const;
		bool IsHiddenRowUnhide() const;
		bool IsInDestructor() const;
		bool IsListMode() const;
		bool IsGridModified() const;
		bool IsQuitFocusOnTab() const;
		bool IsRMouseButtonDown() const;
		bool IsShadedPrintOut() const;
		bool IsSingleColSelection() const;
		bool IsSingleRowSelection() const;
		bool IsHeaderSort() const;
		bool IsStartCut() const;
		bool IsTitleTips() const;
		bool IsTrackFocusCell() const;
		bool IsVirtualMode() const;
		bool IsWysiwygPrinting() const;
		bool IsAllowColHide() const;
		bool IsFixedRowSelection() const;
		bool IsHiddenColUnhide() const;
		bool IsLMouseButtonDown() const;
		bool IsDragRowMode() const;

		//返回 m_state所指属性值 是否 改变，是否和  val 不同
		bool EnsureProChange(GRIDSTATE pro, bool val);
		//
		void EnsureRowResize(bool val);
		void EnsureColumnResize(bool val);
		void EnsureDragAndDrop(bool val=true);
		void EnsureDraw(bool val);
		void EnsureReorderColumn(bool val);
		void EnsureRowHide(bool val);
		void EnsureSelectRowInFixedCol(bool val=true);
		void EnsureSortAscend(bool val);
		void EnsureAutoSizeSkipColHdr(bool val);
		void EnsureDoubleBuffer(bool val);
		void EnsureDropCopy(bool val);
		void EnsureEditable(bool val=true);
		void EnsureSelection(bool val=true);
		void EnsureFixedColumnSelection(bool val);
		void EnsureFrameFocus(bool val);
		void EnsureHandleTabKey(bool val);
		void EnsureHiddenRowUnhide(bool val);
		void EnsureInDestructor(bool val);
		void EnsureListMode(bool val);
		void EnsureGridModified(bool val);
		void EnsureQuitFocusOnTab(bool val);// use only if GetParent() is a CDialog
		void EnsureRMouseButtonDown(bool val);
		void EnsureShadedPrintOut(bool val);
		void EnsureSingleColSelection(bool val=true);
		void EnsureSingleRowSelection(bool val=true);
		void EnsureHeaderSort(bool val);
		void EnsureStartCut(bool val);
		void EnsureTitleTips(bool val);
		void EnsureTrackFocusCell(bool val);
		void EnsureVirtualMode(bool val);
		void EnsureWysiwygPrinting(bool val);
		void EnsureColHide(bool val);
		void EnsureFixedRowSelection(bool val);
		void EnsureHiddenColUnhide(bool val);
		void EnsureLMouseButtonDown(bool val);
		void EnsureDragRowMode(bool val);

public:
	bool IsItemExitVScroll(const HMCellID& cell) const;
	bool IsItemExitVScroll(int row,int col) const;
	//
	int GetRowCount(HMCellID cell) const;
	int GetColCount(HMCellID cell) const;

};


/////////////////////////////////////////////////////////////////////////////



_HM_GridControl_END
#endif
