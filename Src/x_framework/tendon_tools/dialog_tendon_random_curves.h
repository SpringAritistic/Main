#pragma once

#include "..\macro.h"
#include "..\windows_helper\dialog_sub_grid.h"


// 钢束-任意曲线二级表对话框类
class X_FRAMEWORK_API dialog_tendon_random_curves : public dialog_sub_grid
{
	DECLARE_DYNAMIC(dialog_tendon_random_curves)

public:
	dialog_tendon_random_curves(CWnd* parent = nullptr);
	dialog_tendon_random_curves(const CString& template_name, int default_height, int default_width, const CString& title = _T(""), CWnd* parent = nullptr);
	virtual ~dialog_tendon_random_curves();

public:
	virtual void get_data_from_ui();
	virtual void set_data_to_ui();

protected:
	void split_string_by_symbol(std::vector<CString>& result, const CString& text_entry, const TCHAR symbol);

};

