//11/22/2018  4:37:13 PM
#include "stdafx.h"
#include <numeric>
#include "CellRange.h"
_HM_GridControl_BEGIN

HMCellID::HMCellID(int nRow, int nCol) : row(nRow), col(nCol) {}
HMCellID::HMCellID(const HMCellID& id) : HMCellID(id.row, id.col){}
HMCellID::HMCellID(const HMCellRange& range)
{
	operator=(range);
}

void HMCellID::Init()
{
	return Reset(-1, -1);
}
void HMCellID::Reset(int nRow, int nCol)
{
	row = nRow;
	col = nCol;
}

bool HMCellID::IsValid() const { return (row >= 0 && col >= 0); }
bool HMCellID::operator==(const HMCellID& rhs) const { return (row == rhs.row && col == rhs.col); }
bool HMCellID::operator!=(const HMCellID& rhs) const { return !operator==(rhs); }
HMCellID& HMCellID::operator=(const HMCellID& rhs)
{
	row = rhs.row;
	col = rhs.col;
	return *this;
}
bool HMCellID::operator==(const HMCellRange& rhs) const
{
	return rhs == *this;
}
bool HMCellID::operator!=(const HMCellRange& rhs) const
{
	return rhs != *this;
}
HMCellID& HMCellID::operator=(const HMCellRange& rhs)
{
	row = rhs.GetMinRow();
	col = rhs.GetMinCol();
	return *this;
}
HMCellID::operator HMCellRange() const
{
	return HMCellRange(*this);
}

HMCellRange::HMCellRange(const HMCellID & id)
{
	Reset(id.row, id.col, id.row, id.col);
}


HMCellRange::HMCellRange(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol)
{
	Reset(nMinRow, nMinCol, nMaxRow, nMaxCol);
}
int  HMCellRange::Count() const { return (m_nMaxRow - m_nMinRow + 1) * (m_nMaxCol - m_nMinCol + 1); }
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
void  HMCellRange::Init()
{
	return Reset(-1, -1, -1, -1);
}
vector<int> HMCellRange::GetRows()const
{
	vector<int>vec(GetRowSpan());
	if (IsValid())
	{
		std::iota(vec.begin(), vec.end(), GetMinRow());
	}
	return vec;
}
vector<int> HMCellRange::GetCols()const
{
	vector<int>vec(GetColSpan());
	if (IsValid())
	{
		std::iota(vec.begin(), vec.end(), GetMinCol());
	}
	return vec;
}
void HMCellRange::Reset(int minRow, int minCol, int maxRow, int maxCol)
{
	m_nMinRow = minRow;
	m_nMinCol = minCol;
	m_nMaxRow = maxRow;
	m_nMaxCol = maxCol;
}
HMCellRange& HMCellRange::operator=(const HMCellRange& rhs)
{
	if (this != &rhs) Reset(rhs.m_nMinRow, rhs.m_nMinCol, rhs.m_nMaxRow, rhs.m_nMaxCol);
	return *this;
}
HMCellRange& HMCellRange::operator=(const HMCellID& id)
{
	Reset(id.row, id.col, id.row, id.col);
	return *this;

}
bool   HMCellRange::operator==(const HMCellID& rhs)const
{
	return ((m_nMinRow == rhs.row) && (m_nMinCol == rhs.col) &&
		(m_nMaxRow == rhs.row) && (m_nMaxCol == rhs.col));
}
bool   HMCellRange::operator!=(const HMCellID& rhs)const
{
	return  !operator==(rhs);
}
HMCellRange&  HMCellRange::operator+=(const HMCellID& rhs)
{
	return operator+=(HMCellRange(rhs));
}
HMCellRange&  HMCellRange::operator-=(const HMCellID& rhs)
{
	return operator -= (HMCellRange(rhs));

}
HMCellRange::operator HMCellID() const
{
	return HMCellID(*this);
}

bool HMCellRange::operator==(const HMCellRange& rhs)const
{
	return ((m_nMinRow == rhs.m_nMinRow) && (m_nMinCol == rhs.m_nMinCol) &&
		(m_nMaxRow == rhs.m_nMaxRow) && (m_nMaxCol == rhs.m_nMaxCol));
}

