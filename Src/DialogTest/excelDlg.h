#pragma once
#include "HMGridControl/HMGridControlInc.h"

// excelDlg dialog

class excelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(excelDlg)

public:
	excelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~excelDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_Excle };
	HMGridCtrl m_Grid;
	HMGridCtrl* m_pGrid;
public: 
	void init(HMGridCtrl& grid);
	void initGrid();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
