//11/22/2018  4:37:13 PM
#pragma  once
#ifndef _CELLRANGE_GRIDCONTROL_H_
#define _CELLRANGE_GRIDCONTROL_H_
_HM_GridControl_BEGIN
class HM_GridControl_EXT HMCellID
{    
// Attributes
public:
    int row, col;	

// Operations
public:
	explicit HMCellID(int nRow = -1, int nCol = -1);

	int IsValid() const;
	int operator==(const HMCellID& rhs) const;
	int operator!=(const HMCellID& rhs) const;
};

class HM_GridControl_EXT HMCellRange
{ 
public:
    
	HMCellRange(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1);

    void Set(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1);
    
    int  IsValid() const;
    int  InRange(int row, int col) const;
    int  InRange(const HMCellID& cellID) const;
	int  Count();
    
    HMCellID  GetTopLeft() const;
    HMCellRange  Intersect(const HMCellRange& rhs) const;
    
	int GetMinRow() const;
	void SetMinRow(int minRow);
    
	int GetMinCol() const;
	void SetMinCol(int minCol);
    
	int GetMaxRow() const;
	void SetMaxRow(int maxRow);
    
	int GetMaxCol() const;
	void SetMaxCol(int maxCol);

	int GetRowSpan() const;
	int GetColSpan() const;
    
    void operator=(const HMCellRange& rhs);
    int  operator==(const HMCellRange& rhs);
    int  operator!=(const HMCellRange& rhs);
    
protected:
    int m_nMinRow;
    int m_nMinCol;
    int m_nMaxRow;
    int m_nMaxCol;
};



_HM_GridControl_END
#endif
