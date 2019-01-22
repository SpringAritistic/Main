//11/22/2018  4:37:13 PM
#include "stdafx.h"
#include "CellRange.h"
_HM_GridControl_BEGIN

HMCellID::HMCellID(int nRow, int nCol) : row(nRow), col(nCol) {}

int HMCellID::IsValid() const { return (row >= 0 && col >= 0); }
int HMCellID::operator==(const HMCellID& rhs) const { return (row == rhs.row && col == rhs.col); }
int HMCellID::operator!=(const HMCellID& rhs) const { return !operator==(rhs); }


HMCellRange::HMCellRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol)
{
	Set(nMinRow, nMinCol, nMaxRow, nMaxCol);
}
int  HMCellRange::Count() { return (m_nMaxRow - m_nMinRow + 1) * (m_nMaxCol - m_nMinCol + 1); }
int  HMCellRange::GetMinRow() const { return m_nMinRow; }
void  HMCellRange::SetMinRow(int minRow) { m_nMinRow = minRow; }

int  HMCellRange::GetMinCol() const { return m_nMinCol; }
void  HMCellRange::SetMinCol(int minCol) { m_nMinCol = minCol; }

int  HMCellRange::GetMaxRow() const { return m_nMaxRow; }
void  HMCellRange::SetMaxRow(int maxRow) { m_nMaxRow = maxRow; }

int  HMCellRange::GetMaxCol() const { return m_nMaxCol; }
void  HMCellRange::SetMaxCol(int maxCol) { m_nMaxCol = maxCol; }

int  HMCellRange::GetRowSpan() const { return m_nMaxRow - m_nMinRow + 1; }
int  HMCellRange::GetColSpan() const { return m_nMaxCol - m_nMinCol + 1; }

void HMCellRange::Set(int minRow, int minCol, int maxRow, int maxCol)
{
	m_nMinRow = minRow;
	m_nMinCol = minCol;
	m_nMaxRow = maxRow;
	m_nMaxCol = maxCol;
}
void HMCellRange::operator=(const HMCellRange& rhs)
{
	if (this != &rhs) Set(rhs.m_nMinRow, rhs.m_nMinCol, rhs.m_nMaxRow, rhs.m_nMaxCol);
}

int HMCellRange::operator==(const HMCellRange& rhs)
{
	return ((m_nMinRow == rhs.m_nMinRow) && (m_nMinCol == rhs.m_nMinCol) &&
		(m_nMaxRow == rhs.m_nMaxRow) && (m_nMaxCol == rhs.m_nMaxCol));
}

int HMCellRange::operator!=(const HMCellRange& rhs)
{
	return !operator==(rhs);
}

int HMCellRange::IsValid() const
{
	return (m_nMinRow >= 0 && m_nMinCol >= 0 && m_nMaxRow >= 0 && m_nMaxCol >= 0 &&
		m_nMinRow <= m_nMaxRow && m_nMinCol <= m_nMaxCol);
}

int HMCellRange::InRange(int row, int col) const
{
	return (row >= m_nMinRow && row <= m_nMaxRow && col >= m_nMinCol && col <= m_nMaxCol);
}

int HMCellRange::InRange(const HMCellID& cellID) const
{
	return InRange(cellID.row, cellID.col);
}

HMCellID HMCellRange::GetTopLeft() const
{
	return HMCellID(m_nMinRow, m_nMinCol);
}

HMCellRange HMCellRange::Intersect(const HMCellRange& rhs) const
{
	return HMCellRange(max(m_nMinRow, rhs.m_nMinRow), max(m_nMinCol, rhs.m_nMinCol),
		min(m_nMaxRow, rhs.m_nMaxRow), min(m_nMaxCol, rhs.m_nMaxCol));
}

_HM_GridControl_END
