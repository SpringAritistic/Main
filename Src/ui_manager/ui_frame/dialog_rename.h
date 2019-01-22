#pragma once
#include "../resource.h"

#include "project_manager/macro.h"
#include "project_manager/data_center/project_data.h"

#include "x_framework/macro.h"
#include "x_framework/windows_helper/app_dialog_base.h"


class dialog_rename : public app_dialog_base
{
	DECLARE_DYNAMIC(dialog_rename)
	DECLARE_MESSAGE_MAP()

public:
	dialog_rename(CWnd* pParent = NULL);
	virtual ~dialog_rename();

public:
	enum { IDD = IDD_DIALOG_RENAME };

public:
	CString						m_result_name;
	std::vector<CString>		m_exist_names;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* dx);
	virtual void OnOK();

private:
	CEdit		m_edit_graph_name;
};
