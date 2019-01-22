#pragma once
#include "../resource.h"

#include "project_manager/macro.h"
#include "project_manager/data_center/project_data.h"

#include "x_framework/macro.h"
#include "x_framework/windows_helper/app_dialog_base.h"


class dialog_create_graph_object : public app_dialog_base
{
	DECLARE_DYNAMIC(dialog_create_graph_object)
	DECLARE_MESSAGE_MAP()

public:
	dialog_create_graph_object(CWnd* pParent = NULL);
	virtual ~dialog_create_graph_object();

public:
	enum { IDD = IDD_DIALOG_CREATE_GRAPH_OBJECT };

public:
	CString								m_result_graph_name;
	CString								m_result_type_name;
	std::vector<CString>				m_exist_graph_names;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* dx);
	virtual void OnOK();

private:
	CEdit		m_edit_graph_name;
	CComboBox	m_combo_graph_type;
};
