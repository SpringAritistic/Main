//11/22/2018  4:37:13 PM
#include "stdafx.h"

#include "GridCell.h"
#include "InPlaceEdit.h"
#include "GridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
_HM_GridControl_BEGIN
IMPLEMENT_DYNCREATE(HMGridCell, HMGridCellBase)
IMPLEMENT_DYNCREATE(HMGridDefaultCell, HMGridCell)

/////////////////////////////////////////////////////////////////////////////
// GridCell

HMGridCell::HMGridCell()
{
    m_plfFont = NULL;
	HMGridCell::Reset();
}

HMGridCell::~HMGridCell()
{
    delete m_plfFont;
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Attributes

void HMGridCell::operator=( HMGridCell& cell)
{
    if (this != &cell) HMGridCellBase::operator=(cell);
}

void HMGridCell::Reset()
{
    HMGridCellBase::Reset();

    m_strText.Empty();
    m_nImage   = -1;
    m_lParam   = NULL;           // BUG FIX J. Bloggs 20/10/03
    m_pGrid    = NULL;
    m_bEditing = FALSE;
    m_pEditWnd = NULL;

    m_nFormat = (DWORD)-1;       // Use default from HMGridDefaultCell
    m_crBkClr = CLR_DEFAULT;     // Background colour (or CLR_DEFAULT)
    m_crFgClr = CLR_DEFAULT;     // Forground colour (or CLR_DEFAULT)
    m_nMargin = (UINT)-1;        // Use default from HMGridDefaultCell

    delete m_plfFont;
    m_plfFont = NULL;            // Cell font
}

void HMGridCell::SetFont(const LOGFONT* plf)
{
    if (plf == NULL)
    {
        delete m_plfFont;
        m_plfFont = NULL;
    }
    else
    {
        if (!m_plfFont)
            m_plfFont = new LOGFONT;
        if (m_plfFont)
            memcpy(m_plfFont, plf, sizeof(LOGFONT)); 
    }
}

LOGFONT* HMGridCell::GetFont() const
{
    if (m_plfFont == NULL)
    {
        HMGridDefaultCell *pDefaultCell = (HMGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return NULL;

        return pDefaultCell->GetFont();
    }

    return m_plfFont; 
}

CFont* HMGridCell::GetFontObject() const
{
    // If the default font is specified, use the default cell implementation
    if (m_plfFont == NULL)
    {
        HMGridDefaultCell *pDefaultCell = (HMGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return NULL;

        return pDefaultCell->GetFontObject();
    }
    else
    {
        static CFont Font;
        Font.DeleteObject();
        Font.CreateFontIndirect(m_plfFont);
        return &Font;
    }
}

DWORD HMGridCell::GetFormat() const
{
    if (m_nFormat == (DWORD)-1)
    {
        HMGridDefaultCell *pDefaultCell = (HMGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return 0;

        return pDefaultCell->GetFormat();
    }

    return m_nFormat; 
}

UINT HMGridCell::GetMargin() const           
{
    if (m_nMargin == (UINT)-1)
    {
        HMGridDefaultCell *pDefaultCell = (HMGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return 0;

        return pDefaultCell->GetMargin();
    }

    return m_nMargin; 
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Operations

BOOL HMGridCell::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    if ( m_bEditing )
	{      
        if (m_pEditWnd)
		    m_pEditWnd->SendMessage ( WM_CHAR, nChar );    
    }  
	else  
	{   
		DWORD dwStyle = ES_LEFT;
		if (GetFormat() & DT_RIGHT) 
			dwStyle = ES_RIGHT;
		else if (GetFormat() & DT_CENTER) 
			dwStyle = ES_CENTER;
		
		m_bEditing = TRUE;
		
		// InPlaceEdit auto-deletes itself
		HMGridCtrl * pGrid = GetGrid();
		m_pEditWnd = new HMInPlaceEdit(pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
    }
    return TRUE;
}

void HMGridCell::EndEdit()
{
    if (m_pEditWnd)
        ((HMInPlaceEdit*)m_pEditWnd)->EndEdit();
}

void HMGridCell::OnEndEdit()
{
    m_bEditing = FALSE;
    m_pEditWnd = NULL;
}

void  HMGridCell::SetText(LPCTSTR szText)        { m_strText = szText; }
void  HMGridCell::SetImage(int nImage)           { m_nImage = nImage; }
void  HMGridCell::SetData(LPARAM lParam)         { m_lParam = lParam; }
void  HMGridCell::SetGrid(HMGridCtrl * pGrid)      { m_pGrid = pGrid; }
// void SetState(const DWORD nState);  -  use base class version   
void  HMGridCell::SetFormat(DWORD nFormat)       { m_nFormat = nFormat; }
void  HMGridCell::SetTextClr(COLORREF clr)       { m_crFgClr = clr; }
void  HMGridCell::SetBackClr(COLORREF clr)       { m_crBkClr = clr; }
void  HMGridCell::SetMargin(UINT nMargin)        { m_nMargin = nMargin; }
CWnd* HMGridCell::GetEditWnd() const             { return m_pEditWnd; }
void  HMGridCell::SetCoords(int /*nRow*/, int /*nCol*/) {}  // don't need to know the row and
// column for base implementation

LPCTSTR     HMGridCell::GetText() const           
{ 
	//if (m_strText.IsEmpty())
	//	return  _T("");
	return  LPCTSTR(m_strText); 
}
int         HMGridCell::GetImage() const            { return m_nImage; }
LPARAM      HMGridCell::GetData() const             { return m_lParam; }
HMGridCtrl* HMGridCell:: GetGrid() const             { return m_pGrid; }
// DWORD    GetState() const - use base class
COLORREF    HMGridCell::GetTextClr() const          { return m_crFgClr; } // TODO: change to use default cell
COLORREF    HMGridCell::GetBackClr() const          { return m_crBkClr; }

BOOL        HMGridCell::IsEditing() const           { return m_bEditing; }
BOOL        HMGridCell::IsDefaultFont() const       { return (m_plfFont == NULL); }







/////////////////////////////////////////////////////////////////////////////
// HMGridDefaultCell

HMGridDefaultCell::HMGridDefaultCell() 
{
#ifdef _WIN32_WCE
    m_nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#else
	m_nFormat = DT_LEFT /*| DT_VCENTER | DT_SINGLELINE */ | DT_NOPREFIX | DT_END_ELLIPSIS | DT_EDITCONTROL;
#endif
    m_crFgClr = CLR_DEFAULT;
    m_crBkClr = CLR_DEFAULT;
    m_Size    = CSize(30,10);
    m_dwStyle = 0;

#ifdef _WIN32_WCE
    LOGFONT lf;
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
    SetFont(&lf);
#else // not CE

    NONCLIENTMETRICS ncm;
#if defined(_MSC_VER) && (_MSC_VER < 1300)
    ncm.cbSize = sizeof(NONCLIENTMETRICS); // NONCLIENTMETRICS has an extra element after VC6
#else
    // Check the operating system's version
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if( !GetVersionEx((OSVERSIONINFO *) &osvi))
    {
    	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);	
        GetVersionEx ((OSVERSIONINFO *)&osvi);
    }
    
    if (osvi.dwMajorVersion > 5)
    	ncm.cbSize = sizeof(NONCLIENTMETRICS);
    else
	    ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth);
#endif
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));

    SetFont(&(ncm.lfMessageFont));
#endif
}

HMGridDefaultCell::~HMGridDefaultCell()
{
    m_Font.DeleteObject(); 
}

void HMGridDefaultCell::SetFont(const LOGFONT* plf)
{
    ASSERT(plf);

    if (!plf) return;

    m_Font.DeleteObject();
    m_Font.CreateFontIndirect(plf);

    HMGridCell::SetFont(plf);

    // Get the font size and hence the default cell size
    CDC* pDC = CDC::FromHandle(::GetDC(NULL));
    if (pDC)
    {
        CFont* pOldFont = pDC->SelectObject(&m_Font);

        SetMargin(pDC->GetTextExtent(_T(" "), 1).cx);
        m_Size = pDC->GetTextExtent(_T(" XXXXXXXXXXXX "), 14);
        m_Size.cy = (m_Size.cy * 3) / 2;

        pDC->SelectObject(pOldFont);
        ReleaseDC(NULL, pDC->GetSafeHdc());
    }
    else
    {
        SetMargin(3);
        m_Size = CSize(40,16);
    }
}

LOGFONT* HMGridDefaultCell::GetFont() const
{
    ASSERT(m_plfFont);  // This is the default - it CAN'T be NULL!
    return m_plfFont;
}

CFont* HMGridDefaultCell::GetFontObject() const
{
    ASSERT(m_Font.GetSafeHandle());
    return (CFont*) &m_Font; 
}

DWORD HMGridDefaultCell::GetStyle() const                      { return m_dwStyle; }
void  HMGridDefaultCell::SetStyle(DWORD dwStyle)               { m_dwStyle = dwStyle; }
int   HMGridDefaultCell::GetWidth() const                      { return m_Size.cx; }
int   HMGridDefaultCell::GetHeight() const                     { return m_Size.cy; }
void  HMGridDefaultCell::SetWidth(int nWidth)                  { m_Size.cx = nWidth; }
void  HMGridDefaultCell::SetHeight(int nHeight)                { m_Size.cy = nHeight; }

// Disable these properties
void     HMGridDefaultCell::SetData(LPARAM /*lParam*/)             { ASSERT(FALSE); }
void     HMGridDefaultCell::SetState(DWORD /*nState*/)             { ASSERT(FALSE); }
DWORD    HMGridDefaultCell::GetState() const                       { return HMGridCell::GetState() | GVIS_READONLY; }
void     HMGridDefaultCell::SetCoords(int /*row*/, int /*col*/)   { ASSERT(FALSE); }

_HM_GridControl_END