bool HMCellRange::operator!=(const HMCellRange& rhs)const
{
	return !operator==(rhs);
}
HMCellRange& HMCellRange::operator+=(const HMCellRange& rhs)
{
	if (rhs.IsValid())
	{
		m_nMinRow = min(m_nMinRow, rhs.m_nMinRow);
		m_nMinCol = min(m_nMinCol, rhs.m_nMinCol);
		m_nMaxRow = max(m_nMaxRow, rhs.m_nMaxRow);
		m_nMaxCol = max(m_nMaxCol, rhs.m_nMaxCol);
	}
	return *this;
}
HMCellRange& HMCellRange::operator-=(const HMCellRange& rhs)
{
	if (rhs.IsValid())
	{
		HMCellRange rect = Intersect(rhs);
		if (rect == *this)
		{
			this->Reset();
			return *this;
		}
		if (m_nMinRow == rect.m_nMinRow)
		{
			m_nMinRow = min(m_nMaxRow, rect.m_nMaxRow + 1);
		}
		else /*if (m_nMaxRow == rect.m_nMaxRow)*/
		{
			m_nMaxRow = max(m_nMinRow, rect.m_nMinRow - 1);
		}
		if (m_nMinCol == rect.m_nMinCol)
		{
			m_nMinCol = min(m_nMaxCol, rect.m_nMaxCol + 1);
		}
		else /*if (m_nMaxCol == rect.m_nMaxCol)*/
		{
			m_nMaxCol = max(m_nMinCol, rect.m_nMinCol - 1);
		}

	}
	return *this;
}
bool HMCellRange::IsValid() const
{
	return (m_nMinRow >= 0 && m_nMinCol >= 0 && m_nMaxRow >= 0 && m_nMaxCol >= 0 &&
		m_nMinRow <= m_nMaxRow && m_nMinCol <= m_nMaxCol);
}

bool HMCellRange::IsRangeIn(int row, int col) const
{
	return (row >= m_nMinRow && row <= m_nMaxRow && col >= m_nMinCol && col <= m_nMaxCol);
}

