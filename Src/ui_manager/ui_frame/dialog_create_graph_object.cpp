#include "stdafx.h"
#include "project_manager/macro.h"
#include "project_manager/data_center/project_data.h"

#include "dialog_create_graph_object.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_DYNAMIC(dialog_create_graph_object, app_dialog_base)
BEGIN_MESSAGE_MAP(dialog_create_graph_object, app_dialog_base)
END_MESSAGE_MAP()

dialog_create_graph_object::dialog_create_graph_object(CWnd* pParent /*=NULL*/)
	: app_dialog_base(dialog_create_graph_object::IDD, pParent)
{
}

dialog_create_graph_object::~dialog_create_graph_object()
{
}

BOOL dialog_create_graph_object::OnInitDialog()
{
	app_dialog_base::OnInitDialog();

	CString strPaperName = _T("新图纸");
	int iCount = 0;
	for (size_t i = 0; i < m_exist_graph_names.size(); ++i)
	{
		if (m_exist_graph_names[i].Find(strPaperName) != -1) 
			++iCount;
	}
	if (iCount > 0)
		strPaperName += string_tools::num_to_string(iCount);

	m_edit_graph_name.SetWindowText(strPaperName);
	
	vector<graph_register_info*> all_gh_reg_items;
	project_data::get_graph_register_info_list(all_gh_reg_items);

	m_combo_graph_type.ResetContent();
	for (graph_register_info* x : all_gh_reg_items)
		m_combo_graph_type.AddString(x->m_type_name);

	if (m_combo_graph_type.GetCount() > 0)
		m_combo_graph_type.SetCurSel(0);

	return TRUE;
}

void dialog_create_graph_object::DoDataExchange(CDataExchange* dx)
{
	app_dialog_base::DoDataExchange(dx);

	DDX_Control(dx, IDC_EDIT_GRAPH_NAME, m_edit_graph_name);
	DDX_Control(dx, IDC_COMBO_GRAPH_TYPE, m_combo_graph_type);
}

void dialog_create_graph_object::OnOK()
{
	m_edit_graph_name.GetWindowText(m_result_graph_name);
	m_result_graph_name.Trim();
	if (m_result_graph_name.IsEmpty())
	{
		AfxMessageBox(_T("图纸名不能为空"), MB_ICONWARNING | MB_OK);
		return;
	}

	auto it = std::find_if(m_exist_graph_names.begin(), m_exist_graph_names.end(), [&](const CString& cur_name)
	{
		return (0 == cur_name.CompareNoCase(m_result_graph_name));
	});

	if (it != m_exist_graph_names.end())
	{
		AfxMessageBox(_T("图纸名已存在"), MB_ICONWARNING | MB_OK);
		return;
	}

	CString type_name;
	m_combo_graph_type.GetWindowText(type_name);
	if (type_name.IsEmpty())
	{
		AfxMessageBox(_T("图纸类型不能为空"), MB_ICONWARNING | MB_OK);
		return;
	}

	vector<graph_register_info*> all_gh_reg_items;
	project_data::get_graph_register_info_list(all_gh_reg_items);

	for (graph_register_info* x : all_gh_reg_items)
		if (x && 0 == x->m_type_name.CompareNoCase(type_name))
			m_result_type_name = x->m_type_name;

	app_dialog_base::OnOK();
}

