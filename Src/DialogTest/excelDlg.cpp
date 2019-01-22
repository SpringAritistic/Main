// excelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DialogTest.h"
#include "excelDlg.h"
#include "afxdialogex.h"


// excelDlg dialog

IMPLEMENT_DYNAMIC(excelDlg, CDialogEx)

excelDlg::excelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(excelDlg::IDD, pParent)
{
	m_Grid.RegisterWindowClass(theApp.m_hInstance);
	//
	
	init(m_Grid);
	initGrid();
}
void excelDlg::initGrid()
{
	HMGridCtrl* m_pGrid = new HMGridCtrl();
	m_pGrid->RegisterWindowClass(theApp.m_hInstance);


	CRect rect1,  rect3;
	m_Grid.GetClientRect(&rect1);
	//
	rect3 = rect1+CSize(rect1.Height(),0);

	CRect rect2(50,500,900,1500);
	m_pGrid->Create(rect2, this, IDC_GridControlNew);
	init(*m_pGrid);
	Invalidate();
}

void excelDlg::init(HMGridCtrl& grid)
{
	grid.EnsureEditable(true);
	grid.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));//黄色背景
	grid.SetRowCount(30); //初始为10行
	grid.SetColumnCount(8); //初始化为11列
	grid.SetFixedRowCount(1); //表头为一行
	grid.SetFixedColumnCount(1); //表头为一列
	for (int row = 0; row < grid.GetRowCount(); row++)
		for (int col = 0; col < grid.GetColumnCount(); col++)
		{
			//设置表格显示属性
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			grid.SetRowHeight(row, 25); //设置各行高          
			grid.SetColumnWidth(0, 64); //设置0列宽 
			grid.SetColumnWidth(col, 64); //设置各列宽
			if (row == 0 && col == 0) //第(0，0)格
			{
				Item.nFormat = DT_CENTER | DT_WORDBREAK;
				Item.strText.Format(_T("报表显示"), col);
			}
			else if (row < 1) //设置0行表头显示
			{
				Item.nFormat = DT_CENTER | DT_WORDBREAK;
				Item.strText.Format(_T(" 项目%d"), col);
			}
			else if (col < 1) //设置0列表头显示
			{
				if (row < grid.GetRowCount())
				{
					Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
					Item.strText.Format(_T("第%d次"), row);
				}
			}
			else
			{
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
				CString str;
				
				Item.strText.Format(_T("%d行%d列"), row,col);
			}
			grid.SetItem(&Item);
		}
	grid.Refresh();
}

excelDlg::~excelDlg()
{
}

void excelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GridControl, m_Grid);
	if (false)
	DDX_Control(pDX, IDC_GridControlNew, *m_pGrid);

}

 BOOL excelDlg::OnInitDialog()
{

	return	CDialogEx::OnInitDialog();

}

BEGIN_MESSAGE_MAP(excelDlg, CDialogEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// excelDlg message handlers


int excelDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}
