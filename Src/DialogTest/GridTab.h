#pragma once
#include "HMGridControl/HMGridControlInc.h"
#include "vector"
// GridTab dialog
using namespace std;
using namespace HM_GridControl;
class GridTab : public CDialogEx
{
	DECLARE_DYNAMIC(GridTab)

public:
	GridTab(CWnd* pParent = NULL);   // standard constructor
	virtual ~GridTab();

// Dialog Data
	enum { IDD = IDD_DIALOGTAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	vector< HMGridCtrl*> m_vecGrid;
	size_t m_count;
	void RegisterClass(HMGridCtrl* pGrid);
	void InitTab(size_t index=0);
	CRect GetTabRect();//相对大小
	void InitScrollbar();

public:
	afx_msg void TabLoad();
	
	HMGridCtrl* GetTab(size_t index = 0);
	HMGridCtrl* CreateTab();
	size_t m_RowCount;
	size_t m_ColCount;
	int m_RowHeight;
	int m_ColWidth;
public:
	 BOOL OnInitDialog();

	 afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	 afx_msg void OnVScrollImp(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	 afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
private:
	int m_nYPos;

public:
	afx_msg void OnClose();
};
