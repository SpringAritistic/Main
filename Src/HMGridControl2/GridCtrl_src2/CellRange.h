//11/22/2018  4:30:40 PM
#pragma  once
#ifndef _CELLRANGE_GRIDCONTROL_H_
#define _CELLRANGE_GRIDCONTROL_H_
_HM_GridControl_BEGIN
// The code contained in this file is based on the original
// WorldCom Grid control written by Joe Willcoxson,
//      mailto:chinajoe@aol.com
//      http://users.aol.com/chinajoe
class HM_GridControl_EXT HMCellID
{    
// Attributes
public:
    int row, col;	

// Operations
public:
    explicit HMCellID(int nRow = -1, int nCol = -1) : row(nRow), col(nCol) {}

    int IsValid() const { return (row >= 0 && col >= 0); }
    int operator==(const HMCellID& rhs) const { return (row == rhs.row && col == rhs.col); }
    int operator!=(const HMCellID& rhs) const { return !operator==(rhs); }
};

class HM_GridControl_EXT HMCellRange
{ 
public:
    
    HMCellRange(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1)
    {
        Set(nMinRow, nMinCol, nMaxRow, nMaxCol);
    }

    void Set(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1);
    
    int  IsValid() const;
    int  InRange(int row, int col) const;
    int  InRange(const HMCellID& cellID) const;
    int  Count() { return (m_nMaxRow - m_nMinRow + 1) * (m_nMaxCol - m_nMinCol + 1); }
    
    HMCellID  GetTopLeft() const;
    HMCellRange  Intersect(const HMCellRange& rhs) const;
    
    int GetMinRow() const {return m_nMinRow;}
    void SetMinRow(int minRow) {m_nMinRow = minRow;}
    
    int GetMinCol() const {return m_nMinCol;}
    void SetMinCol(int minCol) {m_nMinCol = minCol;}
    
    int GetMaxRow() const {return m_nMaxRow;}
    void SetMaxRow(int maxRow) {m_nMaxRow = maxRow;}
    
    int GetMaxCol() const {return m_nMaxCol;}
    void SetMaxCol(int maxCol) {m_nMaxCol = maxCol;}

    int GetRowSpan() const {return m_nMaxRow - m_nMinRow + 1;}
    int GetColSpan() const {return m_nMaxCol - m_nMinCol + 1;}
    
    void operator=(const HMCellRange& rhs);
    int  operator==(const HMCellRange& rhs);
    int  operator!=(const HMCellRange& rhs);
    
protected:
    int m_nMinRow;
    int m_nMinCol;
    int m_nMaxRow;
    int m_nMaxCol;
};

inline void HMCellRange::Set(int minRow, int minCol, int maxRow, int maxCol)
{
     m_nMinRow = minRow;
     m_nMinCol = minCol;
     m_nMaxRow = maxRow;
     m_nMaxCol = maxCol;
}

inline void HMCellRange::operator=(const HMCellRange& rhs)
{
    if (this != &rhs) Set(rhs.m_nMinRow, rhs.m_nMinCol, rhs.m_nMaxRow, rhs.m_nMaxCol);
}

inline int HMCellRange::operator==(const HMCellRange& rhs)
{
     return ((m_nMinRow == rhs.m_nMinRow) && (m_nMinCol == rhs.m_nMinCol) &&
             (m_nMaxRow == rhs.m_nMaxRow) && (m_nMaxCol == rhs.m_nMaxCol));
}

inline int HMCellRange::operator!=(const HMCellRange& rhs)
{
     return !operator==(rhs);
}

inline int HMCellRange::IsValid() const
{
     return (m_nMinRow >= 0 && m_nMinCol >= 0 && m_nMaxRow >= 0 && m_nMaxCol >= 0 &&
             m_nMinRow <= m_nMaxRow && m_nMinCol <= m_nMaxCol);
}

inline int HMCellRange::InRange(int row, int col) const
{
     return (row >= m_nMinRow && row <= m_nMaxRow && col >= m_nMinCol && col <= m_nMaxCol);
}

inline int HMCellRange::InRange(const HMCellID& cellID) const
{
     return InRange(cellID.row, cellID.col);
}

inline HMCellID HMCellRange::GetTopLeft() const
{
     return HMCellID(m_nMinRow, m_nMinCol);
}

inline HMCellRange HMCellRange::Intersect(const HMCellRange& rhs) const
{
     return HMCellRange(max(m_nMinRow,rhs.m_nMinRow), max(m_nMinCol,rhs.m_nMinCol),
                       min(m_nMaxRow,rhs.m_nMaxRow), min(m_nMaxCol,rhs.m_nMaxCol));
}
_HM_GridControl_END
#endif