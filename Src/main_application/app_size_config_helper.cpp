#include "stdafx.h"
#include "main_application.h"
#include "main_frame.h"

#include "app_size_config_helper.h"


app_size_config_helper::app_size_config_helper()
{
}


app_size_config_helper::~app_size_config_helper()
{
}

CRect app_size_config_helper::get_last_main_frame_pos_and_size()
{
	int left = the_main_app.GetProfileInt(_T("window_size"), _T("main_frame_normal_size_left"), 0);
	int top = the_main_app.GetProfileInt(_T("window_size"), _T("main_frame_normal_size_top"), 0);
	int right = the_main_app.GetProfileInt(_T("window_size"), _T("main_frame_normal_size_right"), 800);
	int bottom = the_main_app.GetProfileInt(_T("window_size"), _T("main_frame_normal_size_bottom"), 600);

	const int min_width = 100;
	const int min_height = 100;

	CRect work_area;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &work_area, NULL);
	if (left < 0) left = 0;
	if (left > work_area.Width() - min_width) left = work_area.Width() - min_width;
	if (right < min_width) right = min_width;
	if (right > work_area.Width()) right = work_area.Width();
	if (top < 0) top = 0;
	if (top > work_area.Height() - min_height) top = work_area.Height() - min_height;
	if (bottom < min_height) bottom = min_height;
	if (bottom > work_area.Height()) bottom = work_area.Height();

	return CRect(left, top, right, bottom);
}

void app_size_config_helper::save_last_main_frame_pos_and_size(const CRect pos)
{
	the_main_app.WriteProfileInt(_T("window_size"), _T("main_frame_normal_size_left"), pos.left);
	the_main_app.WriteProfileInt(_T("window_size"), _T("main_frame_normal_size_top"), pos.top);
	the_main_app.WriteProfileInt(_T("window_size"), _T("main_frame_normal_size_right"), pos.right);
	the_main_app.WriteProfileInt(_T("window_size"), _T("main_frame_normal_size_bottom"), pos.bottom);
}

bool app_size_config_helper::get_maximize_flag()
{
	return (1 == the_main_app.GetProfileInt(_T("window_size"), _T("is_main_frame_maximize"), 0));
}

void app_size_config_helper::save_maxmize_flag(bool is_maximize)
{
	the_main_app.WriteProfileInt(_T("window_size"), _T("is_main_frame_maximize"), is_maximize ? 1 : 0);
}

void app_size_config_helper::save_docking_panes_config_version(const CString& ver)
{
	main_application* app = (main_application*) AfxGetApp();
	if (!app) return;

	app->WriteProfileString(_T("docking_panes_config"), _T("docking_panes_config_version"), ver);
}

CString app_size_config_helper::get_docking_panes_config_version()
{
	main_application* app = (main_application*) AfxGetApp();
	if (!app) return _T("");
	
	return app->GetProfileString(_T("docking_panes_config"), _T("docking_panes_config_version"), _T(""));
}
