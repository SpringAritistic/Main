#include "stdafx.h"
#include "afxdialogex.h"
#include "../tools/string_tools.h"

#include "app_dialog_input.h"


IMPLEMENT_DYNAMIC(app_dialog_input, CXTPResizeDialog)
BEGIN_MESSAGE_MAP(app_dialog_input, CXTPResizeDialog)
END_MESSAGE_MAP()

app_dialog_input::app_dialog_input(CWnd* pParent /*=NULL*/)
	: CXTPResizeDialog(app_dialog_input::IDD, pParent)
{
	m_default_left_pos = 0;
	m_default_top_pos = 0;
	m_default_width = 310;
	m_checking_boot_result_name = false;
}


app_dialog_input::~app_dialog_input()
{

}

BOOL app_dialog_input::OnInitDialog()
{
	CXTPResizeDialog::OnInitDialog();
	set_window_icon_same_as_application(this);

	SetWindowText(m_caption.IsEmpty() ? _T("输入") : m_caption);

	CWnd* label = GetDlgItem(IDC_STATIC);
	if (label->GetSafeHwnd())
		label->SetWindowText(m_label.IsEmpty() ? _T("请输入内容") : m_label);
	
	if (m_checking_boot_result_name)
		m_result_name = string_tools::generate_unique_name(m_exist_names, m_result_name);

	CEdit* pedit = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
	pedit->SetWindowText(m_result_name);

	adjust_window_layout();

	CRect wnd_rect;
	GetWindowRect(&wnd_rect);
	wnd_rect.right = wnd_rect.left + max(m_default_width, 310);

	CRect wk_rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, (PVOID) &wk_rect, NULL);

	if (wnd_rect.left <= 0)
		wnd_rect.MoveToX(max(0, (wk_rect.Width() - wnd_rect.Width()) / 2));

	if (wnd_rect.top <= 0)
		wnd_rect.MoveToY(max(0, (wk_rect.Height() - wnd_rect.Height()) / 2));

	MoveWindow(&wnd_rect);

	return TRUE;
}

void app_dialog_input::DoDataExchange(CDataExchange* pDX)
{
	CXTPResizeDialog::DoDataExchange(pDX);
}

void app_dialog_input::OnOK()
{
	CEdit* pedit = (CEdit*)GetDlgItem(IDC_EDIT_INPUT);
	CString tmp_name;
	pedit->GetWindowText(tmp_name);

	for (auto& x : m_exist_names)
	{
		if (0 == tmp_name.CompareNoCase(x))
		{
			MessageBox(m_repeat_message.IsEmpty() ? _T("输入名称已存在，请重新输入") : m_repeat_message);
			pedit->SetFocus();
			return;
		}
	}

	m_result_name = tmp_name;
	CXTPResizeDialog::OnOK();
}

void app_dialog_input::adjust_window_layout()
{
	SetResize(IDC_STATIC, CXTPResizePoint(0, 0), CXTPResizePoint(1, 1));
	SetResize(IDC_EDIT_INPUT, CXTPResizePoint(0, 0), CXTPResizePoint(1, 1));
	SetResize(IDOK, CXTPResizePoint(1, 1), CXTPResizePoint(1, 1));
}
