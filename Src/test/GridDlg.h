#pragma once
#include "HMGridControl/GridCtrl.h"

// CGridDlg 对话框

class CGridDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGridDlg)

public:
	CGridDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGridDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDC_GridControl };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl m_Grid;
};
