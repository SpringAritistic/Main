#include "StdAfx.h"
#include "../resource.h"

#include "local_resource.h"

#include "app_dialog_base.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(app_dialog_base, CDialogEx, 1)
BEGIN_MESSAGE_MAP(app_dialog_base, CDialogEx)
END_MESSAGE_MAP()

app_dialog_base::app_dialog_base() : CDialogEx()
{
}

app_dialog_base::app_dialog_base(UINT template_id, CWnd* parent /*= nullptr*/) : CDialogEx(template_id, parent)
{
}

app_dialog_base::app_dialog_base(LPCTSTR template_name, CWnd* parent /*= nullptr*/) : CDialogEx(template_name, parent)
{
}

app_dialog_base::~app_dialog_base()
{
}

BOOL app_dialog_base::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	set_window_icon_same_as_application(this);
	return TRUE;
}

void set_window_icon_same_as_application(CWnd* wnd)
{
	if (!wnd || !wnd->GetSafeHwnd()) return;

	HICON handle_icon = NULL;
	CWinApp* win_app = AfxGetApp();

	local_resource tmp;
	if (win_app) handle_icon = win_app->LoadIcon(IDI_ICON_APPLICATIN_DIALOG);

	wnd->SetIcon(handle_icon, FALSE);
	wnd->SetIcon(handle_icon, TRUE);
}
