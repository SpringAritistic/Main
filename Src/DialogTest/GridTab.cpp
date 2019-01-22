// GridTab.cpp : implementation file
//

#include "stdafx.h"
#include "DialogTest.h"
#include "GridTab.h"
#include "afxdialogex.h"
#include "algorithm"
#include "iterator"
// GridTab dialog

IMPLEMENT_DYNAMIC(GridTab, CDialogEx)

GridTab::GridTab(CWnd* pParent /*=NULL*/)
: CDialogEx(GridTab::IDD, pParent), m_count(20), m_RowCount(20), m_ColCount(10), m_RowHeight(50), m_ColWidth(300)
{
	generate_n(back_inserter(m_vecGrid), m_count, [&](){return this->CreateTab(); });
}

GridTab::~GridTab()
{
}

void GridTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
void GridTab::RegisterClass(HMGridCtrl* pGrid)
{
	pGrid->RegisterWindowClass(AfxGetInstanceHandle());
}
BOOL GridTab::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(SW_MAXIMIZE);
	InitScrollbar();
	TabLoad();
	return TRUE;
}

void GridTab::TabLoad()
{
	for (size_t i = 0; i < m_vecGrid.size(); i++)
	{
		InitTab(i);
	}
}
HMGridCtrl* GridTab::CreateTab()
{
	HMGridCtrl* pGrid = new  HMGridCtrl();
	RegisterClass(pGrid);
	return pGrid;
}

HMGridCtrl* GridTab::GetTab(size_t index)
{
	if (m_vecGrid.size() == 0)
	{
		m_vecGrid.push_back(CreateTab());
		m_count = m_vecGrid.size();
	}

	if (index < m_vecGrid.size())
		return m_vecGrid[index];
	else
		return m_vecGrid.back();

}


void GridTab::InitScrollbar()
{
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	CRect rect;
	GetClientRect(&rect);
	scrollinfo.nPage = rect.Height();    //设置滑块大小
	scrollinfo.nMin = 1;
	scrollinfo.nMax = GetTabRect().Height()*m_count;
	scrollinfo.nPos = 10;
	//scrollinfo.nTrackPos = 1;     
	m_nYPos = scrollinfo.nPos;
	SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
}

CRect GridTab::GetTabRect()//相对大小
{
	int height = GetSystemMetrics(SM_CYVSCROLL);
	return CRect(0, 0, m_ColWidth*m_ColCount, m_RowHeight*m_RowCount +int( 0*GetSystemMetrics(SM_CYVSCROLL)));

}

