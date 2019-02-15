//11/22/2018  4:37:13 PM
#pragma  once
#ifndef _GRIDCELL_GRIDCONTROL_H_
#define _GRIDCELL_GRIDCONTROL_H_

_HM_GridControl_BEGIN
class HMGridCtrl;
// Each cell contains one of these. Fields "row" and "column" are not stored since we
// will usually have acces to them in other ways, and they are an extra 8 bytes per
// cell that is probably unnecessary.

class HM_GridControl_EXT HMGridCell : public HMGridCellBase
{
    friend class HMGridCtrl ;
    DECLARE_DYNCREATE(HMGridCell)

// Construction/Destruction
public:
    HMGridCell();
    virtual ~HMGridCell();

// Attributes
public:
    void operator=( HMGridCell& cell);

	virtual void  SetText(LPCTSTR szText);
	virtual void  SetImage(int nImage);
	virtual void  SetData(LPARAM lParam);
	virtual void  SetGrid(HMGridCtrl * pGrid);
    // virtual void SetState(const DWORD nState);  -  use base class version   
	virtual void  SetFormat(DWORD nFormat);
	virtual void  SetTextClr(COLORREF clr);
	virtual void  SetBackClr(COLORREF clr);
    virtual void  SetFont(const LOGFONT* plf);
	virtual void  SetMargin(UINT nMargin);
	virtual CWnd* GetEditWnd() const;
	virtual void  SetCoords(int /*nRow*/, int /*nCol*/); // don't need to know the row and
                                                            // column for base implementation

	virtual LPCTSTR     GetText() const;
	virtual int         GetImage() const;
	virtual LPARAM      GetData() const;
	virtual HMGridCtrl*  GetGrid() const;
    // virtual DWORD    GetState() const - use base class
    virtual DWORD       GetFormat() const;
	virtual COLORREF    GetTextClr() const; // TODO: change to use default cell
	virtual COLORREF    GetBackClr() const;
    virtual LOGFONT*    GetFont() const;
    virtual CFont*      GetFontObject() const;
    virtual UINT        GetMargin() const;

	virtual BOOL        IsEditing() const;
	virtual BOOL        IsDefaultFont() const;
    virtual void        Reset();

// editing cells
public:
    virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void EndEdit();
protected:
    virtual void OnEndEdit();

protected:
    CString    m_strText;      // Cell text (or binary data if you wish...)
    LPARAM     m_lParam;       // 32-bit value to associate with item
    int        m_nImage;       // Index of the list view item¡¯s icon
    DWORD      m_nFormat;
    COLORREF   m_crFgClr;
    COLORREF   m_crBkClr;
    LOGFONT*   m_plfFont;
    UINT       m_nMargin;

    BOOL       m_bEditing;     // Cell being edited?

    HMGridCtrl * m_pGrid;        // Parent grid control
    CWnd*      m_pEditWnd;
};

// This class is for storing grid default values. It's a little heavy weight, so
// don't use it in bulk 
class HMGridDefaultCell : public HMGridCell
{
    DECLARE_DYNCREATE(HMGridDefaultCell)

// Construction/Destruction
public:
    HMGridDefaultCell();
    virtual ~HMGridDefaultCell();

public:
	virtual DWORD GetStyle() const;
	virtual void  SetStyle(DWORD dwStyle);
	virtual int   GetWidth() const;
	virtual int   GetHeight() const;
	virtual void  SetWidth(int nWidth);
	virtual void  SetHeight(int nHeight);

    // Disable these properties
	virtual void     SetData(LPARAM /*lParam*/);
	virtual void     SetState(DWORD /*nState*/);
	virtual DWORD    GetState() const;
	virtual void     SetCoords(int /*row*/, int /*col*/);
    virtual void     SetFont(const LOGFONT* /*plf*/);
    virtual LOGFONT* GetFont() const;   
    virtual CFont*   GetFontObject() const;

protected:
    CSize m_Size;       // Default Size
    CFont m_Font;       // Cached font
    DWORD m_dwStyle;    // Cell Style - unused

};




_HM_GridControl_END
#endif
