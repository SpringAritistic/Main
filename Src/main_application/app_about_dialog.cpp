#include "stdafx.h"

#include "app_about_dialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(app_about_dialog, app_dialog_base, 1)
BEGIN_MESSAGE_MAP(app_about_dialog, app_dialog_base)
	ON_BN_CLICKED(IDOK, on_button_ok_clicked)
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()


app_about_dialog::app_about_dialog(CWnd* parent /*=NULL*/) : app_dialog_base(IDD, parent)
{

}

app_about_dialog::~app_about_dialog()
{
}

void app_about_dialog::DoDataExchange(CDataExchange* pDX)
{
	app_dialog_base::DoDataExchange(pDX);
}

void app_about_dialog::layout_window()
{

}

void app_about_dialog::on_button_ok_clicked()
{
	app_dialog_base::OnOK();
}

void app_about_dialog::OnSize(UINT flag, int cx, int cy)
{
	layout_window();
}

void app_about_dialog::OnSizing(UINT flag, LPRECT rc)
{
	layout_window();
}

BOOL app_about_dialog::OnInitDialog()
{
	app_dialog_base::OnInitDialog();

	return TRUE;
}