void GridTab::InitTab(size_t index)
{
	int& rowHeight = m_RowHeight;
	int rowCount = m_RowCount;
	CRect rectBase = GetTabRect();
	CRect rect = rectBase;
	GetClientRect(&rectBase);
	rect.right = rectBase.right;

	if (index == 0)
	{
		//GetClientRect(rect);
		////rect.bottom/=2;
		//rect += CSize(0, 0);

	}
	else
	{
		GetTab(index - 1)->GetWindowRect(rect);
		ScreenToClient(&rect);
		rect += CSize(0, rect.Height());
	}

	HMGridCtrl& grid = *GetTab(index);
	size_t i = index + 1;

	grid.Create(rect, this, IDC_GridControl + index, WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL);
	//grid.SetVirtualMode(TRUE);
	grid.DeleteAllItems();
	//grid.SetDefaultCellType(RUNTIME_CLASS(HMGridCellCombo));//组合框

	grid.EnsureEditable(true);
	grid.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));//黄色背景
	grid.SetBkColor(RGB(255, 255, 255));
	grid.SetRowCount(m_RowCount); //初始为10行
	grid.SetColumnCount(m_ColCount); //初始化为11列
	grid.SetFixedRowCount(1); //表头为一行
	grid.SetFixedColumnCount(1); //表头为一列
	CString title;
	title.Format("第%d个", i);

	vector<CString> options;
	for (size_t i = 0; i < 50; ++i)
	{
		CString str;

		str.Format("Option%d", i + 1);
		options.push_back(str);
	}
	for (int row = 0; row < grid.GetRowCount(); row++)
	{
		grid.SetRowHeight(row, m_RowHeight); //设置各行高          
		for (int col = 0; col < grid.GetColumnCount(); col++)
		{
			//grid.SetColumnWidth(0, 64); //设置0列宽 
			//if (col==0)
			grid.SetColumnWidth(col, m_ColWidth); //设置各列宽

										  //设置表格显示属性
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;
			Item.col = col;

			if (row == 0 && col == 0) //第(0，0)格
			{
				Item.nFormat = DT_CENTER | DT_WORDBREAK;

				Item.strText.Format(title + _T("报表显示"), col);
			}
			else if (row < 1) //设置0行表头显示
			{
				Item.nFormat = DT_CENTER | DT_WORDBREAK;
				Item.strText.Format(_T("项目%d"), col);
			}
			else if (col < 1) //设置0列表头显示
			{
				if (row < grid.GetRowCount())
				{
					Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
					Item.strText.Format(_T("第%d次"), row);
				}
			}
			else if (col == 1)
			{
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
				switch (row)
				{
				case 1:
				{
					Item.strText = "组合框";
					grid.SetCellType(row, col, RUNTIME_CLASS(HMGridCellCombo));


					HMGridCellCombo *pCell = (HMGridCellCombo*)grid.GetCell(row, col);
					if (pCell)
					{
						pCell->SetOptions(options);
						Item.mask |= CBS_DROPDOWN;
						//pCell->SetStyle(CBS_DROPDOWN);
					}
					break;
				}
				case 2:
				{
					Item.strText = "2019";
					grid.SetCellType(row, col, RUNTIME_CLASS(HMGridCellNumeric));
					break;
				}
				case 3:
				{
					grid.SetCellType(row, col, RUNTIME_CLASS(HMGridCellDateTime));
					HMGridCellDateTime* pCell = (HMGridCellDateTime*)grid.GetCell(row, col);
					if (pCell)
					{
						pCell->SetTime(CTime::GetCurrentTime());
						Item.strText = pCell->GetText();
					}

					break;
				}
				case 4:
				{
					grid.SetCellType(row, col, RUNTIME_CLASS(HMGridCellDateTime));
					HMGridCellDateTime* pCell = (HMGridCellDateTime*)grid.GetCell(row, col);
					if (pCell)
					{
						pCell->Init(DTS_TIMEFORMAT);
						Item.strText = pCell->GetText();
					}

					break;
				}
				
				case 5:
				{
					Item.strText = "选择";
					grid.SetCellType(row, col, RUNTIME_CLASS(CGridCellCheck));
					break;
				}
				case 6:
				{
					Item.strText = "http://www.baidu.com";
					grid.SetCellType(row, col, RUNTIME_CLASS(HMGridURLCell));
					break;
				}
				case 7:
				{
					Item.strText = "Option1,Option2,Option5";
					grid.SetCellType(row, col, RUNTIME_CLASS(HMGridCellCheckCombo));
					HMGridCellCheckCombo* pCell = (HMGridCellCheckCombo*)grid.GetCell(row, col);
					if (pCell)
					{

						pCell->SetOptions(options);
					}
					break;
				}
				default:
				{
					Item.strText.Format(_T("%d行%d列"), row, col);
					break;
				}
				}


			}
			else
			{
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;

				Item.strText.Format(_T("%d行%d列"), row, col);
			}
			grid.SetItem(&Item);


		}
	}
	//grid.SetRowResize(TRUE);
	grid.EnsureColumnResize(TRUE);
	//grid.AutoSize(GVS_BOTH);
	grid.MergeCells(1, 5, 1, 9);
	grid.MergeCells(1, 4, 5, 5);
	grid.EnsureRowHide(TRUE);
	//
	HMGridCellBase*cell= grid.GetCell(2,5);
	CString str(cell->GetText());
	DWORD state=cell->GetState();
	cell->SetState(GVIS_FIXED | state | GVIS_READONLY);
	//
	long height = grid.GetVirtualHeight();
	long width = grid.GetVirtualWidth();
	//grid.AutoFill();
	SetWindowPos(&grid, rect.left, rect.top, height, width, SWP_SHOWWINDOW);
}