bool HMCellRange::IsRangeIn(const HMCellID& cellID) const
{
	return IsRangeIn(cellID.row, cellID.col);
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
bool HMRow::IsValid() const
{
	return m_row > 0 && m_cols.size() > 0 && m_cols.front() > 0;
}
vector<HMCellID> HMRow::GetCells()const
{
	vector<HMCellID>vec;
	if (IsValid())
	{
		for (const int & item : m_cols)
			vec.emplace_back(m_row, item);
	}
	return vec;
}
vector<HMCellRange>HMRow::GetRanges() const
{
	vector<HMCellRange>vecRange;
	if (IsValid())
	{
		vector<size_t>vec;
		std::adjacent_difference(m_cols.begin(), m_cols.end(), back_inserter(vec));

		size_t endIndex = 1;
		size_t startIndex = 0;
		auto itEnd = vec.end();
		do
		{
			itEnd = find_if_not(min(vec.end(), vec.begin() + endIndex), vec.end(), [](int val){ return val == 1; });
			endIndex = std::distance(vec.begin(), itEnd);
			vecRange.emplace_back(m_row, m_cols[startIndex], m_row, m_cols[endIndex-1]);
			startIndex = endIndex++;
			//
		} while (itEnd<vec.end());
	}
	return vecRange;

}

void HMRow::operator=(const HMRow& rhs)
{
	if (rhs.IsValid())
	{
		m_row = rhs.m_row;
		m_cols = rhs.m_cols;
	}

}
HMRow::HMRow(const HMRow& row) :HMRow()
{
	push_back(row);
}

void HMRow::push_back(const HMRow& row)
{
	if (!IsValid())
		operator=(row);
	else
	{
		if (row.IsValid() && m_row == row.m_row)
		{
			for (auto item : row.m_cols)
			{
				push_back(item);
			}
		}
	}
}

HMRow::HMRow(int row, int col) :m_row(row)
{
	push_back(col);
}

void HMExcle::push_back(const HMRow& row)
{
	if (!row.IsValid())
		return;
	auto it = find_if(m_data.begin(), m_data.end(), [&](const HMRow& item){return item.m_row == row.m_row; });
	if (it != m_data.end())
		(*it).push_back(row);
	else
	{
		it = find_if(m_data.begin(), m_data.end(), [&](const HMRow& item){return item.m_row > row.m_row; });
		//if (it != m_data.end())
		//	m_data.push_back(row);
		//else
			m_data.insert(it, row);
	}

}
void HMExcle::push_back(int row, int col)
{
	push_back(HMRow(row, col));
}
void HMExcle::push_back(const HMCellID& id)
{
	push_back(id.row, id.col);
}
void HMExcle::push_back(const HMCellRange& range)
{
	if (range.IsValid())
	{
		for (int i = range.GetMinRow(); i <= range.GetMaxRow(); ++i)
		{
			push_back(HMRow(i, range.GetCols()));
		}
	}
}
HMExcle::HMExcle(){}

void HMExcle::push_back(const HMExcle& excel)
{
	for (const auto& item : excel.m_data)
		push_back(item);
}



//Ö¸¶¨ ÐÐºÅ
void HMExcle::erase(int row)
{
	auto it = find_if(m_data.begin(), m_data.end(), [&](const HMRow& item){return item.m_row == row; });
	if (it != m_data.end())
		m_data.erase(it);
}
HMCellRange HMExcle::GetFirstRange() const
{
	vector<HMCellRange>& ranges = GetRanges();
	if (ranges.size() > 0)
	{
		return ranges.front();
	}
	return HMCellRange();
}
vector<int>HMExcle::GetRows()const
{
	vector<int>vec;
	for (const HMRow& row : m_data)
	{
		vec.push_back(row.m_row);
	}
	return vec;
}

vector<HMCellRange>HMExcle::GetRanges() const
{
	vector<HMCellRange>vec;

	for (const HMRow& row : m_data)
	{
		vector<HMCellRange>& vecRange = row.GetRanges();
		vec.insert(vec.end(), vecRange.begin(), vecRange.end());
	}
	return vec;
}
vector<HMRow>& HMExcle::GetData()
{
	return m_data;
}
//
IDPOSE& PosOr(IDPOSE& pos1, const IDPOSE& pos2)
{
	if (IsPosOut(pos2))
		pos1 = POSOUT;
	 else if (IsPosOut(pos1))
		 pos1 = IDPOSE(pos2 | POSIN );
	else
		pos1 = IDPOSE(pos1 | pos2 | POSIN);
	 return pos1;
}

IDPOSE GetLinePos(const HMCellID& id, int val, bool isCol)
{
	if (!id.IsValid() || val<0)
		return POSOUT;
	if (isCol)
		return val == id.col ||  val == id.col+1 ? POSIN : POSOUT;
	else
		return val == id.row || val == id.row + 1 ? POSIN : POSOUT;

}
IDPOSE GetLineColPos(const HMCellID& id, int col)
{
	return GetLinePos(id,col,true);
}
IDPOSE GetLineRowPos(const HMCellID& id, int row)
{
	return GetLinePos(id, row, false);

}
IDPOSE GetLinePos(const HMCellRange& id, int val, bool isCol)
{
	if (!id.IsValid() || val < 0)
		return POSOUT;
	if (isCol)
		return val >= id.GetMinCol() && val <= id.GetMaxCol() + 1 ? POSIN : POSOUT;
	else
		return val >= id.GetMinRow() && val <= id.GetMaxRow() + 1 ? POSIN : POSOUT;
}
IDPOSE GetLineColPos(const HMCellRange& id, int col)
{
	return GetLinePos(id,col,true);
}
IDPOSE GetLineRowPos(const HMCellRange& id, int row)
{
	return GetLinePos(id, row, false);
}
IDPOSE GetPos(const HMCellRange& range, int val, bool isCol)
{
	IDPOSE pos = POSOUT;
	if (isCol)
	{
		if (val == range.GetMinCol())
			PosOr(pos,POSLEFT);
		if (val == range.GetMaxCol())
			PosOr(pos,POSRIGHT);
		if (val<range.GetMaxCol() && val>range.GetMinCol())
			PosOr(pos, POSCOLIN);
	}
	else
	{
		if (val == range.GetMinRow())
			PosOr(pos, POSTOP);
		if (val == range.GetMaxRow())
			PosOr(pos, POSBOT);
		if (val<range.GetMaxRow() && val>range.GetMinRow())
			PosOr(pos, POSROWIN);
	}
	return pos;
}
IDPOSE GetColPos(const HMCellRange& range, int val)
{
	return GetPos(range,val,true);
}
IDPOSE GetRowPos(const HMCellRange& range, int val)
{
	return GetPos(range, val, false);
}
IDPOSE GetPos(const HMCellRange& range, int row, int col)
{
	IDPOSE pos = GetColPos(range, col);
	return PosOr(pos, GetRowPos(range, col));
}

bool IsPosOut(IDPOSE pos)
{
	return pos & POSOUT;
}
bool IsPosIn(IDPOSE pos)
{
	return !IsPosOut(pos);
}
_HM_GridControl_END
