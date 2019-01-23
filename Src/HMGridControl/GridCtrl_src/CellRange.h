//11/22/2018  4:37:13 PM
#pragma  once
#ifndef _CELLRANGE_GRIDCONTROL_H_
#define _CELLRANGE_GRIDCONTROL_H_
#include <algorithm>
#include <vector>
using namespace std;
_HM_GridControl_BEGIN


class  HMCellRange;

class HM_GridControl_EXT HMCellID
{
	// Attributes
public:
	int row, col;

	// Operations
public:
	explicit HMCellID(int nRow = -1, int nCol = -1);
	HMCellID(const HMCellID& id);
	HMCellID(const HMCellRange& range);
	void Reset(int nRow = -1, int nCol = -1);
	void Init();

	bool IsValid() const;
	//
	HMCellID& operator=(const HMCellID& rhs);
	bool operator==(const HMCellID& rhs) const;
	bool operator!=(const HMCellID& rhs) const;
	//////
	HMCellID& operator=(const HMCellRange& rhs);
	bool operator==(const HMCellRange& rhs) const;
	bool operator!=(const HMCellRange& rhs) const;
	//隐式转换
	operator HMCellRange() const;
};
enum IDPOSE{
	 POSNULL=0, POSOUT = 1, POSIN = POSOUT << 1, //针对于 线<->单元格<->区域
	//下面的全为：单元格<->区域内
	POSLEFT = POSIN << 1, POSRIGHT = POSLEFT << 1, POSTOP = POSRIGHT<<1,
	POSBOT = POSTOP << 1,
	POSCOLIN = POSBOT << 1, POSROWIN = POSCOLIN << 1,
	POSLEFTTOP = POSTOP | POSLEFT, POSTOPRIGHT = POSTOP | POSRIGHT,
	POSRIGHTBOT = POSRIGHT | POSBOT, POSBOTLEFT = POSBOT | POSLEFT
};

class HM_GridControl_EXT HMCellRange
{
public:

	HMCellRange(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1);
	HMCellRange(const HMCellID & id);

	void Reset(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1);
	void Init();

	bool  IsValid() const;


	bool  IsRangeIn(int row, int col) const;
	bool  IsRangeIn(const HMCellID& cellID) const;


	int  Count() const;

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

	HMCellRange& operator=(const HMCellRange& rhs);
	bool  operator==(const HMCellRange& rhs)const;
	bool  operator!=(const HMCellRange& rhs)const;
	HMCellRange& operator+=(const HMCellRange& rhs);
	HMCellRange& operator-=(const HMCellRange& rhs);

	HMCellRange& operator=(const HMCellID& rhs);
	bool  operator==(const HMCellID& rhs)const;
	bool  operator!=(const HMCellID& rhs)const;
	HMCellRange& operator+=(const HMCellID& rhs);
	HMCellRange& operator-=(const HMCellID& rhs);
	vector<int> GetRows()const;
	vector<int> GetCols()const;
	//隐式转换
	operator HMCellID() const;
protected:
	int m_nMinRow;
	int m_nMinCol;
	int m_nMaxRow;
	int m_nMaxCol;
};

class HM_GridControl_EXT HMRow
{
public:
	HMRow(int row = -1, int col = -1);
	HMRow(const HMRow& row);
public:
	//添加
	template <class T>
	void push_back(const T& col)
	{
		int val(col);
		if (val < 0)
			return;
		if (find(m_cols.begin(), m_cols.end(), val) == m_cols.end())
		{
			auto it = find_if(m_cols.begin(), m_cols.end(), [=](int item){return item > val; });
			if (m_cols.end() == it)
			{
				m_cols.push_back(val);
			}
			else
			{
				m_cols.insert(it, val);
			}
		}
	}
public:

	template < typename T>
	HMRow(int row, const T & col)
	{
		m_row = row;
		push_back(col);
	}
	//
	template < typename T>
	HMRow(int row, const std::vector<T, std::allocator<T>> & cols = std::vector<T, std::allocator<T>>())
	{
		m_row = row;
		for (auto item : cols)
			push_back(item);
	}

	template <class T>
	void erase(T col)
	{
		int val(col);
		auto it = find(m_cols.begin(), m_cols.end(), val);

		m_cols.erase(it);
	}
	void push_back(const HMRow& row);
public:
	bool IsValid() const;
	void operator=(const HMRow& rhs);
	vector<HMCellID>GetCells()const;
	vector<HMCellRange>GetRanges() const;
private:
	std::vector<int> m_cols;
	int m_row;
private:
	friend class HMExcle;
};
class HM_GridControl_EXT HMExcle
{
public:
	HMExcle();
public:
	void push_back(const HMRow& row);
	void push_back(int row, int col);
	void push_back(const HMCellID& id);
	void push_back(const HMCellRange& range);
	void push_back(const HMExcle& excel);
	//指定 行号
	void erase(int row);
	vector<int>GetRows()const;
	//
	HMCellRange GetFirstRange() const;
	vector<HMCellRange>GetRanges() const;
	vector<HMRow>& GetData();
public:
	vector<HMRow> m_data;
};

//通用方式

// 线<->单元格
HM_GridControl_EXT IDPOSE GetLinePos(const HMCellID& id, int val, bool isCol);
HM_GridControl_EXT IDPOSE GetLineColPos(const HMCellID& id, int col);
HM_GridControl_EXT IDPOSE GetLineRowPos(const HMCellID& id, int row);
HM_GridControl_EXT IDPOSE GetLinePos(const HMCellRange& id, int val, bool isCol);
HM_GridControl_EXT IDPOSE GetLineColPos(const HMCellRange& id, int col);
HM_GridControl_EXT IDPOSE GetLineRowPos(const HMCellRange& id, int row);
//单元格<->区域
HM_GridControl_EXT IDPOSE GetPos(const HMCellRange& range, int val, bool isCol);
HM_GridControl_EXT IDPOSE GetColPos(const HMCellRange& range, int col);
HM_GridControl_EXT IDPOSE GetRowPos(const HMCellRange& range, int row);
HM_GridControl_EXT IDPOSE GetPos(const HMCellRange& range, int row, int col);
//
HM_GridControl_EXT IDPOSE& PosOr(IDPOSE& pos1, const IDPOSE& pos2);
//
HM_GridControl_EXT bool IsPosIn(IDPOSE pos);
HM_GridControl_EXT bool IsPosOut(IDPOSE pos);
_HM_GridControl_END
#endif