BEGIN_MESSAGE_MAP(GridTab, CDialogEx)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void GridTab::OnVScrollImp(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//TODO: Add your message handler code here and/or call default

	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	int unit = 20;//滚动时，移动像素数
	switch (nSBCode)
	{
	case SB_LINEUP:          //Scroll one line up
		scrollinfo.nPos -= unit;
		if (scrollinfo.nPos<scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, unit);
		break;
	case SB_LINEDOWN:           //Scroll one line down
		scrollinfo.nPos += unit;
		if (scrollinfo.nPos + int(scrollinfo.nPage)>scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -unit);
		break;
	case SB_PAGEUP:            //Scroll one page up.
		scrollinfo.nPos -= int(scrollinfo.nPage);
		if (scrollinfo.nPos <= scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, scrollinfo.nPage);
		break;
	case SB_PAGEDOWN:        //Scroll one page down        
		scrollinfo.nPos += int(scrollinfo.nPage);
		if (scrollinfo.nPos + int(scrollinfo.nPage) >= scrollinfo.nMax)  //此处一定要注意加上滑块的长度，再作判断
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -(int(scrollinfo.nPage)));
		break;
	case SB_ENDSCROLL:      //End scroll     
		break;
	case SB_THUMBPOSITION:  //Scroll to the absolute position. The current position is provided in nPos
		break;
	case SB_THUMBTRACK:                  //Drag scroll box to specified position. The current position is provided in nPos
		ScrollWindow(0, scrollinfo.nPos - nPos);
		scrollinfo.nPos = nPos;
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		break;
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

}




void GridTab::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//TODO: Add your message handler code here and/or call default

	return OnVScrollImp(nSBCode, nPos, pScrollBar);

	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
	int unit = 20;//滚动时，移动像素数
	int nPrevPos = scrollinfo.nPos;
	switch (nSBCode)
	{
	case SB_TOP:
		scrollinfo.nPos = scrollinfo.nMin;
		break;
	case SB_BOTTOM:
		scrollinfo.nPos = scrollinfo.nMax;
		break;
	case SB_LINEUP:
		scrollinfo.nPos -= unit;
		break;
	case SB_LINEDOWN:
		scrollinfo.nPos += unit;
		break;
	case SB_PAGEUP:
		scrollinfo.nPos -= scrollinfo.nPage;
		break;
	case SB_PAGEDOWN:
		scrollinfo.nPos += scrollinfo.nPage;
		break;
	case SB_THUMBTRACK:
		scrollinfo.nPos = nPos;
		break;
	default:
		break;
	}
	scrollinfo.fMask = SIF_POS;
	//if (scrollinfo.nPos < scrollinfo.nMin)
	//	scrollinfo.nPos = scrollinfo.nMin;
	//if (scrollinfo.nPos > scrollinfo.nMax)
	//	scrollinfo.nPos = scrollinfo.nMax;

	SetScrollInfo(SB_VERT, &scrollinfo, TRUE);
	if (scrollinfo.nPos != nPrevPos)
	{
		m_nYPos += scrollinfo.nPos - nPrevPos;
		ScrollWindow(0, -m_nYPos, NULL, NULL);
		Invalidate(FALSE);
		UpdateWindow();
	}

	CDialog::OnVScroll(nSBCode, nPrevPos, pScrollBar);
}




BOOL GridTab::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	OnVScroll(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void GridTab::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnClose();
}
